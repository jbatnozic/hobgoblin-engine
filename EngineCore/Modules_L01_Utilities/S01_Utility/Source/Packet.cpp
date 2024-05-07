// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Unicode/String_conversions.hpp>
#include <Hobgoblin/Logging.hpp>

#ifdef _MSC_VER
#include <winsock.h> // for hton & friends
#else
#include <arpa/inet.h> // for hton & friends
#endif

#include <cstring>
#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

namespace {
constexpr auto LOG_ID = "Hobgoblin.Utility";
} // namespace

Packet::Packet() = default;

Packet::~Packet() = default;

void Packet::clear() {
    _buffer.clear();
    _readPos = 0;
    _isValid = true;
}

PZInteger Packet::getReadPosition() const {
  return stopz(_readPos);
}

const void* Packet::getData() const {
  return _buffer.empty() ? nullptr : _buffer.data();
}

void* Packet::getMutableData() {
    return _buffer.empty() ? nullptr : _buffer.data();
}

PZInteger Packet::getDataSize() const {
  return stopz(_buffer.size());
}

PZInteger Packet::getRemainingDataSize() const {
  return (getDataSize() - getReadPosition());
}

bool Packet::endOfPacket() const {
  return (_readPos >= _buffer.size());
}

///////////////////////////////////////////////////////////////////////////
// APPENDING                                                             //
///////////////////////////////////////////////////////////////////////////

void Packet::appendBytes(const void* aData, PZInteger aByteCount) {
    if (aData && (aByteCount > 0)) {
        const auto start = _buffer.size();
        _buffer.resize(start + pztos(aByteCount));
        std::memcpy(_buffer.data() + start, aData, pztos(aByteCount));
    }
}

Packet& operator<<(PacketExtender& aPacketExt, bool aData) {
    aPacketExt << static_cast<std::int8_t>(aData);
    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, std::int8_t aData) {
    aPacketExt->appendBytes(&aData, sizeof(aData));
    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, std::uint8_t aData) {
    aPacketExt->appendBytes(&aData, sizeof(aData));
    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, std::int16_t aData) {
    std::int16_t toWrite = static_cast<std::int16_t>(htons(static_cast<std::uint16_t>(aData)));
    aPacketExt->appendBytes(&toWrite, sizeof(toWrite));
    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, std::uint16_t aData) {
    std::uint16_t toWrite = htons(aData);
    aPacketExt->appendBytes(&toWrite, sizeof(toWrite));
    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, std::int32_t aData) {
    std::int32_t toWrite = static_cast<std::int32_t>(htonl(static_cast<std::uint32_t>(aData)));
    aPacketExt->appendBytes(&toWrite, sizeof(toWrite));
    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, std::uint32_t aData) {
    std::uint32_t toWrite = htonl(aData);
    aPacketExt->appendBytes(&toWrite, sizeof(toWrite));
    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, std::int64_t aData) {
    // Since htonll is not available everywhere, we have to convert
    // to network byte order (big endian) manually
    std::uint8_t toWrite[] =
    {
        static_cast<std::uint8_t>((aData >> 56) & 0xFF),
        static_cast<std::uint8_t>((aData >> 48) & 0xFF),
        static_cast<std::uint8_t>((aData >> 40) & 0xFF),
        static_cast<std::uint8_t>((aData >> 32) & 0xFF),
        static_cast<std::uint8_t>((aData >> 24) & 0xFF),
        static_cast<std::uint8_t>((aData >> 16) & 0xFF),
        static_cast<std::uint8_t>((aData >>  8) & 0xFF),
        static_cast<std::uint8_t>((aData      ) & 0xFF)
    };
    aPacketExt->appendBytes(&toWrite, sizeof(toWrite));
    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, std::uint64_t aData) {
    // Since htonll is not available everywhere, we have to convert
    // to network byte order (big endian) manually
    std::uint8_t toWrite[] =
    {
        static_cast<std::uint8_t>((aData >> 56) & 0xFF),
        static_cast<std::uint8_t>((aData >> 48) & 0xFF),
        static_cast<std::uint8_t>((aData >> 40) & 0xFF),
        static_cast<std::uint8_t>((aData >> 32) & 0xFF),
        static_cast<std::uint8_t>((aData >> 24) & 0xFF),
        static_cast<std::uint8_t>((aData >> 16) & 0xFF),
        static_cast<std::uint8_t>((aData >>  8) & 0xFF),
        static_cast<std::uint8_t>((aData      ) & 0xFF)
    };
    aPacketExt->appendBytes(&toWrite, sizeof(toWrite));
    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, float aData) {
    std::int32_t temp;
    static_assert(sizeof(temp) == sizeof(aData), "Use integral type that has the same size as float");

    std::memcpy(&temp, &aData, sizeof(temp));
    aPacketExt << temp;

    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, double aData) {
    std::int64_t temp;
    static_assert(sizeof(temp) == sizeof(aData), "Use integral type that has the same size as double");

    std::memcpy(&temp, &aData, sizeof(temp));
    aPacketExt << temp;

    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, std::string_view aData){
    aPacketExt << static_cast<std::uint32_t>(aData.size());
    aPacketExt->appendBytes(aData.data(), stopz(aData.size()));
    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, const UnicodeString& aData) {
    const auto utf8str = UniStrConv(TO_UTF8_STD_STRING, aData);
    aPacketExt << utf8str;
    return *aPacketExt;
}

