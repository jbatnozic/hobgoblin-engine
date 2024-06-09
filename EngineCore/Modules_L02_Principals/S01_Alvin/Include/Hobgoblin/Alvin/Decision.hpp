// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_DECISION_HPP
#define UHOBGOBLIN_ALVIN_DECISION_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

enum class Decision : signed char {
    //! Accept the collision so that it can be processed further.
    //! Both collising shapes must accept the collision, otherwise it will be rejected.
    ACCEPT_COLLISION = true,

    //! Reject the collision so it isn't processed further.
    REJECT_COLLISION = false
};

namespace detail {
constexpr cpBool ToBool(Decision aDecision) {
    return static_cast<bool>(aDecision);
}
} // namespace detail

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_DECISION_HPP
