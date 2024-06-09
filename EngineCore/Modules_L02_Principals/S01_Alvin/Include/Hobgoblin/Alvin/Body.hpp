// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_BODY_HPP
#define UHOBGOBLIN_ALVIN_BODY_HPP

#include <Hobgoblin/Alvin/Private/Memory_management.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

//! TODO(description)
class Body {
public:
    Body(NeverNull<cpBody*> aBody)
        : _body{aBody} {}

    ~Body() {
        if (_body) {
            auto* space = cpBodyGetSpace(_body.get());
            if (space) {
                cpSpaceRemoveBody(space, _body.get());
            }
        }
    }

    static Body createDynamic(cpFloat aMass, cpFloat aMoment) {
        return {cpBodyNew(aMass, aMoment)};
    }

    static Body createKinematic() {
        return {cpBodyNewKinematic()};
    }

    static Body createStatic() {
        return {cpBodyNewStatic()};
    }

    //! Prevent copying.
    Body(const Body&)            = delete;
    Body& operator=(const Body&) = delete;

    //! Allow cheap moving.
    Body(Body&&)            = default;
    Body& operator=(Body&&) = default;

    //! Automatic conversion to `cpBody*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator cpBody*() {
        return _body.get();
    }

    //! Automatic conversion to `const cpBody*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator const cpBody*() const {
        return _body.get();
    }

    // TODO: convenience methods

private:
    detail::ChipmunkBodyUPtr _body;
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_BODY_HPP
