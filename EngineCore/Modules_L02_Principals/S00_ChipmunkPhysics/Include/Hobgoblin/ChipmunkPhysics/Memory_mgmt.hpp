// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_CHIPMUNKPHYSICS_MEMORY_MGMT_HPP
#define UHOBGOBLIN_CHIPMUNKPHYSICS_MEMORY_MGMT_HPP

#include <Hobgoblin/Private/chipmunk/chipmunk.h>

#include <cassert>
#include <memory>
#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace cp {

namespace cp_detail {

inline
void cpCleanUpAndDelete(cpShape* shape) { // TODO To .cpp file 
    cpSpace* space = cpShapeGetSpace(shape);
    if (space) {
        cpSpaceRemoveShape(space, shape);
    }
    cpShapeFree(shape);
}

inline
void cpCleanUpAndDelete(cpBody* body) { // TODO To .cpp file 
    cpSpace* space = cpBodyGetSpace(body);
    if (space) {
        cpSpaceRemoveBody(space, body);
    }
    cpBodyFree(body);
}

inline
void cpCleanUpAndDelete(cpConstraint* constraint) { // TODO To .cpp file 
    cpSpace* space = cpConstraintGetSpace(constraint);
    if (space) {
        cpSpaceRemoveConstraint(space, constraint);
    }
    cpConstraintFree(constraint);
}

inline
void cpCleanUpAndDelete(cpSpace* space) { // TODO To .cpp file 
    cpSpaceFree(space);
}

template <class T>
struct cpGenericDeleter {
    static_assert(std::is_same_v<T, cpBody> ||
                  std::is_same_v<T, cpShape> ||
                  std::is_same_v<T, cpConstraint> ||
                  std::is_same_v<T, cpSpace>,
                  "Incompatible type provided");

    void operator()(T* object) {
        cpCleanUpAndDelete(object);
    }
};

using cpBodyDeleter = cpGenericDeleter<cpBody>;
using cpShapeDeleter = cpGenericDeleter<cpShape>;
using cpConstraintDeleter = cpGenericDeleter<cpConstraint>;
using cpSpaceDeleter = cpGenericDeleter<cpSpace>;

} // namespace cp_detail

using cpBodyUPtr = std::unique_ptr<cpBody, cp_detail::cpBodyDeleter>;
using cpShapeUPtr = std::unique_ptr<cpShape, cp_detail::cpShapeDeleter>;
using cpConstraintUPtr = std::unique_ptr<cpConstraint, cp_detail::cpConstraintDeleter>;
using cpSpaceUPtr = std::unique_ptr<cpSpace, cp_detail::cpSpaceDeleter>;

} // namespace cp
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_CHIPMUNKPHYSICS_MEMORY_MGMT_HPP

// clang-format on
