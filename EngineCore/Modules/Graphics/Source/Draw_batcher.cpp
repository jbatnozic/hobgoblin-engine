#include <Hobgoblin/Graphics/Draw_batcher.hpp>

#include <cassert>
#include <typeinfo>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

// VERTICES ALWAYS COUNTER-CLOCKWISE!

namespace {
void AppendSpriteToVertexArray(const Sprite& aSprite, VertexArray& aVertexArray) {
#if 0
    Vertex vert[4]; // top-left, top-right, bottom-right, bottom-left

    const auto transform = aSprite.getTransform();

    const auto lb = aSprite.getLocalBounds();

    vert[0].position = transform * sf::Vector2f{lb.left,            lb.top};
    vert[1].position = transform * sf::Vector2f{lb.left + lb.width, lb.top};
    vert[2].position = transform * sf::Vector2f{lb.left + lb.width, lb.top + lb.height};
    vert[3].position = transform * sf::Vector2f{lb.left,            lb.top + lb.height};

    const auto tr = aSprite.getTextureRect();

    vert[0].texCoords = {(float)tr.left, (float)tr.top};
    vert[1].texCoords = {(float)tr.left + (float)tr.width, (float)tr.top};
    vert[2].texCoords = {(float)tr.left + (float)tr.width, (float)tr.top + (float)tr.height};
    vert[3].texCoords = {(float)tr.left, (float)tr.top + (float)tr.height};

    vert[0].color = vert[1].color = vert[2].color = vert[3].color = aSprite.getColor();

    for (const auto& v : vert) {
        aVertexArray.append(v);
    }
#endif
}

bool operator==(const RenderStates& aStates1, const RenderStates& aStates2) {
    return (aStates1.texture   == aStates2.texture   &&
            aStates1.blendMode == aStates2.blendMode &&
            aStates1.transform == aStates2.transform &&
            aStates1.shader    == aStates2.shader);
}

bool operator!=(const RenderStates& aStates1, const RenderStates& aStates2) {
    return (!(aStates1 == aStates2));
}
} // namespace

DrawBatcher::DrawBatcher(Canvas& aCanvas)
    : _canvas{aCanvas}
{
}

//void DrawBatcher::draw(const Drawable& aDrawable,
//                       const RenderStates& aStates) {
//    detail::Drawable_DrawOntoCanvas(aDrawable, SELF, aStates);
//}

void DrawBatcher::draw(const Drawable& aDrawable,
                       const RenderStates& aStates) {
#if 0
    // Enable batching for sprites and shapes

    const auto batchingType = aDrawable.getBatchingType();
    switch (batchingType) {
    case Drawable::BatchingType::VertexBuffer:
        assert(typeid(aDrawable) == typeid(VertexBuffer));
        // TODO - this branch should never happen
        break;

    case Drawable::BatchingType::VertexArray:
        assert(typeid(aDrawable) == typeid(VertexArray));
        // TODO
        break;

    case Drawable::BatchingType::Sprite:
        assert(typeid(aDrawable) == typeid(Sprite));
        _drawSprite(static_cast<const Sprite&>(aDrawable), aStates);
        break;

    case Drawable::BatchingType::Aggregate:
        detail::Drawable_DrawOntoCanvas(aDrawable, SELF, aStates);
        break;

    case Drawable::BatchingType::Custom:
        break;
    }

    // Check if it's a sprite
    if (typeid(aDrawable) == typeid(sf::Sprite)) {
        const auto& sprite = static_cast<const sf::Sprite&>(aDrawable);

        switch (_status) {
        default:
            _flush();
            SWITCH_FALLTHROUGH;

        case Status::Empty:
            SWITCH_FALLTHROUGH;

        case Status::BatchingSprites:
            if (_status == Status::Empty) {
                _prepForBatchingSprites(aStates, sprite.getTexture());
            }
            else {
                // If batches are not compatible, flush and start a new batch
                if (aStates != _renderStates || sprite.getTexture() != _texture) {
                    _flush();
                    _prepForBatchingSprites(aStates, sprite.getTexture());
                }
            }

            AppendSpriteToVertexArray(sprite, _vertexArray);
            break;
        }

        return;
    }

    // Check if it's a vertex array
    if (typeid(aDrawable) == typeid(sf::Sprite)) {
        const auto vertexArray = static_cast<const sf::VertexArray&>(aDrawable);

        this->draw(
            std::addressof(vertexArray[0u]),
            vertexArray.getVertexCount(),
            vertexArray.getPrimitiveType(),
            aStates
        );

        return;
    }

    // Otherwise...
    _flush();
    _canvas.draw(aDrawable, aStates);
#endif
}

void DrawBatcher::draw(const Vertex* aVertices,
                       PZInteger aVertexCount,
                       PrimitiveType aType,
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
        }
        else {
            // If batches are not compatible, flush and start a new batch
            if (aStates != _renderStates || _vertexArray.primitiveType != aType) {
                _flush();
                _prepForBatchingVertices(aStates, aType);
            }
        }

        for (std::size_t i = 0; i < aVertexCount; i += 1) {
            _vertexArray.vertices.push_back(aVertices[i]);
        }
        break;
    }
}

void DrawBatcher::draw(const VertexBuffer& aVertexBuffer,
                       const RenderStates& aStates) {
    // Always draw vertex buffers individually
    _flush();
    _canvas.draw(aVertexBuffer, aStates);
}

void DrawBatcher::draw(const VertexBuffer& aVertexBuffer,
                       PZInteger aFirstVertex,
                       PZInteger aVertexCount,
                       const RenderStates& aStates) {
    // Always draw vertex buffers individually
    _flush();
    _canvas.draw(aVertexBuffer, aFirstVertex, aVertexCount, aStates);
}

void DrawBatcher::flush() {
    _flush();
    _canvas.flush();
}

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
    // Quads only supported on desktop, for mobile should use 2 triangles...
    _vertexArray.primitiveType = PrimitiveType::Triangles;
    _renderStates = aStates;
    _texture = aTexture;

    _status = Status::BatchingSprites;
}

void DrawBatcher::_prepForBatchingVertices(const RenderStates& aStates, PrimitiveType aType) {
    _vertexArray.primitiveType = aType;
    _renderStates = aStates;

    _status = Status::BatchingVertices;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
