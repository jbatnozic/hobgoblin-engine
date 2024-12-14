// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_STREAM_OPERATORS_HPP
#define UHOBGOBLIN_UTIL_STREAM_OPERATORS_HPP

#include <Hobgoblin/Unicode/Unicode_string.hpp>

#include <cstdint>
#include <string>
#include <string_view>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class OutputStream;
class InputStream;

//! Forward-declare PacketExtender.
//!
//! This class is used to define append and extract operations into/from a Packet.
class OutputStreamExtender;
class InputStreamExtender;

// Forward-declare append operators
//clang-format off
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, bool                 aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::int8_t          aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::uint8_t         aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::int16_t         aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::uint16_t        aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::int32_t         aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::uint32_t        aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::int64_t         aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::uint64_t        aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, float                aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, double               aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, std::string_view     aData);
OutputStream& operator<<(OutputStreamExtender& aStreamExtender, const UnicodeString& aData);
// OutputStream& operator<<(OutputStreamExtender& aPacket, const Packet&        aData);
//clang-format on

// Forward-declare extract operators
//clang-format off
InputStream& operator>>(InputStreamExtender& aStreamExtender, bool&                aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, std::int8_t&         aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, std::uint8_t&        aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, std::int16_t&        aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, std::uint16_t&       aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, std::int32_t&        aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, std::uint32_t&       aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, std::int64_t&        aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, std::uint64_t&       aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, float&               aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, double&              aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, std::string&         aData);
InputStream& operator>>(InputStreamExtender& aStreamExtender, UnicodeString&       aData);
// InputStream& operator>>(InputStreamExtender& aStreamExtender, Packet&              aData);
//clang-format on

// TODO (description)
class OutputStreamExtender {
public:
    OutputStream& operator*() {
        return _ostream;
    }

    OutputStream* operator->() {
        return &_ostream;
    }

private:
    friend class OutputStream;

    OutputStreamExtender(OutputStream& aOStream) : _ostream{aOStream} {}

    OutputStream& _ostream;
};

// TODO (description)
class InputStreamExtender {
public:
    InputStream& operator*() {
        return _istream;
    }

    InputStream* operator->() {
        return &_istream;
    }

private:
    friend class InputStream;

    InputStreamExtender(InputStream& aIStream) : _istream{aIStream} {}

    InputStream& _istream;
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_STREAM_OPERATORS_HPP
