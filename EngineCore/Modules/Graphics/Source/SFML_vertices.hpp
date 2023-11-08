#ifndef UHOBGOBLIN_GRAPHICS_SFML_VERTICES_HPP
#define UHOBGOBLIN_GRAPHICS_SFML_VERTICES_HPP

#include <Hobgoblin/Graphics/Vertex.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <cstddef>
#include <cstdlib>
#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class SFMLVertices {
public:
    SFMLVertices(const Vertex* aVertices, std::size_t aVertexCount)
        : _vertices{aVertices}, _vertexCount{aVertexCount}
    {
    }

    const sf::Vertex* getVertices() const {
#define ENSURE(_expr_) static_assert(_expr_, #_expr_)

        ENSURE(sizeof(Vertex) == sizeof(sf::Vertex));
        ENSURE(offsetof(Vertex, position)  == offsetof(sf::Vertex, position));
        ENSURE(offsetof(Vertex, color)     == offsetof(sf::Vertex, color));
        ENSURE(offsetof(Vertex, texCoords) == offsetof(sf::Vertex, texCoords));
        ENSURE(sizeof(std::declval<Vertex>().position)  == sizeof(std::declval<sf::Vertex>().position));
        ENSURE(sizeof(std::declval<Vertex>().color)     == sizeof(std::declval<sf::Vertex>().color));
        ENSURE(sizeof(std::declval<Vertex>().texCoords) == sizeof(std::declval<sf::Vertex>().texCoords));

#undef ENSURE

        // The cast will work so long as the Vertex types are binary-compatible (see ENSUREs above).
        return reinterpret_cast<const sf::Vertex*>(_vertices);
    }

    std::size_t getVertexCount() const {
        return _vertexCount;
    }

private:
    const Vertex* _vertices;
    std::size_t _vertexCount;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !#define UHOBGOBLIN_GRAPHICS_SFML_VERTICES_HPP
