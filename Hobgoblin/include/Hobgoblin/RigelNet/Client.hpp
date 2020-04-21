#ifndef UHOBGOBLIN_RN_CLIENT_HPP
#define UHOBGOBLIN_RN_CLIENT_HPP

#include <Hobgoblin/RigelNet/Node.hpp>
#include <Hobgoblin/RigelNet/Connector.hpp>

#include <chrono>
#include <cstdint>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

struct RN_RemoteInfo;

template <class T, class U>
class RN_Client : public RN_Node {
public:
    RN_Client(RN_NodeType type);

    // Running:
    void connect(std::uint16_t localPort, sf::IpAddress serverIp, std::uint16_t serverPort, std::string passphrase)
        CRTP_METHOD(T, connect, localPort, serverIp, serverPort, passphrase)

    void disconnect(bool notifyRemote)
        CRTP_METHOD(T, disconnect, notifyRemote)

    void update(RN_UpdateMode mode)
        CRTP_METHOD(T, update, mode)

    // Utility:
    const RN_Connector<U>& getServer() const
        CRTP_METHOD(const T, getServer)

    std::chrono::microseconds getTimeoutLimit() const
        CRTP_METHOD(const T, getTimeoutLimit)

    void setTimeoutLimit(std::chrono::microseconds limit)
        CRTP_METHOD(T, setTimeoutLimit, limit)

    const std::string& getPassphrase() const
        CRTP_METHOD(const T, getPassphrase)
};

template <class T, class U>
RN_Client<T, U>::RN_Client(RN_NodeType type)
    : RN_Node{type}
{
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_CLIENT_HPP
