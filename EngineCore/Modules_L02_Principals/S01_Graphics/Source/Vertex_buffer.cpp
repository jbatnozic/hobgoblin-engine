// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Graphics/Vertex_buffer.hpp>

#include <SFML/Graphics/VertexBuffer.hpp>

#include <new>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType = sf::VertexBuffer;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

VertexBuffer::VertexBuffer() {
    static_assert(STORAGE_SIZE == IMPL_SIZE,   "VertexBuffer::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "VertexBuffer::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
}

VertexBuffer::VertexBuffer(PrimitiveType type) {
    // TODO
}

VertexBuffer::VertexBuffer(Usage usage) {
    // TODO
}

VertexBuffer::VertexBuffer(PrimitiveType type, Usage usage) {
    // TODO
}

VertexBuffer::VertexBuffer(const VertexBuffer& aCopy) {
    new (&_storage) ImplType();
}

VertexBuffer::~VertexBuffer() {
    SELF_IMPL->~ImplType();
}

bool VertexBuffer::create(PZInteger aVertexCount) {
    return SELF_IMPL->create(pztos(aVertexCount));
}

PZInteger VertexBuffer::getVertexCount() const {
    return stopz(SELF_CIMPL->getVertexCount());
}

bool VertexBuffer::update(const Vertex* vertices) {
    // TODO
    return false;
}

bool VertexBuffer::update(const Vertex* vertices, PZInteger vertexCount, PZInteger offset) {
    // TODO
    return false;
}

bool VertexBuffer::update(const VertexBuffer& aVertexBuffer) {
    return SELF_IMPL->update(*CIMPLOF(aVertexBuffer));
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& aRight) {
    *SELF_IMPL = *CIMPLOF(aRight);
    return SELF;
}

void VertexBuffer::swap(VertexBuffer& aRight) {
    SELF_IMPL->swap(*IMPLOF(aRight));
}

unsigned int VertexBuffer::getNativeHandle() const {
    return SELF_CIMPL->getNativeHandle();
}

void VertexBuffer::setPrimitiveType(PrimitiveType aType) {
    SELF_IMPL->setPrimitiveType(ToSf(aType));
}

PrimitiveType VertexBuffer::getPrimitiveType() const {
    return ToHg(SELF_CIMPL->getPrimitiveType());
}

void VertexBuffer::setUsage(Usage aUsage) {
    SELF_IMPL->setUsage(ToSf(aUsage));
}

VertexBuffer::Usage VertexBuffer::getUsage() const {
    return ToHg(SELF_CIMPL->getUsage());
}

void VertexBuffer::bind(const VertexBuffer* aVertexBuffer) {
    if (aVertexBuffer == nullptr) {
        sf::VertexBuffer::bind(nullptr);
        return;
    }
    sf::VertexBuffer::bind(CIMPLOF(*aVertexBuffer));
}

bool VertexBuffer::isAvailable() {
    return sf::VertexBuffer::isAvailable();
}

void VertexBuffer::draw(RenderTarget& target, RenderStates states) const {
    // TODO
}

void* VertexBuffer::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* VertexBuffer::_getSFMLImpl() const {
    return SELF_CIMPL;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
