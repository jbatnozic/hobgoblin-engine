// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_COMMON_NAME_OF_TYPE_HPP
#define UHOBGOBLIN_COMMON_NAME_OF_TYPE_HPP

#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

namespace detail {
//! \note Implementation based on https://github.com/nessan/utilities (utilities/type.h).
template <typename T>
constexpr auto NameOfTypeImpl() {
    // clang-format off
#ifdef __clang__
    std::string_view name   = __PRETTY_FUNCTION__;
    std::string_view prefix = "auto jbatnozic::hobgoblin::detail::NameOfTypeImpl() [T = ";
    std::string_view suffix = "]";
#elif defined(__GNUC__)
    std::string_view name   = __PRETTY_FUNCTION__;
    std::string_view prefix = "constexpr auto jbatnozic::hobgoblin::detail::NameOfTypeImpl() [with T = ";
    std::string_view suffix = "]";
#elif defined(_MSC_VER)
    std::string_view name   = __FUNCSIG__;
    std::string_view prefix = "auto __cdecl jbatnozic::hobgoblin::detail::NameOfTypeImpl<";
    std::string_view suffix = ">(void)";
#else
#error "You're using an unsupported compiler."
#endif
    // clang-format on

    name.remove_prefix(prefix.size());
    name.remove_suffix(suffix.size());

    return name;
}
} // namespace detail

//! Returns a string that represents the type of an object as seen by the compiler.
template <typename T>
constexpr std::string_view GetNameOfType() {
    return detail::NameOfTypeImpl<T>();
}

//! Returns a string that represents the type of an object as seen by the compiler.
template <typename T>
constexpr std::string_view GetNameOfType(T&&) {
    return detail::NameOfTypeImpl<T>();
}

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_COMMON_NAME_OF_TYPE_HPP
