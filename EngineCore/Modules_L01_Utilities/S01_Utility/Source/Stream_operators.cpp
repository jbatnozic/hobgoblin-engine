// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Utility/Stream_operators.hpp>

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Unicode/String_conversions.hpp>
#include <Hobgoblin/Utility/Stream_input.hpp>
#include <Hobgoblin/Utility/Stream_output.hpp>

#ifdef _MSC_VER
#include <winsock.h> // for hton & friends
#else
#include <arpa/inet.h> // for hton & friends
#endif

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, bool aData) {
    aOutputStreamExt << static_cast<std::int8_t>(aData);
    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, std::int8_t aData) {
    (void)aOutputStreamExt->write(&aData, sizeof(aData));
    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, std::uint8_t aData) {
    (void)aOutputStreamExt->write(&aData, sizeof(aData));
    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, std::int16_t aData) {
    std::int16_t toWrite = static_cast<std::int16_t>(htons(static_cast<std::uint16_t>(aData)));
    (void)aOutputStreamExt->write(&toWrite, sizeof(toWrite));
    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, std::uint16_t aData) {
    std::uint16_t toWrite = htons(aData);
    (void)aOutputStreamExt->write(&toWrite, sizeof(toWrite));
    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, std::int32_t aData) {
    std::int32_t toWrite = static_cast<std::int32_t>(htonl(static_cast<std::uint32_t>(aData)));
    (void)aOutputStreamExt->write(&toWrite, sizeof(toWrite));
    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, std::uint32_t aData) {
    std::uint32_t toWrite = htonl(aData);
    (void)aOutputStreamExt->write(&toWrite, sizeof(toWrite));
    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, std::int64_t aData) {
    // Since htonll is not available everywhere, we have to convert
    // to network byte order (big endian) manually
    // clang-format off
    std::uint8_t toWrite[] = {static_cast<std::uint8_t>((aData >> 56) & 0xFF),
                              static_cast<std::uint8_t>((aData >> 48) & 0xFF),
                              static_cast<std::uint8_t>((aData >> 40) & 0xFF),
                              static_cast<std::uint8_t>((aData >> 32) & 0xFF),
                              static_cast<std::uint8_t>((aData >> 24) & 0xFF),
                              static_cast<std::uint8_t>((aData >> 16) & 0xFF),
                              static_cast<std::uint8_t>((aData >>  8) & 0xFF),
                              static_cast<std::uint8_t>((aData      ) & 0xFF)};
    // clang-format on
    (void)aOutputStreamExt->write(&toWrite, sizeof(toWrite));
    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, std::uint64_t aData) {
    // Since htonll is not available everywhere, we have to convert
    // to network byte order (big endian) manually
    // clang-format off
    std::uint8_t toWrite[] = {static_cast<std::uint8_t>((aData >> 56) & 0xFF),
                              static_cast<std::uint8_t>((aData >> 48) & 0xFF),
                              static_cast<std::uint8_t>((aData >> 40) & 0xFF),
                              static_cast<std::uint8_t>((aData >> 32) & 0xFF),
                              static_cast<std::uint8_t>((aData >> 24) & 0xFF),
                              static_cast<std::uint8_t>((aData >> 16) & 0xFF),
                              static_cast<std::uint8_t>((aData >>  8) & 0xFF),
                              static_cast<std::uint8_t>((aData      ) & 0xFF)};
    // clang-format on
    (void)aOutputStreamExt->write(&toWrite, sizeof(toWrite));
    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, float aData) {
    std::int32_t temp;
    static_assert(sizeof(temp) == sizeof(aData), "Use integral type that has the same size as float");

    std::memcpy(&temp, &aData, sizeof(temp));
    aOutputStreamExt << temp;

    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, double aData) {
    std::int64_t temp;
    static_assert(sizeof(temp) == sizeof(aData), "Use integral type that has the same size as double");

    std::memcpy(&temp, &aData, sizeof(temp));
    aOutputStreamExt << temp;

    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, std::string_view aData) {
    aOutputStreamExt << static_cast<std::uint32_t>(aData.size());
    (void)aOutputStreamExt->write(aData.data(), static_cast<std::int64_t>(aData.size()));
    return *aOutputStreamExt;
}

OutputStream& operator<<(OutputStreamExtender& aOutputStreamExt, const UnicodeString& aData) {
    const auto utf8str = UniStrConv(TO_UTF8_STD_STRING, aData);
    aOutputStreamExt << utf8str;
    return *aOutputStreamExt;
}

///////////////////////////////////////////////////////////////////////////
// EXTRACTING                                                            //
///////////////////////////////////////////////////////////////////////////

