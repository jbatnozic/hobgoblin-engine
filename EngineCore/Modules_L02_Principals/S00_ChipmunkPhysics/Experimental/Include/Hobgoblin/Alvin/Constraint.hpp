// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_CONSTRAINT_HPP
#define UHOBGOBLIN_ALVIN_CONSTRAINT_HPP

#include <Hobgoblin/Alvin/Private/Memory_management.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

//! TODO(description)
class Constraint {
public:
    Constraint(NeverNull<cpConstraint*> aConstraint)
        : _constraint{aConstraint} {}

    // TODO: static create methods

    //! Prevent copying.
    Constraint(const Constraint&) = delete;
    Constraint& operator=(const Constraint&) = delete;

    //! Allow cheap moving.
    Constraint(Constraint&&) = default;
    Constraint& operator=(Constraint&&) = default;

    //! Automatic conversion to `cpConstraint*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator cpConstraint*() {
        return _constraint.get();
    }

    //! Automatic conversion to `const cpConstraint*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator const cpConstraint*() const {
        return _constraint.get();
    }

    // TODO: convenience methods

private:
    detail::ChipmunkConstraintUPtr _constraint;
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_CONSTRAINT_HPP
