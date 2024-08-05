// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Graphics/Draw_batcher.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <array>
#include <cassert>
#include <typeinfo>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

// VERTICES ALWAYS CLOCKWISE!

namespace {
// clang-format off
void AppendSpriteToVertexArray(const Sprite& aSprite, VertexArray& aVertexArray) {
    Vertex vert[6]; // FIRST: top-left, top-right, bottom-left
                    //  THEN: top-right, bottom-right, bottom-left

    const auto transform = aSprite.getTransform();

    const auto lb = aSprite.getLocalBounds();

    vert[0].position = transform * math::Vector2f{lb.x,        lb.y};
    vert[1].position = transform * math::Vector2f{lb.x + lb.w, lb.y};
    vert[2].position = transform * math::Vector2f{lb.x,        lb.y + lb.h};
    vert[3].position = transform * math::Vector2f{lb.x + lb.w, lb.y};
    vert[4].position = transform * math::Vector2f{lb.x + lb.w, lb.y + lb.h};
    vert[5].position = transform * math::Vector2f{lb.x,        lb.y + lb.h};

    const auto tr = aSprite.getTextureRect();

    vert[0].texCoords = {(float)tr.x,               (float)tr.y};
    vert[1].texCoords = {(float)tr.x + (float)tr.w, (float)tr.y};
    vert[2].texCoords = {(float)tr.x,               (float)tr.y + (float)tr.h};
    vert[3].texCoords = {(float)tr.x + (float)tr.w, (float)tr.y};
    vert[4].texCoords = {(float)tr.x + (float)tr.w, (float)tr.y + (float)tr.h};
    vert[5].texCoords = {(float)tr.x,               (float)tr.y + (float)tr.h};

    vert[0].color = vert[1].color = vert[2].color = 
    vert[3].color = vert[4].color = vert[5].color = aSprite.getColor();

    aVertexArray.vertices.reserve(aVertexArray.vertices.size() + 6);
    for (const auto& v : vert) {
        aVertexArray.vertices.push_back(v);
    }
}

bool operator==(const RenderStates& aStates1, const RenderStates& aStates2) {
    return (aStates1.texture   == aStates2.texture   &&
            aStates1.blendMode == aStates2.blendMode &&
            aStates1.transform == aStates2.transform &&
            aStates1.shader    == aStates2.shader);
}
// clang-format on

bool operator!=(const RenderStates& aStates1, const RenderStates& aStates2) {
    return (!(aStates1 == aStates2));
}

bool IsBatchable(PrimitiveType aPrimitiveType) {
    struct Mapping {
        std::array<bool, (unsigned)PrimitiveType::COUNT> table;

        constexpr Mapping() {
            table[(unsigned)PrimitiveType::POINTS]         = true;
            table[(unsigned)PrimitiveType::LINES]          = true;
            table[(unsigned)PrimitiveType::LINE_STRIP]     = false;
            table[(unsigned)PrimitiveType::TRIANGLES]      = true;
            table[(unsigned)PrimitiveType::TRIANGLE_STRIP] = false;
            table[(unsigned)PrimitiveType::TRIANGLE_FAN]   = false;
        }
    };

    static constexpr Mapping mapping{};

#ifdef UHOBGOBLIN_DEBUG
    return mapping.table.at((unsigned)aPrimitiveType);
#else
    return mapping.table[(unsigned)aPrimitiveType];
#endif
}
} // namespace

DrawBatcher::DrawBatcher(Canvas& aCanvas)
    : _canvas{aCanvas} {}

math::Vector2pz DrawBatcher::getSize() const {
    return _canvas.getSize();
}

void DrawBatcher::getCanvasDetails(CanvasType& aType, void*& aRenderingBackend) {
    _canvas.getCanvasDetails(aType, aRenderingBackend);
}

bool DrawBatcher::isSrgb() const {
    return _canvas.isSrgb();
}

///////////////////////////////////////////////////////////////////////////
// CANVAS - DRAWING                                                      //
///////////////////////////////////////////////////////////////////////////

void DrawBatcher::clear(const Color& aColor) {
    _canvas.clear(aColor);
}

void DrawBatcher::draw(const Drawable& aDrawable, const RenderStates& aStates) {
    const auto batchingType = aDrawable.getBatchingType();
    switch (batchingType) {
    case Drawable::BatchingType::VertexBuffer:
        // TODO - this branch should never happen
        assert(false);
        break;

    case Drawable::BatchingType::VertexArray:
        assert(typeid(aDrawable) == typeid(VertexArray));
        _drawVertexArray(static_cast<const VertexArray&>(aDrawable), aStates);
        break;

    case Drawable::BatchingType::Sprite:
        assert(typeid(aDrawable) == typeid(Sprite));
        _drawSprite(static_cast<const Sprite&>(aDrawable), aStates);
        break;

    case Drawable::BatchingType::Aggregate:
        _canvas.draw(aDrawable, aStates);
        break;

    case Drawable::BatchingType::Custom:
        _flush();
        _canvas.draw(aDrawable, aStates);
        break;

    default:
        HG_UNREACHABLE("Invalid batching type encountered ({}).", (int)batchingType);
    }
}

