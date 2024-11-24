// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_RN_UDP_CONNECTOR_PACKET_KINDS_HPP
#define UHOBGOBLIN_RN_UDP_CONNECTOR_PACKET_KINDS_HPP

#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

// clang-format off
constexpr std::uint32_t UDP_PACKET_KIND_HELLO      = 0x3BF0E110; //!< Client notifies server of its existence and of the wish to connect.
constexpr std::uint32_t UDP_PACKET_KIND_CONNECT    = 0x83C96CA4; //!< Server notifies client that the connection is accepted.
constexpr std::uint32_t UDP_PACKET_KIND_DISCONNECT = 0xD0F235AB; //!< Node notifies peer of the disconnect.
constexpr std::uint32_t UDP_PACKET_KIND_DATA       = 0xA765B8F6; //!< Regular data packet.
constexpr std::uint32_t UDP_PACKET_KIND_DATA_MORE  = 0x782A2A78; //!< Part of a fragmented data packet.
constexpr std::uint32_t UDP_PACKET_KIND_DATA_TAIL  = 0x00DA7A11; //!< Final part of a fragmented data packet.
constexpr std::uint32_t UDP_PACKET_KIND_ACKS       = 0x71AC2519; //!< Collection of acknowledges.
// clang-format on

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_CONNECTOR_PACKET_KINDS_HPP