Packet& operator<<(PacketExtender& aPacketExt, const Packet& aData) {
    aPacketExt << static_cast<std::int32_t>(aData.getDataSize());
    aPacketExt->appendBytes(aData.getData(), aData.getDataSize());
    return *aPacketExt;
}

///////////////////////////////////////////////////////////////////////////
// EXTRACTING                                                            //
///////////////////////////////////////////////////////////////////////////

void* Packet::extractBytes(PZInteger aByteCount) {
    void* result = extractBytesNoThrow(aByteCount);
    if (!SELF) {
        HG_THROW_TRACED(
            PacketExtractError,
            0,
            "Failed to extract {} raw bytes from hg::util::Packet (actual # of bytes remaining: {}).",
            aByteCount,
            getRemainingDataSize()
        );
    }
    return result;
}

void* Packet::extractBytesNoThrow(PZInteger aByteCount) {
    void* result = _buffer.data() + _readPos;

    if (aByteCount > 0 && _checkSize(pztos(aByteCount))) {
      _readPos += pztos(aByteCount);
      return result;
    }

    return nullptr;
}

Packet& operator>>(PacketExtender& aPacketExt, bool& aData) {
    aData = (aPacketExt->extractNoThrow<std::int8_t>() != 0);
    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, std::int8_t& aData) {
    static constexpr auto BYTE_COUNT = sizeof(std::remove_reference_t<decltype(aData)>);

    if (aPacketExt->_checkSize(BYTE_COUNT)) {
        std::memcpy(&aData, &(aPacketExt->_buffer[aPacketExt->_readPos]), BYTE_COUNT);
        aPacketExt->_readPos += BYTE_COUNT;
    }

    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, std::uint8_t& aData) {
    static constexpr auto BYTE_COUNT = sizeof(std::remove_reference_t<decltype(aData)>);

    if (aPacketExt->_checkSize(BYTE_COUNT)) {
        std::memcpy(&aData, &(aPacketExt->_buffer[aPacketExt->_readPos]), BYTE_COUNT);
        aPacketExt->_readPos += BYTE_COUNT;
    }

    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, std::int16_t& aData) {
    static constexpr auto BYTE_COUNT = sizeof(std::remove_reference_t<decltype(aData)>);

    if (aPacketExt->_checkSize(BYTE_COUNT)) {
        std::memcpy(&aData, &(aPacketExt->_buffer[aPacketExt->_readPos]), BYTE_COUNT);
        aData = static_cast<std::int16_t>(ntohs(static_cast<std::uint16_t>(aData)));
        aPacketExt->_readPos += BYTE_COUNT;
    }

    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, std::uint16_t& aData) {
    static constexpr auto BYTE_COUNT = sizeof(std::remove_reference_t<decltype(aData)>);

    if (aPacketExt->_checkSize(BYTE_COUNT)) {
        std::memcpy(&aData, &(aPacketExt->_buffer[aPacketExt->_readPos]), BYTE_COUNT);
        aData = ntohs(aData);
        aPacketExt->_readPos += BYTE_COUNT;
    }

    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, std::int32_t& aData) {
    static constexpr auto BYTE_COUNT = sizeof(std::remove_reference_t<decltype(aData)>);

    if (aPacketExt->_checkSize(BYTE_COUNT)) {
        std::memcpy(&aData, &(aPacketExt->_buffer[aPacketExt->_readPos]), BYTE_COUNT);
        aData = static_cast<std::int32_t>(ntohl(static_cast<std::uint32_t>(aData)));
        aPacketExt->_readPos += BYTE_COUNT;
    }

    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, std::uint32_t& aData) {
    static constexpr auto BYTE_COUNT = sizeof(std::remove_reference_t<decltype(aData)>);

    if (aPacketExt->_checkSize(BYTE_COUNT)) {
        std::memcpy(&aData, &(aPacketExt->_buffer[aPacketExt->_readPos]), BYTE_COUNT);
        aData = ntohl(aData);
        aPacketExt->_readPos += BYTE_COUNT;
    }

    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, std::int64_t& aData) {
    static constexpr auto BYTE_COUNT = sizeof(std::remove_reference_t<decltype(aData)>);

    if (aPacketExt->_checkSize(BYTE_COUNT)) {
        // Since ntohll is not available everywhere, we have to convert
        // to network byte order (big endian) manually
        std::uint8_t bytes[BYTE_COUNT];
        std::memcpy(bytes, &(aPacketExt->_buffer[aPacketExt->_readPos]), BYTE_COUNT);
        aData = (static_cast<std::int64_t>(bytes[0]) << 56) |
                (static_cast<std::int64_t>(bytes[1]) << 48) |
                (static_cast<std::int64_t>(bytes[2]) << 40) |
                (static_cast<std::int64_t>(bytes[3]) << 32) |
                (static_cast<std::int64_t>(bytes[4]) << 24) |
                (static_cast<std::int64_t>(bytes[5]) << 16) |
                (static_cast<std::int64_t>(bytes[6]) <<  8) |
                (static_cast<std::int64_t>(bytes[7])      );
        aPacketExt->_readPos += BYTE_COUNT;
    }

    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, std::uint64_t& aData) {
    static constexpr auto BYTE_COUNT = sizeof(std::remove_reference_t<decltype(aData)>);

    if (aPacketExt->_checkSize(BYTE_COUNT)) {
        // Since ntohll is not available everywhere, we have to convert
        // to network byte order (big endian) manually
        std::uint8_t bytes[BYTE_COUNT];
        std::memcpy(bytes, &(aPacketExt->_buffer[aPacketExt->_readPos]), BYTE_COUNT);
        aData = (static_cast<std::uint64_t>(bytes[0]) << 56) |
                (static_cast<std::uint64_t>(bytes[1]) << 48) |
                (static_cast<std::uint64_t>(bytes[2]) << 40) |
                (static_cast<std::uint64_t>(bytes[3]) << 32) |
                (static_cast<std::uint64_t>(bytes[4]) << 24) |
                (static_cast<std::uint64_t>(bytes[5]) << 16) |
                (static_cast<std::uint64_t>(bytes[6]) <<  8) |
                (static_cast<std::uint64_t>(bytes[7])      );
        aPacketExt->_readPos += BYTE_COUNT;
    }

    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, float& aData) {
    std::int32_t temp;
    static_assert(sizeof(temp) == sizeof(float), "Use integral type that has the same size as float");

    aPacketExt >> temp;
    std::memcpy(&aData, &temp, sizeof(temp));

    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, double& aData) {
    std::int64_t temp;
    static_assert(sizeof(temp) == sizeof(double), "Use integral type that has the same size as double");

    aPacketExt >> temp;
    std::memcpy(&aData, &temp, sizeof(temp));

    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, std::string& aData) {
    // First extract string length
    const auto length = aPacketExt->extractNoThrow<std::uint32_t>();
    if (!*aPacketExt) {
        return *aPacketExt;
    }

    aData.clear();
    if ((length > 0) && aPacketExt->_checkSize(length)) {
        // Then extract characters
        aData.assign(reinterpret_cast<const char*>(
            aPacketExt->_buffer.data()) + aPacketExt->_readPos,
            length
        );

        // Update reading position
        aPacketExt->_readPos += length;
    }

    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, UnicodeString& aData) {
    const auto utf8str = aPacketExt->extractNoThrow<std::string>();
    aData = UniStrConv(FROM_UTF8_STD_STRING, utf8str);
    return *aPacketExt;
}