void DrawBatcher::draw(const Vertex*       aVertices,
                       PZInteger           aVertexCount,
                       PrimitiveType       aType,
                       const RenderStates& aStates) {
    switch (_status) {
    default:
        _flush();
        SWITCH_FALLTHROUGH;

    case Status::Empty:
        SWITCH_FALLTHROUGH;

    case Status::BatchingVertices:
        if (_status == Status::Empty) {
            _prepForBatchingVertices(aStates, aType);
        } else {
            // If batches are not compatible, flush and start a new batch
            if (_vertexArray.primitiveType != aType || !IsBatchable(aType) || aStates != _renderStates) {
                _flush();
                _prepForBatchingVertices(aStates, aType);
            }
        }

        for (PZInteger i = 0; i < aVertexCount; i += 1) {
            _vertexArray.vertices.push_back(aVertices[pztos(i)]);
        }
        break;
    }
}

void DrawBatcher::draw(const VertexBuffer& aVertexBuffer, const RenderStates& aStates) {
    // Always draw vertex buffers individually
    _flush();
    _canvas.draw(aVertexBuffer, aStates);
}

void DrawBatcher::draw(const VertexBuffer& aVertexBuffer,
                       PZInteger           aFirstVertex,
                       PZInteger           aVertexCount,
                       const RenderStates& aStates) {
    // Always draw vertex buffers individually
    _flush();
    _canvas.draw(aVertexBuffer, aFirstVertex, aVertexCount, aStates);
}

void DrawBatcher::flush() {
    _flush();
    _canvas.flush();
}

///////////////////////////////////////////////////////////////////////////
// CANVAS - OPEN GL                                                      //
///////////////////////////////////////////////////////////////////////////

[[nodiscard]] bool DrawBatcher::setActive(bool aActive) {
    return _canvas.setActive(aActive);
}

void DrawBatcher::pushGLStates() {
    _canvas.pushGLStates();
}

void DrawBatcher::popGLStates() {
    _canvas.popGLStates();
}

void DrawBatcher::resetGLStates() {
    _canvas.resetGLStates();
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

void DrawBatcher::_flush() {
    switch (_status) {
    case Status::Empty:
        return;

    case Status::BatchingSprites:
        _renderStates.texture = _texture;
        _canvas.draw(_vertexArray, _renderStates);
        _vertexArray.vertices.clear();
        break;

    case Status::BatchingVertices:
        _canvas.draw(_vertexArray, _renderStates);
        _vertexArray.vertices.clear();
        break;

    default:
        break;
    }

    _status = Status::Empty;
}

void DrawBatcher::_prepForBatchingSprites(const RenderStates& aStates, const Texture* aTexture) {
    _vertexArray.primitiveType = PrimitiveType::TRIANGLES;
    _renderStates              = aStates;
    _texture                   = aTexture;

    _status = Status::BatchingSprites;
}

void DrawBatcher::_prepForBatchingVertices(const RenderStates& aStates, PrimitiveType aType) {
    _vertexArray.primitiveType = aType;
    _renderStates              = aStates;

    _status = Status::BatchingVertices;
}

void DrawBatcher::_drawVertexArray(const VertexArray& aVertexArray, const RenderStates& aStates) {
    this->draw(aVertexArray.vertices.data(),
               stopz(aVertexArray.vertices.size()),
               aVertexArray.primitiveType,
               aStates);
}

void DrawBatcher::_drawSprite(const Sprite& aSprite, const RenderStates& aStates) {
    switch (_status) {
    default:
        _flush();
        SWITCH_FALLTHROUGH;

    case Status::Empty:
        SWITCH_FALLTHROUGH;

    case Status::BatchingSprites:
        if (_status == Status::Empty) {
            _prepForBatchingSprites(aStates, aSprite.getTexture());
        } else {
            // If batches are not compatible, flush and start a new batch
            if (aSprite.getTexture() != _texture || aStates != _renderStates) {
                _flush();
                _prepForBatchingSprites(aStates, aSprite.getTexture());
            }
        }

        AppendSpriteToVertexArray(aSprite, _vertexArray);
        break;
    }
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
