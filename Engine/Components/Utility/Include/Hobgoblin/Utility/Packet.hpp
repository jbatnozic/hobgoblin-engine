#ifndef UHOBGOBLIN_UTIL_PACKET_HPP
#define UHOBGOBLIN_UTIL_PACKET_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Exceptions.hpp>
#include <SFML/Network/Packet.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

using PacketBase = sf::Packet;

class Packet : public PacketBase {
public:
    template <class T>
    T extractOrThrow();

    /// Returns a pointer to where the data to be read is stored and advances the internal seek counter by byteCount.
    /// Users must persist the packet until they're done reading the data.
    void* extractBytes(std::size_t byteCount);

    template <class T>
    void insert(T&& value);

    friend PacketBase& operator<<(PacketBase& dstPacket, const Packet& srcPacket);
    friend PacketBase& operator>>(PacketBase& dstPacket, Packet& srcPacket);
};

template <class T>
T Packet::extractOrThrow() {
    T value;
    if (!(SELF >> value)) {
        throw TracedRuntimeError{"Bad read from Packet"};
    }
    return value;
}

template <class T>
void Packet::insert(T&& value) {
    SELF << std::forward<T>(value);
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_PACKET_HPP