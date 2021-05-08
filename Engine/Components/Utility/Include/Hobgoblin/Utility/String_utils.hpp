#ifndef UHOBGOBLIN_UTIL_STRING_UTILS_HPP
#define UHOBGOBLIN_UTIL_STRING_UTILS_HPP

#include <Hobgoblin/Common.hpp>

#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! Constexpr string hash function.
//! Copied from: https://xueyouchao.github.io/2016/11/16/CompileTimeString/ 
template <std::size_t N>
constexpr inline std::size_t HornerHash(const char (&str)[N],
                                        std::size_t prime = 31,
                                        std::size_t length = N - 1) {
    return (length <= 1) ? str[0] : (prime * HornerHash(str, prime, length - 1) + str[length - 1]);
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_STRING_UTILS_HPP