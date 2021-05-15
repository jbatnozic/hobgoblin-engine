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

template <class ...NoArgs>
typename std::enable_if_t<sizeof...(NoArgs) == 0, void>  Autopack(PacketBase& packet) {
    // Do nothing
}

template <class ArgsHead, class ...ArgsRest>
void Autopack(PacketBase& packet, ArgsHead&& argsHead, ArgsRest&&... argsRest) {
    packet << std::forward<ArgsHead>(argsHead);
    Autopack(packet, std::forward<ArgsRest>(argsRest)...);
}


template <class ...NoArgs>
typename std::enable_if_t<sizeof...(NoArgs) == 0, void>  Autounpack(PacketBase& packet) {
    // Do nothing
}

template <class ArgsHead, class ...ArgsRest>
void Autounpack(PacketBase& packet, ArgsHead&& argsHead, ArgsRest&&... argsRest) {
    packet >> std::forward<ArgsHead>(argsHead);
    Autounpack(packet, std::forward<ArgsRest>(argsRest)...);
}

} // namespace detail

// ========================================================================= //
#define HG_ENABLE_AUTOPACK(_class_name_, ...) \
    void UHOBGOBLIN_autopack(::jbatnozic::hobgoblin::util::PacketBase& packet) const {\
        ::jbatnozic::hobgoblin::util::detail::Autopack(packet, __VA_ARGS__); \
    } \
    void UHOBGOBLIN_autounpack(::jbatnozic::hobgoblin::util::PacketBase& packet) { \
        ::jbatnozic::hobgoblin::util::detail::Autounpack(packet, __VA_ARGS__); \
    } \
    friend ::jbatnozic::hobgoblin::util::PacketBase& operator<<(::jbatnozic::hobgoblin::util::PacketBase& packet, \
                                                                const _class_name_& self) { \
        self.UHOBGOBLIN_autopack(packet); return packet; \
    } \
    friend ::jbatnozic::hobgoblin::util::PacketBase& operator>>(::jbatnozic::hobgoblin::util::PacketBase& packet, \
                                                                _class_name_& self) { \
        self.UHOBGOBLIN_autounpack(packet); return packet; \
    }
// ========================================================================= //

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_AUTOPACK_HPP