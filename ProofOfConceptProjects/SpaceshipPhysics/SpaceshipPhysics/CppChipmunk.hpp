#ifndef CPP_CHIPMUNK_HPP
#define CPP_CHIPMUNK_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>

#include <cassert>
#include <memory>
#include <type_traits>

template <class T>
class ChipmunkDeleter {
public:

    static_assert(std::is_same_v<T, cpBody> ||
                  std::is_same_v<T, cpShape> ||
                  std::is_same_v<T, cpConstraint>,
                  "Incompatible type used to ChipmunkDeleter");

    void operator()(T* object) {
        if constexpr (std::is_same_v<T, cpBody>) {
            cpSpace* space = cpBodyGetSpace(object);
            if (space) {
                cpSpaceRemoveBody(space, object);
            }
            cpBodyFree(object);
        }
        if constexpr (std::is_same_v<T, cpShape>) {
            cpSpace* space = cpShapeGetSpace(object);
            if (space) {
                cpSpaceRemoveShape(space, object);
            }
            cpShapeFree(object);
        }
        if constexpr (std::is_same_v<T, cpConstraint>) {
            cpSpace* space = cpConstraintGetSpace(object);
            if (space) {
                cpSpaceRemoveConstraint(space, object);
            }
            cpConstraintFree(object);
        }
    }

};

using BodyDeleter = ChipmunkDeleter<cpBody>;
using ShapeDeleter = ChipmunkDeleter<cpShape>;
using ConstraintDeleter = ChipmunkDeleter<cpConstraint>;

using BodyUniquePtr = std::unique_ptr<cpBody, BodyDeleter>;
using ShapeUniquePtr = std::unique_ptr<cpShape, ShapeDeleter>;
using ConstraintUniquePtr = std::unique_ptr<cpConstraint, ConstraintDeleter>;

/*BodyUniquePtr MakeBodyUniquePtr(cpSpace* space, cpBody* body = nullptr) {
    return BodyUniquePtr(body, BodyDeleter{space});
}

ShapeUniquePtr MakeShapeUniquePtr(cpSpace* space, cpShape* shape = nullptr) {
    return ShapeUniquePtr(shape, ShapeDeleter{space});
}

ConstraintUniquePtr MakeConstraintUniquePtr(cpSpace* space, cpConstraint* constraint = nullptr) {
    return ConstraintUniquePtr(constraint, ConstraintDeleter{space});
}*/

#endif // !CPP_CHIPMUNK_HPP
