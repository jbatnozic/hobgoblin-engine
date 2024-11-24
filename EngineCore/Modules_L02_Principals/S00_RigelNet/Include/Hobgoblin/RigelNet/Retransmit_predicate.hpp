// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_RN_RETRANSMIT_PREDICATE_HPP
#define UHOBGOBLIN_RN_RETRANSMIT_PREDICATE_HPP

#include <Hobgoblin/Common.hpp>

#include <chrono>
#include <functional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

//! Function that tells a RigelNet connector whether to retransmit a not yet acknowledged packet or
//! wait some more (true = retransmit, false = wait and check again next cycle).
//!
//! \param aCyclesSinceLasySend number of update cycles (assuming the node is updated once per cycle)
//!                             since the packet was last sent to the recepient.
//! \param aTimeSinceLastSend time in microseconds since the packet was last sent to the recepient.
//! \param aCurrentLatency current estimated (round-trip) latency to the recepient.
using RN_RetransmitPredicate = std::function<bool(PZInteger                 aCyclesSinceLasySend,
                                                  std::chrono::microseconds aTimeSinceLastSend,
                                                  std::chrono::microseconds aCurrentLatency)>;

//! Retransmit predicate used by default by RigelNet.
bool RN_DefaultRetransmitPredicate(PZInteger                 aCyclesSinceLastTransmit,
                                   std::chrono::microseconds aTimeSinceLastSend,
                                   std::chrono::microseconds aCurrentLatency);

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_RETRANSMIT_PREDICATE_HPP
