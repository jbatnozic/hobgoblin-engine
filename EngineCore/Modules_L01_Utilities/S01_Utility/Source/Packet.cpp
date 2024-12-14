// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Utility/Packet.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <cstring>
#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

void* Packet::getMutableData() {
    return _buffer.empty() ? nullptr : _buffer.data();
}

Packet& Packet::appendBytes(NeverNull<const void*> aData, PZInteger aByteCount) {
    if (aData && aByteCount > 0) {
        _buffer.insert(_buffer.end(),
                       static_cast<const std::uint8_t*>(aData.get()),
                       static_cast<const std::uint8_t*>(aData.get()) + aByteCount);
    }
    return SELF;
}

std::int64_t Packet::seek(std::int64_t aPosition) {
    if (_buffer.size() == 0) {
        return 0;
    }

    const std::size_t min = 0;
    const std::size_t max = _buffer.size() - 1;

    _readPos = math::Clamp(static_cast<std::size_t>(aPosition), min, max);
    return static_cast<std::int64_t>(_readPos);
}

std::int64_t Packet::seekRelative(std::int64_t aOffset) {
    if (_buffer.size() == 0) {
        return 0;
    }

    const std::size_t min = 0;
    const std::size_t max = _buffer.size() - 1;

    const std::size_t newPos =
        static_cast<std::size_t>(static_cast<std::int64_t>(_readPos) + aOffset);

    _readPos = math::Clamp(newPos, min, max);
    return static_cast<std::int64_t>(_readPos);
}

int Packet::read(NeverNull<void*> aDestination, PZInteger aByteCount) {
    if (!_isValid) {
        return -1;
    }

    if (aByteCount == 0) {
        return 0;
    }

    const auto rem = _buffer.size() - _readPos;
    if (rem >= pztos(aByteCount)) {
        std::memcpy(aDestination, &(_buffer[_readPos]), pztos(aByteCount));
        _readPos += pztos(aByteCount);
        return aByteCount;
    } else if (rem > 0) {
        std::memcpy(aDestination, &(_buffer[_readPos]), rem);
        _readPos += rem;
        return (int)rem;
    } else {
        return 0;
    }
}

void* Packet::extractBytes(PZInteger aByteCount) {
    void* result = extractBytesNoThrow(aByteCount);
    if (!SELF) {
        HG_THROW_TRACED(
            StreamExtractError,
            0,
            "Failed to extract {} raw bytes from hg::util::Packet (actual # of bytes remaining: {}).",
            aByteCount,
            getRemainingDataSize()
        );
    }
    return result;
}

void* Packet::extractBytesNoThrow(PZInteger aByteCount) {
    if (aByteCount == 0) {
        return nullptr;
    }

    const auto rem = _buffer.size() - _readPos;
    if (pztos(aByteCount) > rem) {
        _isValid = false;
        return nullptr;
    }

    void* result = &(_buffer[_readPos]);
    _readPos += pztos(aByteCount);
    return result;
}

OutputStream& operator<<(OutputStreamExtender& aPacketExt, const Packet& aData) {
    aPacketExt << static_cast<std::int32_t>(aData.getDataSize());
    if (const auto size = aData.getDataSize(); size> 0) {
        aPacketExt->appendBytes(aData.getData(), size);
    }
    return *aPacketExt;
}

InputStream& operator>>(InputStreamExtender& aPacketExt, Packet& aData) {
    // First extract data length
    const auto length = aPacketExt->extractNoThrow<std::int32_t>();
    if (!*aPacketExt) {
        return *aPacketExt;
    }

    aData.clear();
    if (length > 0) {
        const auto  pzlen = static_cast<PZInteger>(length);
        const auto* bytes = aPacketExt->extractBytesNoThrow(pzlen);
        if (bytes) {
            aData.appendBytes(bytes, pzlen);
        }
    }

    return *aPacketExt;
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
