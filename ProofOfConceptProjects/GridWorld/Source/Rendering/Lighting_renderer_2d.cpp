
#include <GridWorld/Rendering/Lighting_renderer_2d.hpp>
#include <GridWorld/Coord_conversion.hpp>

#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Math.hpp>

#include <GL/glew.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <type_traits>

namespace {
constexpr auto LOG_ID = "gridworld";
} // namespace

///////////////////////////////////////////////////////////////////////////
// OPENGL ERROR HANDLING                                                 //
///////////////////////////////////////////////////////////////////////////
namespace {
struct OpenGLErrorContext {
    void addError(int aErrorCode, const char* aCodeString) {
        messageCount  += 1;
        errorMessages += fmt::format("{}. Code '{}' resulted in {:#X}; ", messageCount, aCodeString, aErrorCode);
    }

    bool hasErrors() const {
        return messageCount > 0;
    }

    std::string errorMessages;
    int         messageCount = 0;
};

#define THROW_ON_ERROR(_error_context_) \
    do { \
        if ((_error_context_).hasErrors()) { \
            HG_THROW_TRACED(::jbatnozic::hobgoblin::TracedRuntimeError, \
                            0, \
                            "There were {} OpenGL errors: {}", \
                            (_error_context_).messageCount, \
                            (_error_context_).errorMessages); \
        } \
    } while (false)

void ClearOpenGLErrors() {
    while (glGetError() != GL_NO_ERROR) {}
}

void StoreOpenGLErrors(OpenGLErrorContext& aErrorCtx, const char* aCodeString) {
    while (auto error = glGetError()) {
        aErrorCtx.addError(error, aCodeString);
    }
}

#define GLCALL(_error_context_, _code_) \
    do { \
        ClearOpenGLErrors(); \
        { _code_ ; } \
        StoreOpenGLErrors(_error_context_, #_code_); \
    } while (false)
} // namespace

static_assert(std::is_same_v<unsigned int, GLuint>,
              "Adjust type of LightingRenderer2D::_pboNames");

///////////////////////////////////////////////////////////////////////////
// GRIDWORLD                                                             //
///////////////////////////////////////////////////////////////////////////
namespace gridworld {

namespace {
const auto ADDITIVE_DRAW = hg::gr::RenderStates{hg::gr::BLEND_ADD};

float MultiplierForPurpose(LightingRenderer2D::Purpose aPurpose) {
    switch (aPurpose) {
        case LightingRenderer2D::FOR_TOPDOWN:
            return 1.f;

        case LightingRenderer2D::FOR_DIMETRIC:
            {
                // (17/16) * sqrt(2)
                static constexpr float MAGIC_DIMETRIC_MULTIPLIER = 1.41421356237f * 17.f / 16.f;
                return MAGIC_DIMETRIC_MULTIPLIER;
            }

        default:
            HG_UNREACHABLE("Invalid value for LightingRenderer2D::Purpose ({}).", (int)aPurpose);
    }
}
} // namespace

LightingRenderer2D::LightingRenderer2D(const World& aWorld,
                                       const hg::gr::SpriteLoader& aSpriteLoader,
                                       hg::PZInteger aTextureSize,
                                       Purpose aPurpose)
    : _world{aWorld}
    , _spriteLoader{aSpriteLoader}
    , _sizeMultiplier{MultiplierForPurpose(aPurpose)}
    , _textureSize{aTextureSize}
{
    _renderTexture.create({aTextureSize, aTextureSize});

    // Allocate Texture RAM Buffer:
    const auto size = hg::pztos(_textureSize) * hg::pztos(_textureSize) * 4u;
    _textureRamBuffer.resize(size);
    std::memset(_textureRamBuffer.data(), 0x00, _textureRamBuffer.size());

    // Generate and configure PBOs:
    OpenGLErrorContext ectx;
    for (auto& pboName : _pboNames) {
        pboName = 0;
        GLCALL(ectx, glGenBuffers(1, &pboName));
        GLCALL(ectx, glBindBuffer(GL_PIXEL_PACK_BUFFER, pboName));
        GLCALL(ectx, glBufferData(GL_PIXEL_PACK_BUFFER, _textureRamBuffer.size(), NULL, GL_STREAM_READ));
    }

    // Unbind PBOs for now:
    GLCALL(ectx, glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));

