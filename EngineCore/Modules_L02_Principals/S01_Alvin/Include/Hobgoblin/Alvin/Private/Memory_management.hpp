// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_PRIVATE_MEMORY_MANAGEMENT_HPP
#define UHOBGOBLIN_ALVIN_PRIVATE_MEMORY_MANAGEMENT_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>

#include <cassert>
#include <memory>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {
namespace detail {

inline void CleanUpAndDelete(cpShape* aShape) {
    assert(aShape != nullptr && cpShapeGetSpace(aShape) != nullptr);
    cpShapeFree(aShape);
}

inline void CleanUpAndDelete(cpBody* aBody) {
    assert(aBody != nullptr && cpBodyGetSpace(aBody) != nullptr);
    cpBodyFree(aBody);
}

inline void CleanUpAndDelete(cpConstraint* aConstraint) {
    assert(aConstraint != nullptr && cpConstraintGetSpace(aConstraint) != nullptr);
    cpConstraintFree(aConstraint);
}

inline void CleanUpAndDelete(cpSpace* aSpace) {
    cpSpaceFree(aSpace);
}

// clang-format off
template <class T>
struct GenericChipmunkDeleter {
    static_assert(std::is_same_v<T, cpBody> ||
                  std::is_same_v<T, cpShape> ||
                  std::is_same_v<T, cpConstraint> ||
                  std::is_same_v<T, cpSpace>,
                  "Incompatible type provided");

    void operator()(T* aObject) {
        CleanUpAndDelete(aObject);
    }
};

using ChipmunkBodyUPtr       = std::unique_ptr<cpBody,       GenericChipmunkDeleter<cpBody>>;
using ChipmunkShapeUPtr      = std::unique_ptr<cpShape,      GenericChipmunkDeleter<cpShape>>;
using ChipmunkConstraintUPtr = std::unique_ptr<cpConstraint, GenericChipmunkDeleter<cpConstraint>>;
using ChipmunkSpaceUPtr      = std::unique_ptr<cpSpace,      GenericChipmunkDeleter<cpSpace>>;
// clang-format on

} // namespace detail
} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_PRIVATE_MEMORY_MANAGEMENT_HPP
