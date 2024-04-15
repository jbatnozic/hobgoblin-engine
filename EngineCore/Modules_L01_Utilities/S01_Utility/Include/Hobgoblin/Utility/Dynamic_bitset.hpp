// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_UTIL_DYNAMIC_BITSET_HPP
#define UHOBGOBLIN_UTIL_DYNAMIC_BITSET_HPP

#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class DynamicBitset {
public:
    ~DynamicBitset();

    bool getBit(PZInteger aIndex) const;

    void setBit(PZInteger aIndex);

    void clearBit(PZInteger aIndex);

private:
    mutable char* _buf = 0;
    mutable PZInteger _bufferSize = 0;

    void _ensureSize(PZInteger aSize) const;
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_DYNAMIC_BITSET_HPP

// clang-format on