    // Handle errors:
    if (ectx.hasErrors()) {
        _deletePbos();
        THROW_ON_ERROR(ectx);
    }
}

LightingRenderer2D::~LightingRenderer2D() {
    _deletePbos();
}

void LightingRenderer2D::start(hg::math::Vector2f aWorldPosition,
                               hg::math::Vector2f aViewSize,
                               float aPadding) {
    _renderTexture.clear();

    const float width  = aViewSize.x + aPadding;
    const float height = aViewSize.y + aPadding;
    const float largerDimension = std::max(width, height);

    _recommendedScale = largerDimension * _sizeMultiplier / _renderTexture.getSize().x;

    const float virtualSquareEdge = ceil(largerDimension * _sizeMultiplier);

    hg::gr::View view;
    view.setSize({virtualSquareEdge, virtualSquareEdge});
    view.setCenter(aWorldPosition);
    view.setViewport({0.f, 0.f, 1.f, 1.f});
    _renderTexture.setView(view);

    _stepCounter += 1;
}

void LightingRenderer2D::render() {
    for (auto iter = _world.lightDataBegin(), end = _world.lightDataEnd(); iter != end; iter = std::next(iter)) {
        auto& light = iter->second; // TODO(check that this does not do a copy)
        _renderLight(light);
        _drawLight(light);
    }

    // TODO (remove when not needed anymore)
#if 0
    {
        hg::gr::CircleShape circle{4.f};
        circle.setOrigin({2.f, 2.f});
        circle.setFillColor(hg::gr::COLOR_RED);
        circle.setPosition(_renderTexture.getView().getCenter());
        _renderTexture.draw(circle);
    }
#endif

    OpenGLErrorContext ectx;

    /* DISPLAY & START TRANSFER TO PBO */ {
        const auto t1 = std::chrono::steady_clock::now();
        _renderTexture.display();

        const unsigned targetPbo = (_stepCounter % 2);
        GLCALL(ectx, glBindBuffer(GL_PIXEL_PACK_BUFFER, _pboNames[targetPbo]));
        GLCALL(ectx, glBindTexture(GL_TEXTURE_2D, _renderTexture.getTexture().getNativeHandle()));

        // Starts the asychronous transfer of texture pixel data into the PBO.
        // This transfer will happen whenever is suitable for the GPU, as to now
        // stall the graphics pipeline.
        GLCALL(ectx, glGetTexImage(/* target */      GL_TEXTURE_2D,
                                   /* level */       0,
                                   /* pixelformat */ GL_RGBA,
                                   /* pixeltype */   GL_UNSIGNED_BYTE,
                                   /* offset */      nullptr));

        GLCALL(ectx, glBindTexture(GL_TEXTURE_2D, 0));
        GLCALL(ectx, glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));

        THROW_ON_ERROR(ectx);

        const auto t2 = std::chrono::steady_clock::now();
        //HG_LOG_INFO(LOG_ID, "display() took {}ms.", std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1000.0);
    }

    /* LOAD PBO INTO RAM BUFFER */ {
        const auto t1 = std::chrono::steady_clock::now();

        const unsigned targetPbo = ((_stepCounter + 1) % 2);

        const void* p = nullptr;
        GLCALL(ectx, glBindBuffer(GL_PIXEL_PACK_BUFFER, _pboNames[targetPbo]));
        // Maps the memory of the target PBO into memory addressable by the CPU.
        // If a transfer into this PBO previously initiated by 'glGetTexImage' is
        // not yet complete, 'glMapBuffer' will wait until it is.
        // Note: Where the PBO holds pixel data and whether 'glMapBuffer' involves
        // copying the data is up to the specific OpenGL implementation.
        GLCALL(ectx, {
            p = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
        });

        if (p != nullptr) {
            std::memcpy(_textureRamBuffer.data(), p, _textureRamBuffer.size());
        } else {
            HG_LOG_WARN(LOG_ID, "PBO mapping into RAM returned NULL.");
            std::memset(_textureRamBuffer.data(), 0x00, _textureRamBuffer.size());
        }

        GLCALL(ectx, glUnmapBuffer(GL_PIXEL_PACK_BUFFER));
        GLCALL(ectx, glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));

        THROW_ON_ERROR(ectx);

        const auto t2 = std::chrono::steady_clock::now();
        //HG_LOG_INFO(LOG_ID, "copyToImage() took {}ms.", std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1000.0);
    }
}

std::optional<hg::gr::Color> LightingRenderer2D::getColorAt(hg::math::Vector2f aPos) const {
    auto pixelPos = _renderTexture.mapCoordsToPixel(aPos, 0);

    if (pixelPos.x < 0 || pixelPos.x >= _textureSize ||
        pixelPos.y < 0 || pixelPos.y >= _textureSize)
    {
        return {};
    }

    // Note: The calculation is as such because the pixel data we get from OpenGL is flipped vertically.
    const auto* p = _textureRamBuffer.data() + ((_textureSize - 1 - pixelPos.y) * _textureSize + pixelPos.x) * 4;

    return hg::gr::Color{p[0], p[1], p[2], 255};
}

