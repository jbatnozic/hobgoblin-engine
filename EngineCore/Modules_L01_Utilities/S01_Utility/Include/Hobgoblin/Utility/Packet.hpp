#ifndef UHOBGOBLIN_UTIL_PACKET_HPP
#define UHOBGOBLIN_UTIL_PACKET_HPP

#include <Hobgoblin/HGExcept.hpp>
#include <SFML/Network/Packet.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

using PacketBase = sf::Packet;

class Packet : public PacketBase {
public:
    //! Constructs an empty packet.
    Packet();

    //! Regular destructor, nothing to see here.
    ~Packet() override;

    // Inherited from PacketBase:
    //     void append(const void* data, std::size_t sizeInBytes);
    //     void clear();
    //     const void* getData() const;
    //     std::size_t getDataSize() const;
    //     bool endOfPacket() const;

    //! Same as getData() but you can edit the data.
    //! Note: Data in the packet is stored in network order (big-endian)
    //! Note: Appending to the packet may invalidate the returned pointer!
    void* getMutableData();

    //!
    std::size_t getReadPos() const;

    //! Number of bytes remaining to be read.
    std::size_t getRemainingDataSize() const;

    template <class T>
    T extract();

    //! 
    template <class T>
    T extractOrThrow();

    //! Returns a pointer to where the data to be read is stored and advances the
    //! internal seek counter by byteCount. Users must persist the packet until 
    //! they're done reading the data.
    //! Note: Appending to the packet may invalidate the returned pointer!
    void* extractBytes(std::size_t byteCount);

    //! Appends a value to the packet, same as operator <<.
    template <class T>
    void insert(T&& value);

    //! Inserts a packet into another packet, with a leading 32-bit uint marking byte count.
    friend PacketBase& operator<<(PacketBase& dstPacket, const Packet& srcPacket);

    //! Opposite of above
    friend PacketBase& operator>>(PacketBase& dstPacket, Packet& srcPacket);

    class ReadError : public TracedRuntimeError {
    public:
        using TracedRuntimeError::TracedRuntimeError;
    };
};

template <class T>
T Packet::extract() {
    T value;
    SELF >> value;
    return value;
}

template <class T>
T Packet::extractOrThrow() {
    T value;
    if (!(SELF >> value)) {
        HG_THROW_TRACED(ReadError, 0, "Bad read from Packet");
    }
    return value;
}

template <class T>
void Packet::insert(T&& value) {
    SELF << std::forward<T>(value);
}

template <class ...NoArgs>
typename std::enable_if_t<sizeof...(NoArgs) == 0, void> PackArgs(util::Packet& packet) {
    // Do nothing
}

template <class ArgsHead, class ...ArgsRest>
void PackArgs(util::Packet& packet, ArgsHead argsHead, ArgsRest&&... argsRest) {
    packet.insert(argsHead);
    PackArgs<ArgsRest...>(packet, std::forward<ArgsRest>(argsRest)...);
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

///////////////////////////////////////////////////////////////////////////
// MAKE SUPER PACKETBASE SUPPORTS ALL FIXED STD INTS ON ALL PLATFORMS    //
///////////////////////////////////////////////////////////////////////////

// Note: This is likely a temporary solution necessitated by 2 facts:
//   - PacketBase is currently an alias to sf::Packet
//   - We're using an older SFML version where std fixed width ints aren't all handled properly

#ifndef _MSC_VER
#define hgutil jbatnozic::hobgoblin::util

namespace sf {

hgutil::PacketBase& operator<<(hgutil::PacketBase& aPacketBase, std::int64_t  aData);
hgutil::PacketBase& operator>>(hgutil::PacketBase& aPacketBase, std::int64_t& aData);

hgutil::PacketBase& operator<<(hgutil::PacketBase& aPacketBase, std::uint64_t  aData);
hgutil::PacketBase& operator>>(hgutil::PacketBase& aPacketBase, std::uint64_t& aData);

} // namespace sf

#undef hgutil
#endif

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_PACKET_HPP