// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

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
void Autopack(OutputStream& aOStream, taArgs&&... aArgs) {
    (aOStream << ... << std::forward<taArgs>(aArgs));
}

template <class ...taNoArgs,
          T_ENABLE_IF(sizeof...(taNoArgs) == 0)>
void Autounpack(InputStream& aIStream) {}

template <class taArgsHead, class ...taArgsRest>
void Autounpack(InputStream& aIStream, taArgsHead&& aArgsHead, taArgsRest&&... aArgsRest) {
    if (aIStream.noThrow() >> std::forward<taArgsHead>(aArgsHead)) {
        Autounpack(aIStream, std::forward<taArgsRest>(aArgsRest)...);
    }
}

} // namespace detail

// ========================================================================= //
#define HG_ENABLE_AUTOPACK(_class_name_, ...) \
    void UHOBGOBLIN_autopack(::jbatnozic::hobgoblin::util::OutputStream& aOStream) const { \
        ::jbatnozic::hobgoblin::util::detail::Autopack(aOStream, __VA_ARGS__); \
    } \
    void UHOBGOBLIN_autounpack(::jbatnozic::hobgoblin::util::InputStream& aIStream) { \
        ::jbatnozic::hobgoblin::util::detail::Autounpack(aIStream, __VA_ARGS__); \
    } \
    friend ::jbatnozic::hobgoblin::util::OutputStream& operator<<(::jbatnozic::hobgoblin::util::OutputStreamExtender& aOStream, \
                                                            const _class_name_& aSelf) { \
        aSelf.UHOBGOBLIN_autopack(*aOStream); return *aOStream; \
    } \
    friend ::jbatnozic::hobgoblin::util::InputStream& operator>>(::jbatnozic::hobgoblin::util::InputStreamExtender& aIStream, \
                                                            _class_name_& aSelf) { \
        aSelf.UHOBGOBLIN_autounpack(*aIStream); return *aIStream; \
    }
// ========================================================================= //

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_AUTOPACK_HPP

// clang-format on
