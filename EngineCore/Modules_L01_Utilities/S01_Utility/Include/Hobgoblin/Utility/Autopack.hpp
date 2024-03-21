#ifndef UHOBGOBLIN_UTIL_AUTOPACK_HPP
#define UHOBGOBLIN_UTIL_AUTOPACK_HPP

#include <Hobgoblin/Utility/Packet.hpp>

#include <iostream>
#include <type_traits>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

namespace detail {

template <class ...taArgs>
void Autopack(Packet& aPacket, taArgs&&... aArgs) {
    (aPacket << ... << std::forward<taArgs>(aArgs));
}

template <class ...taNoArgs,
          T_ENABLE_IF(sizeof...(taNoArgs) == 0)>
void Autounpack(Packet& aPacket) {}

template <class taArgsHead, class ...taArgsRest>
void Autounpack(Packet& aPacket, taArgsHead&& aArgsHead, taArgsRest&&... aArgsRest) {
    if (aPacket.noThrow() >> std::forward<taArgsHead>(aArgsHead)) {
        Autounpack(aPacket, std::forward<taArgsRest>(aArgsRest)...);
    }
}

} // namespace detail

// ========================================================================= //
#define HG_ENABLE_AUTOPACK(_class_name_, ...) \
    void UHOBGOBLIN_autopack(::jbatnozic::hobgoblin::util::Packet& aPacket) const { \
        ::jbatnozic::hobgoblin::util::detail::Autopack(aPacket, __VA_ARGS__); \
    } \
    void UHOBGOBLIN_autounpack(::jbatnozic::hobgoblin::util::Packet& aPacket) { \
        ::jbatnozic::hobgoblin::util::detail::Autounpack(aPacket, __VA_ARGS__); \
    } \
    friend ::jbatnozic::hobgoblin::util::Packet& operator<<(::jbatnozic::hobgoblin::util::PacketExtender& aPacket, \
                                                            const _class_name_& aSelf) { \
        aSelf.UHOBGOBLIN_autopack(*aPacket); return *aPacket; \
    } \
    friend ::jbatnozic::hobgoblin::util::Packet& operator>>(::jbatnozic::hobgoblin::util::PacketExtender& aPacket, \
                                                            _class_name_& aSelf) { \
        aSelf.UHOBGOBLIN_autounpack(*aPacket); return *aPacket; \
    }
// ========================================================================= //

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_AUTOPACK_HPP