const hg::gr::Texture& LightingRenderer2D::getTexture(hg::math::Vector2f* aRecommendedScale) const {
    if (aRecommendedScale != nullptr) {
        aRecommendedScale->x = _recommendedScale;
        aRecommendedScale->y = _recommendedScale;
    }
    return _renderTexture.getTexture();
}

void LightingRenderer2D::_deletePbos() {
    for (auto& pboName : _pboNames) {
        // OpenGL error handling deliberately left out:
        // "glDeleteBuffers silently ignores 0's and names that
        // do not correspond to existing buffer objects."
        glDeleteBuffers(1, &pboName);
        pboName = 0;
    }
}

hg::gr::Sprite& LightingRenderer2D::_getSprite(model::SpriteId aSpriteId) const {
    const auto iter = _spriteCache.find(aSpriteId);
    if (iter != _spriteCache.end()) {
        return iter->second;
    }

    const auto blueprint = _spriteLoader.getBlueprint(aSpriteId);
    const auto newIter = _spriteCache.emplace(std::make_pair(aSpriteId, blueprint.spr()));
    return newIter.first->second;
}

void LightingRenderer2D::_renderLight(const model::LightData& aLightData) {
    const auto size = aLightData.texture.getSize();
    const auto cellResolution = _world.getCellResolution();

    auto& view = aLightData.texture.getView();
    view.setSize({(float)size.x, (float)size.y}); // TODO: call only once on creation
    view.setCenter(aLightData.position);
    view.setViewport({0.f, 0.f, 1.f, 1.f});

    aLightData.texture.clear(hg::gr::COLOR_BLACK);

    {
        auto& sprite = _getSprite(aLightData.spriteId);
        sprite.setPosition(aLightData.position);
        aLightData.texture.draw(sprite);
    }

    hg::gr::Vertex vertices[10];
    for (auto& vertex : vertices) {
        vertex.color = hg::gr::COLOR_BLACK;
    }

    const auto startGridX = 
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.x - size.x / 2.f) / cellResolution)), 
                        0, 
                        _world.getCellCountX() - 1
        );
    const auto startGridY =
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.y - size.y / 2.f) / cellResolution)),
                        0,
                        _world.getCellCountY() - 1
        );
    const auto endGridX = 
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.x + size.x / 2.f) / cellResolution)), 
                        0, 
                        _world.getCellCountX() - 1
        );
    const auto endGridY =
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.y + size.y / 2.f) / cellResolution)),
                        0,
                        _world.getCellCountY() - 1
        );

    for (hg::PZInteger y = startGridY; y <= endGridY; y += 1) {
        for (hg::PZInteger x = startGridX; x <= endGridX; x += 1) {
            // ****************************************************
            if (auto wall = _world.getCellAtUnchecked(x, y).wall) {
                vertices[0].position = { x      * cellResolution,  y      * cellResolution};
                vertices[2].position = { x      * cellResolution, (y + 1) * cellResolution};
                vertices[4].position = {(x + 1) * cellResolution, (y + 1) * cellResolution};
                vertices[6].position = {(x + 1) * cellResolution,  y      * cellResolution};
                vertices[8] = vertices[0];

                for (int i = 1; i < 10; i += 2) {
                    hg::math::Vector2f diff = {vertices[i - 1].position.x - aLightData.position.x,
                        vertices[i - 1].position.y - aLightData.position.y};
                    diff.x *= 1000.f;
                    diff.y *= 1000.f;

                    vertices[i].position = {vertices[i - 1].position.x + diff.x,
                        vertices[i - 1].position.y + diff.y};
                }

                aLightData.texture.draw(vertices, 10, hg::gr::PrimitiveType::TriangleStrip);
            }
            // ****************************************************
        }
    }

    aLightData.texture.display();
}

void LightingRenderer2D::_drawLight(const model::LightData& aLightData) {
    hg::gr::Sprite spr{&(aLightData.texture.getTexture())};
    const auto bounds = spr.getLocalBounds();
    spr.setOrigin({bounds.w / 2.f, bounds.h / 2.f});
    spr.setPosition(aLightData.position);
    spr.setRotation(aLightData.angle);
    _renderTexture.draw(spr, ADDITIVE_DRAW);
}

} // namespace gridworld
