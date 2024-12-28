// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_PACKET_HPP
#define UHOBGOBLIN_UTIL_PACKET_HPP

#include <Hobgoblin/Utility/Stream_buffer.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

using PacketReadError = StreamReadError;

class Packet final : public BufferStream {
public:
    //! Same as `endOfStream()`.
    bool endOfPacket() const {
        return endOfStream();
    }
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_PACKET_HPP
