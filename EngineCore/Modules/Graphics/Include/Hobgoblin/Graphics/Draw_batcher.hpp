#ifndef UHOBGOBLIN_GR_DRAW_BATCHER_HPP
#define UHOBGOBLIN_GR_DRAW_BATCHER_HPP

#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Primitive_type.hpp>
#include <Hobgoblin/Graphics/Render_states.hpp>
#include <Hobgoblin/Graphics/Sprite.hpp>
#include <Hobgoblin/Graphics/Vertex_array.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class DrawBatcher : public Canvas {
public:
    explicit DrawBatcher(Canvas& aCanvas);

    void draw(const Drawable& aDrawable,
              const RenderStates& aStates = RenderStates::DEFAULT) override;

    void draw(const Vertex* aVertices, 
              PZInteger aVertexCount, 
              PrimitiveType aType,
              const RenderStates& aStates = RenderStates::DEFAULT) override;

    void draw(const VertexBuffer& aVertexBuffer,
              const RenderStates& aStates = RenderStates::DEFAULT) override;

    void draw(const VertexBuffer& aVertexBuffer, 
              PZInteger aFirstVertex, 
              PZInteger aVertexCount,
              const RenderStates& aStates = RenderStates::DEFAULT) override;

    void flush() override;

    void getCanvasDetails(CanvasType& aType, void*& aRenderingBackend) override;

private:
    enum class Status {
        Empty,
        BatchingSprites,
        BatchingVertices
    };

    Canvas& _canvas;

    Status _status = Status::Empty;

    RenderStates _renderStates;
    VertexArray _vertexArray;
    const Texture* _texture = nullptr;

    void _flush();

    void _prepForBatchingSprites(const RenderStates& aStates, const Texture* aTexture);
    void _prepForBatchingVertices(const RenderStates& aStates, PrimitiveType aType);

    void _drawVertexArray(const VertexArray& aVertexArray, const RenderStates& aStates);
    void _drawSprite(const Sprite& aSprite, const RenderStates& aStates);
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_DRAW_BATCHER_HPP
