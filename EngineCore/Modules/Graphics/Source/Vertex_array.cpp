#include <Hobgoblin/Graphics/Vertex_array.hpp>

#include <SFML/Graphics/VertexArray.hpp>

#include <new>

#include "Draw_bridge.hpp"
#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

#if 0
using ImplType = sf::VertexArray;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

VertexArray::VertexArray() {
    static_assert(STORAGE_SIZE  == IMPL_SIZE,  "VertexArray::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "VertexArray::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
}

VertexArray::VertexArray(PrimitiveType aType, PZInteger aVertexCount) {
    new (&_storage) ImplType(ToSf(aType), pztos(aVertexCount));
}

VertexArray::~VertexArray() {
    SELF_IMPL->~ImplType();
}

PZInteger VertexArray::getVertexCount() const {
    return stopz(SELF_CIMPL->getVertexCount());
}

//Vertex& VertexArray::operator[](PZInteger aIndex) {
//    // TODO
//}
//
//const Vertex& VertexArray::operator[](PZInteger aIndex) const {
//    // TODO
//}

void VertexArray::clear() {
    SELF_IMPL->clear();
}

void VertexArray::resize(PZInteger aVertexCount) {
    SELF_IMPL->resize(pztos(aVertexCount));
}

void VertexArray::append(const Vertex& aVertex) {
    // TODO
}

void VertexArray::setPrimitiveType(PrimitiveType aType) {
    SELF_IMPL->setPrimitiveType(ToSf(aType));
}

PrimitiveType VertexArray::getPrimitiveType() const {
    return ToHg(SELF_CIMPL->getPrimitiveType());
}

math::Rectangle<float> VertexArray::getBounds() const {
    const auto bounds = SELF_CIMPL->getBounds();
    return {
        bounds.left,
        bounds.top,
        bounds.width,
        bounds.height
    };
}

void VertexArray::_draw(Canvas& aCanvas, const RenderStates& aStates) const {
    // TODO
}

void* VertexArray::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* VertexArray::_getSFMLImpl() const {
    return SELF_CIMPL;
}
#endif

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
