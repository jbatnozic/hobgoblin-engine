
#include <Hobgoblin/Utility/Dynamic_bitset.hpp>

#include <cstdlib>
#include <new>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

DynamicBitset::~DynamicBitset() {
    if (_bufferSize != 0) {
        std::free(_buf);
    }
}

bool DynamicBitset::getBit(PZInteger aIndex) const {
    _ensureSize(aIndex + 1);

    const auto byteIndex = aIndex / CHAR_BIT;
    const char* byte;
    if (_bufferSize == 0) {
        byte = reinterpret_cast<const char*>(&_buf) + byteIndex;
    }
    else {
        byte = _buf + byteIndex;
    }

    const auto bitmask = 1 << (aIndex % CHAR_BIT);
    return ((*byte & bitmask) != 0);
}

void DynamicBitset::setBit(PZInteger aIndex) {
    _ensureSize(aIndex + 1);

    const auto byteIndex = aIndex / CHAR_BIT;
    char* byte;
    if (_bufferSize == 0) {
        byte = reinterpret_cast<char*>(&_buf) + byteIndex;
    }
    else {
        byte = _buf + byteIndex;
    }

    const auto bitmask = 1 << (aIndex % CHAR_BIT);
    *byte |= bitmask;
}

void DynamicBitset::clearBit(PZInteger aIndex) {
    _ensureSize(aIndex + 1);

    const auto byteIndex = aIndex / CHAR_BIT;
    char* byte;
    if (_bufferSize == 0) {
        byte = reinterpret_cast<char*>(&_buf) + byteIndex;
    }
    else {
        byte = _buf + byteIndex;
    }

    const auto bitmask = 1 << (aIndex % CHAR_BIT);
    *byte &= ~bitmask;
}

void DynamicBitset::_ensureSize(PZInteger aSize) const {
    if (aSize <= sizeof(decltype(_buf)) * CHAR_BIT) {
        return;
    }
    if (_bufferSize != 0) {
        // Already uses dynamic allocation

        const auto newBufferSize = ((aSize - 1) / CHAR_BIT) + 1;
        if (newBufferSize < _bufferSize) {
            return;
        }

        char* const newBuffer = static_cast<char*>(std::calloc(newBufferSize, sizeof(char)));
        if (newBuffer == nullptr) {
            throw std::bad_alloc{};
        }

        std::memcpy(newBuffer, _buf, _bufferSize);

        std::free(_buf);

        _buf = newBuffer;
        _bufferSize = newBufferSize;
    }
    else {
        // Switch to dynamic allocation now

        const auto newBufferSize = ((aSize - 1) / CHAR_BIT) + 1;
        char* const newBuffer = static_cast<char*>(std::calloc(newBufferSize, sizeof(char)));
        if (newBuffer == nullptr) {
            throw std::bad_alloc{};
        }

        std::memcpy(newBuffer, &_buf, sizeof(decltype(_buf)));

        _buf = newBuffer;
        _bufferSize = newBufferSize;
    }
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>