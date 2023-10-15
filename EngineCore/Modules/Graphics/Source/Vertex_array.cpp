#include <Hobgoblin/Graphics/Vertex_array.hpp>

#include <SFML/Graphics/VertexArray.hpp>

#include <new>

#include "Draw_bridge.hpp"
#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace {
sf::VertexArray ConvertToSf(const VertexArray& aVertexArray) {
    sf::VertexArray sfVertexArray{ToSf(aVertexArray.primitiveType), aVertexArray.vertices.size()};

    for (std::size_t i = 0; i < aVertexArray.vertices.size(); i += 1) {
        const auto& hgVertex = aVertexArray.vertices[i];
        auto& sfVertex = sfVertexArray[i];

        sfVertex.position = ToSf(hgVertex.position);
        sfVertex.texCoords = ToSf(hgVertex.texCoords);
        sfVertex.color = ToSf(hgVertex.color);
    }

    return sfVertexArray;
}
} // namespace

math::Rectangle<float> VertexArray::calculateBounds() const {
    const auto sfVertexArray = ConvertToSf(SELF);
    return ToHg(sfVertexArray.getBounds());
}

Drawable::BatchingType VertexArray::getBatchingType() const {
    return BatchingType::VertexArray;
}

void VertexArray::_draw(Canvas& aCanvas, const RenderStates& aStates) const {
    const auto sfVertexArray = ConvertToSf(SELF);

    const auto drawingWasSuccessful = 
        Draw(aCanvas, [&sfVertexArray, &aStates](sf::RenderTarget& aSfRenderTarget) {
            aSfRenderTarget.draw(sfVertexArray, ToSf(aStates));
        });
    assert(drawingWasSuccessful);
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
