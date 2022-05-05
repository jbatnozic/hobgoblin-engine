#ifndef UHOBGOBLIN_GR_DRAW_BATCHER_HPP
#define UHOBGOBLIN_GR_DRAW_BATCHER_HPP

#include <Hobgoblin/Graphics/Canvas.hpp>

#include <SFML/Graphics.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class DrawBatcher : public Canvas {
public:
    explicit DrawBatcher(Canvas& aCanvas);

    void draw(const Drawable& aDrawable,
              const sf::RenderStates& aStates = sf::RenderStates::Default) override;

    void draw(const sf::Drawable& drawable,
              const sf::RenderStates& states = sf::RenderStates::Default) override;

    void draw(const sf::Vertex* vertices, 
              std::size_t vertexCount, 
              sf::PrimitiveType type,
              const sf::RenderStates& states = sf::RenderStates::Default) override;

    void draw(const sf::VertexBuffer& vertexBuffer,
              const sf::RenderStates& states = sf::RenderStates::Default) override;

    void draw(const sf::VertexBuffer& vertexBuffer, 
              std::size_t firstVertex, 
              std::size_t vertexCount,
              const sf::RenderStates& states = sf::RenderStates::Default) override;

    void flush() override;

private:
    enum class Status {
        Empty,
        BatchingSprites,
        BatchingVertices
    };

    Canvas& _canvas;

    Status _status = Status::Empty;

    sf::RenderStates _renderStates;
    sf::VertexArray _vertexArray;
    const sf::Texture* _texture = nullptr;

    void _flush();

    void _prepForBatchingSprites(const sf::RenderStates& aStates, const sf::Texture* aTexture);

    void _prepForBatchingVertices(const sf::RenderStates& aStates, sf::PrimitiveType aType);
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_DRAW_BATCHER_HPP
