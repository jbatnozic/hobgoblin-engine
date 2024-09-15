// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/RigelNet/Retransmit_predicate.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

bool RN_DefaultRetransmitPredicate(PZInteger /*aCyclesSinceLastTransmit*/,
                                   std::chrono::microseconds aTimeSinceLastSend,
                                   std::chrono::microseconds aCurrentLatency) {
    return (aTimeSinceLastSend >= (2 * aCurrentLatency));
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