InputStream& operator>>(InputStreamExtender& aInputStreamExt, bool& aData) {
    aData = (aInputStreamExt->extractNoThrow<std::int8_t>() != 0);
    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, std::int8_t& aData) {
    static constexpr auto BYTE_COUNT =
        static_cast<std::int64_t>(sizeof(std::remove_reference_t<decltype(aData)>));

    (void)aInputStreamExt->read(&aData, BYTE_COUNT);

    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, std::uint8_t& aData) {
    static constexpr auto BYTE_COUNT =
        static_cast<std::int64_t>(sizeof(std::remove_reference_t<decltype(aData)>));

    (void)aInputStreamExt->read(&aData, BYTE_COUNT);

    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, std::int16_t& aData) {
    static constexpr auto BYTE_COUNT =
        static_cast<std::int64_t>(sizeof(std::remove_reference_t<decltype(aData)>));

    (void)aInputStreamExt->read(&aData, BYTE_COUNT);
    aData = static_cast<std::int16_t>(ntohs(static_cast<std::uint16_t>(aData)));

    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, std::uint16_t& aData) {
    static constexpr auto BYTE_COUNT =
        static_cast<std::int64_t>(sizeof(std::remove_reference_t<decltype(aData)>));

    (void)aInputStreamExt->read(&aData, BYTE_COUNT);
    aData = ntohs(aData);

    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, std::int32_t& aData) {
    static constexpr auto BYTE_COUNT =
        static_cast<std::int64_t>(sizeof(std::remove_reference_t<decltype(aData)>));

    (void)aInputStreamExt->read(&aData, BYTE_COUNT);
    aData = static_cast<std::int32_t>(ntohl(static_cast<std::uint32_t>(aData)));

    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, std::uint32_t& aData) {
    static constexpr auto BYTE_COUNT =
        static_cast<std::int64_t>(sizeof(std::remove_reference_t<decltype(aData)>));

    (void)aInputStreamExt->read(&aData, BYTE_COUNT);
    aData = ntohl(aData);

    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, std::int64_t& aData) {
    static constexpr auto BYTE_COUNT =
        static_cast<std::int64_t>(sizeof(std::remove_reference_t<decltype(aData)>));

    // Since ntohll is not available everywhere, we have to convert
    // to network byte order (big endian) manually
    std::uint8_t bytes[BYTE_COUNT];
    const auto   readCnt = aInputStreamExt->read(&bytes, BYTE_COUNT);
    // clang-format off
    aData = (static_cast<std::int64_t>(bytes[0]) << 56) |
            (static_cast<std::int64_t>(bytes[1]) << 48) |
            (static_cast<std::int64_t>(bytes[2]) << 40) |
            (static_cast<std::int64_t>(bytes[3]) << 32) |
            (static_cast<std::int64_t>(bytes[4]) << 24) |
            (static_cast<std::int64_t>(bytes[5]) << 16) |
            (static_cast<std::int64_t>(bytes[6]) <<  8) |
            (static_cast<std::int64_t>(bytes[7])      ) ;
    // clang-format on

    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, std::uint64_t& aData) {
    static constexpr auto BYTE_COUNT =
        static_cast<std::int64_t>(sizeof(std::remove_reference_t<decltype(aData)>));

    // Since ntohll is not available everywhere, we have to convert
    // to network byte order (big endian) manually
    std::uint8_t bytes[BYTE_COUNT];
    const auto   readCnt = aInputStreamExt->read(&bytes, BYTE_COUNT);
    // clang-format off
    aData = (static_cast<std::uint64_t>(bytes[0]) << 56) |
            (static_cast<std::uint64_t>(bytes[1]) << 48) |
            (static_cast<std::uint64_t>(bytes[2]) << 40) |
            (static_cast<std::uint64_t>(bytes[3]) << 32) |
            (static_cast<std::uint64_t>(bytes[4]) << 24) |
            (static_cast<std::uint64_t>(bytes[5]) << 16) |
            (static_cast<std::uint64_t>(bytes[6]) <<  8) |
            (static_cast<std::uint64_t>(bytes[7])      ) ;
    // clang-format on

    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, float& aData) {
    std::int32_t temp;
    static_assert(sizeof(temp) == sizeof(float), "Use integral type that has the same size as float");

    aInputStreamExt >> temp;
    std::memcpy(&aData, &temp, sizeof(temp));

    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, double& aData) {
    std::int64_t temp;
    static_assert(sizeof(temp) == sizeof(double), "Use integral type that has the same size as double");

    aInputStreamExt >> temp;
    std::memcpy(&aData, &temp, sizeof(temp));

    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, std::string& aData) {
    // First extract string length
    const auto length = aInputStreamExt->extractNoThrow<std::uint32_t>();
    if (!*aInputStreamExt) {
        return *aInputStreamExt;
    }

    aData.clear();
    if (length > 0) {
        aData.resize(length);
        (void)aInputStreamExt->read(aData.data(), static_cast<PZInteger>(length));
    }

    return *aInputStreamExt;
}

InputStream& operator>>(InputStreamExtender& aInputStreamExt, UnicodeString& aData) {
    const auto utf8str = aInputStreamExt->extractNoThrow<std::string>();
    aData              = UniStrConv(FROM_UTF8_STD_STRING, utf8str);
    return *aInputStreamExt;
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
