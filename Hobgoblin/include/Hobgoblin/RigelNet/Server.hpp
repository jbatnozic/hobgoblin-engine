#ifndef UHOBGOBLIN_RN_SERVER_HPP
#define UHOBGOBLIN_RN_SERVER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Node.hpp>
#include <Hobgoblin/RigelNet/Connector.hpp>

#include <chrono>
#include <cstdint>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

// TODO constexpr Any port

struct RN_RemoteInfo;

template <class T, class U>
class RN_Server : public RN_Node {
public:
    RN_Server(RN_NodeType type);

    // Running:
    void start(std::uint16_t localPort, std::string passphrase)
        CRTP_METHOD(T, start, localPort, passphrase)

    void stop()
        CRTP_METHOD(T, stop)

    bool isRunning() const
        CRTP_METHOD(T, isRunning)

    void update(RN_UpdateMode mode)
        CRTP_METHOD(T, update, mode)

    // Client management:
    const RN_Connector<U>& getClient(PZInteger clientIndex) const
        CRTP_METHOD(const T, getConnector, clientIndex)

    void swapClients(PZInteger index1, PZInteger index2)
        CRTP_METHOD(T, swapClients, index1, index2)

    void kickClient(PZInteger clientIndex)
        CRTP_METHOD(T, kickClient, clientIndex)

    // Miscellaneous:
    PZInteger getSenderIndex() const
        CRTP_METHOD(const T, getSenderIndex)

    std::uint16_t getLocalPort() const
        CRTP_METHOD(const T, getLocalPort)

    PZInteger getSize() const
        CRTP_METHOD(const T, getSize)

    void resize(PZInteger newSize)
        CRTP_METHOD(T, resize)

    std::chrono::microseconds getTimeoutLimit() const
        CRTP_METHOD(const T, getTimeoutLimit)

    void setTimeoutLimit(std::chrono::microseconds limit)
        CRTP_METHOD(T, setTimeoutLimit)

    const std::string& getPassphrase() const
        CRTP_METHOD(const T, getPassphrase)
};

template <class T, class U>
RN_Server<T, U>::RN_Server(RN_NodeType type)
    : RN_Node{type}
{
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_SERVER_HPP
