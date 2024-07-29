// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#include <Hobgoblin/Graphics/Vertex_array.hpp>

#include <SFML/Graphics/VertexArray.hpp>

#include <new>

#include "Draw_bridge.hpp"
#include "SFML_conversions.hpp"
#include "SFML_vertices.hpp"

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
    const auto sfVertexArray = ConvertToSf(SELF); // TODO(optimization: copy/paste sfml algo for this)
    return ToHg(sfVertexArray.getBounds());
}

Drawable::BatchingType VertexArray::getBatchingType() const {
    return BatchingType::VertexArray;
}

void VertexArray::_drawOnto(Canvas& aCanvas, const RenderStates& aStates) const {
    SFMLVertices sfVertices{vertices.data(), vertices.size()};
    const auto sfPrimitiveType = ToSf(primitiveType);

    const auto drawingWasSuccessful = 
        Draw(aCanvas, [&sfVertices, sfPrimitiveType, &aStates](sf::RenderTarget& aSfRenderTarget) {
            aSfRenderTarget.draw(
                sfVertices.getVertices(),
                sfVertices.getVertexCount(),
                sfPrimitiveType,
                ToSf(aStates)
            );
        });
    assert(drawingWasSuccessful);
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
