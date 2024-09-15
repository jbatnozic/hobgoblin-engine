// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_RN_CONFIGURATION_HPP
#define UHOBGOBLIN_RN_CONFIGURATION_HPP

#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

enum class RN_Protocol {
    None, //!< No protocol implemented (only on dummy nodes).
    TCP,  //!< Transmission control protocol (NOT YET IMPLEMENTED).
    UDP   //!< User datagram protocol.
};

enum class RN_NetworkingStack {
    Default, //!< Use socket implementation and networking stack of the host OS.
    ZeroTier //!< TODO Add description...
};

struct RN_ComposeForAllType {};
constexpr RN_ComposeForAllType RN_COMPOSE_FOR_ALL{};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_CONFIGURATION_HPP
