// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_SHAPE_HPP
#define UHOBGOBLIN_ALVIN_SHAPE_HPP

#include <Hobgoblin/Alvin/Private/Memory_management.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

//! TODO(description)
class Shape {
public:
    Shape(NeverNull<cpShape*> aShape)
        : _shape{aShape} {}

    static Shape createBox(NeverNull<cpBody*> aBody,
                           cpFloat            aWidth,
                           cpFloat            aHeight,
                           cpFloat            aCornerRadius) {
        return {cpBoxShapeNew(aBody, aWidth, aHeight, aCornerRadius)};
    }

    static Shape createBox(NeverNull<cpBody*> aBody, const cpBB& aBoundingBox, cpFloat aCornerRadius) {
        return {cpBoxShapeNew2(aBody, aBoundingBox, aCornerRadius)};
    }

    static Shape createCircle(NeverNull<cpBody*> aBody, cpFloat aRadius, cpVect aOffset) {
        return {cpCircleShapeNew(aBody, aRadius, aOffset)};
    }

    //! Prevent copying.
    Shape(const Shape&)            = delete;
    Shape& operator=(const Shape&) = delete;

    //! Allow cheap moving.
    Shape(Shape&&)            = default;
    Shape& operator=(Shape&&) = default;

    //! Automatic conversion to `cpShape*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator cpShape*() {
        return _shape.get();
    }

    //! Automatic conversion to `const cpShape*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator const cpShape*() const {
        return _shape.get();
    }

    // TODO: convenience methods
    // add/remove space
    // get/set position
    // get/set rotation

private:
    detail::ChipmunkShapeUPtr _shape;
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_SHAPE_HPP
