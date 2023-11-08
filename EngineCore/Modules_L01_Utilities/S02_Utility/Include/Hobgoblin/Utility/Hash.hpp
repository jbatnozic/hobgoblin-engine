#ifndef UHOBGOBLIN_UTIL_HASH_HPP
#define UHOBGOBLIN_UTIL_HASH_HPP

#include <cstddef>
#include <functional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

inline
std::size_t CombineHashes(std::size_t aHash1, std::size_t aHash2) {
    return aHash2 + 0x9e3779b9 + (aHash1 << 6) + (aHash1 >> 2);
}

template <class taHashable>
std::size_t CalcHashAndCombine(const taHashable& aObjectToHash, 
                               std::size_t aPreExistingHashValue) {
    std::hash<taHashable> hasher;
    return CombineHashes(hasher(aObjectToHash), aPreExistingHashValue);
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_HASH_HPP