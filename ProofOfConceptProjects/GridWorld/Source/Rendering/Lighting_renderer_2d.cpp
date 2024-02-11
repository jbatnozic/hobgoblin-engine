
#include <GridWorld/Rendering/Lighting_renderer_2d.hpp>
#include <GridWorld/Coord_conversion.hpp>

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Math.hpp>

#include <GL/glew.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <vector>

static constexpr auto LOG_ID = "gridworld";

static void ClearOpenGLErrors() {
    while (glGetError() != GL_NO_ERROR) {}
}

static void LogOpenGLErrors(const char* aFile, int aLine) {
    while (auto error = glGetError()) {
        HG_LOG_ERROR(LOG_ID, "[OpenGL ERROR] code: {} file: {}:{}", (int)error, aFile, aLine);
    }
}

#define GLCALL(...) \
    do { \
        ClearOpenGLErrors(); \
        (__VA_ARGS__); \
        LogOpenGLErrors(__FILE__, __LINE__); \
    } while (false)
//#define GLCALL(...) __VA_ARGS__

namespace gridworld {

namespace {
const auto ADDITIVE_DRAW = hg::gr::RenderStates{hg::gr::BLEND_ADD};

float MultiplierForPurpose(LightingRenderer2D::Purpose aPurpose) {
    switch (aPurpose) {
        case LightingRenderer2D::FOR_TOPDOWN:
            return 1.f;

        case LightingRenderer2D::FOR_DIMETRIC:
            //return 2.f;
            return 1.41421356237f; // sqrt(2)

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
    , _textureSize{aTextureSize}
    , _sizeMultiplier{MultiplierForPurpose(aPurpose)}
{
    _renderTexture.create({aTextureSize, aTextureSize});

    const auto size = hg::pztos(_textureSize) * hg::pztos(_textureSize) * 4u;
    _imageData.resize(size);

#if 0
    // Generate 1st buffer and attach texture to it
    GLCALL(glGenFramebuffers(1, &_framebuffers[0]));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, _framebuffers[0]));
    GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTexture.getTexture().getNativeHandle(), 0));

    // Generate 2nd buffer and attach texture to it
    GLCALL(glGenFramebuffers(1, &_framebuffers[1]));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, _framebuffers[1]));
    GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTexture.getTexture().getNativeHandle(), 0));

    // Unbind buffer
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
#endif


    GLCALL(glGenBuffers(1, &_pbo));
    GLCALL(glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo));
    GLCALL(glBufferData(GL_PIXEL_PACK_BUFFER, aTextureSize * aTextureSize * 4, NULL, GL_STREAM_READ));
    GLCALL(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
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
}

void LightingRenderer2D::render() {
    if (_step % 2 == 0) {
        _step += 1;
        goto RENDER;
    } else {
        _step += 1;
        goto COPY;
    }

    RENDER:
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

    {
        const auto t1 = std::chrono::steady_clock::now();
        _renderTexture.display();

        // bind PBO
        glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo); 

        // bind texture
        glBindTexture(GL_TEXTURE_2D, _renderTexture.getTexture().getNativeHandle());

        // transfer texture into PBO
        glGetTexImage(/* target */      GL_TEXTURE_2D,
                      /* level */       0,
                      /* pixelformat */ GL_RGBA,
                      /* pixeltype */   GL_UNSIGNED_BYTE,
                      /* offset */      nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0); 

        const auto t2 = std::chrono::steady_clock::now();
        HG_LOG_INFO(LOG_ID, "display() took {}ms.", std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1000.0);
    }
    return;

    COPY:
    {
        const auto t1 = std::chrono::steady_clock::now();
        //_image = _renderTexture.getTexture().copyToImage();

        const void* p = nullptr;
        glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo);
        GLCALL(p = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
        HG_HARD_ASSERT(p != nullptr);
        const auto size = hg::pztos(_textureSize) * hg::pztos(_textureSize) * 4u;
        std::memcpy(_imageData.data(), p, size);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

        const auto t2 = std::chrono::steady_clock::now();
        HG_LOG_INFO(LOG_ID, "copyToImage() took {}ms.", std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1000.0);
    }
}

std::optional<hg::gr::Color> LightingRenderer2D::getColorAt(hg::math::Vector2f aPos) const {
    auto pixelPos = _renderTexture.mapCoordsToPixel(aPos, 0);

    if (pixelPos.x < 0 || pixelPos.x >= _textureSize ||
        pixelPos.y < 0 || pixelPos.y >= _textureSize)
    {
        return {};
    }

    const auto* p = _imageData.data() + ((_textureSize - 1 - pixelPos.y) * _textureSize + pixelPos.x) * 4;

    return hg::gr::Color{p[0], p[1], p[2], p[3]};
}

const hg::gr::Texture& LightingRenderer2D::getTexture(hg::math::Vector2f* aRecommendedScale) const {
    if (aRecommendedScale != nullptr) {
        aRecommendedScale->x = _recommendedScale;
        aRecommendedScale->y = _recommendedScale;
    }
    return _renderTexture.getTexture();
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
