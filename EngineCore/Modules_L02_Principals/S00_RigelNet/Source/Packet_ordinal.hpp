// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_RN_PACKET_ORDINAL_HPP
#define UHOBGOBLIN_RN_PACKET_ORDINAL_HPP

#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

// Assuming a connector consistently sends 1000 packets per seconds (which is already
// unreasonable), the connection could last over a 1000 hours (~41 days) before erroring
// out due to uint32 overflow.
using PacketOrdinal = std::uint32_t;

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_PACKET_ORDINAL_HPP
