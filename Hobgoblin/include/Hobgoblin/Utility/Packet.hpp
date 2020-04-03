#ifndef UHOBGOBLIN_UTIL_PACKET_HPP
#define UHOBGOBLIN_UTIL_PACKET_HPP

#include <Hobgoblin/Utility/Exceptions.hpp>
#include <SFML/Network/Packet.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

using PacketBase = sf::Packet;

class Packet : public PacketBase {
public:
    template <class T>
    T extractOrThrow();

    template <class T>
    void insert(T&& value);
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