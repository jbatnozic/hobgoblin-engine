#ifndef UHOBGOBLIN_RN_PACKET_WRAPPER_HPP
#define UHOBGOBLIN_RN_PACKET_WRAPPER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include <stdexcept>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

class RN_PacketReadError : public TracedException {
public:
    using TracedException::TracedException;
};

namespace detail {

class RN_PacketWrapper {
public:
    util::Packet packet;

    template <class T>
    T extract();

    template <class T>
    T extractOrThrow();

    template <class T>
    void insert(T&& value);
};

template <class T>
T RN_PacketWrapper::extract() {
    T value;
    packet >> value;
    return value;
}

template <class T>
T RN_PacketWrapper::extractOrThrow() {
    T value;
    if (!(packet >> value)) {
        throw RN_PacketReadError{"Bad read from hg::util::Packet"};
    }
    return value;
}

template <class T>
void RN_PacketWrapper::insert(T&& value) {
    packet.insert(std::forward<T>(value));
}

template <class ...NoArgs>
typename std::enable_if_t<sizeof...(NoArgs) == 0, void> PackArgs(RN_PacketWrapper& packetWrap) {
    // Do nothing
}

template <class ArgsHead, class ...ArgsRest>
void PackArgs(RN_PacketWrapper& packetWrap, ArgsHead argsHead, ArgsRest&&... argsRest) {
    packetWrap.insert(argsHead);
    PackArgs<ArgsRest...>(packetWrap, std::forward<ArgsRest>(argsRest)...);
}

} // namespace detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_PACKET_WRAPPER_HPP