Packet& operator>>(PacketExtender& aPacketExt, Packet& aData) {
    // First extract data length
    const auto length = aPacketExt->extractNoThrow<std::int32_t>();
    if (!*aPacketExt) {
        return *aPacketExt;
    }

    aData.clear();
    if ((length > 0) && aPacketExt->_checkSize(length)) {
        // Then extract characters
        aData.appendBytes(aPacketExt->_buffer.data() + aPacketExt->_readPos, length);

        // Update reading position
        aPacketExt->_readPos += (std::size_t)length;
    }

    return *aPacketExt;
}

Packet::NoThrowAdapter Packet::noThrow() {
  return {SELF};
}

///////////////////////////////////////////////////////////////////////////
// TESTING VALIDITY                                                      //
///////////////////////////////////////////////////////////////////////////

Packet::operator BoolType() const {
    return _isValid ? &Packet::_checkSize : NULL;
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE                                                               //
///////////////////////////////////////////////////////////////////////////

bool Packet::_checkSize(std::size_t aSize) {
    _isValid = _isValid && (_readPos + aSize <= _buffer.size());
    return _isValid;
}

void Packet::_logExtractionError(const char* aErrorMessage) {
    HG_LOG_WARN(LOG_ID, "Exception caught while extracting data from hg::util::Packet in "
                        "no-throw mode: {}", aErrorMessage);
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
