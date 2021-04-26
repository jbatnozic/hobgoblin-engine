#ifndef UHOBGOBLIN_RN_CLIENT_INTERFACE_HPP
#define UHOBGOBLIN_RN_CLIENT_INTERFACE_HPP

#include <Hobgoblin/RigelNet/Connector_interface.hpp>
#include <Hobgoblin/RigelNet/Node_interface.hpp>

#include <chrono>
#include <cstdint>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

class RN_ServerInterface;

class RN_ClientInterface : public RN_NodeInterface {
public:
    ///////////////////////////////////////////////////////////////////////////
    // CLIENT CONTROL                                                        //
    ///////////////////////////////////////////////////////////////////////////

    virtual void connect(std::uint16_t localPort,
                         sf::IpAddress serverIp,
                         std::uint16_t serverPort) = 0;

    virtual void connectLocal(RN_ServerInterface& server) = 0;

    virtual void disconnect(bool notifyRemote) = 0;

    virtual void setTimeoutLimit(std::chrono::microseconds limit) = 0;

    virtual void setRetransmitPredicate(RN_RetransmitPredicate pred) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // STATE INSPECTION                                                      //
    ///////////////////////////////////////////////////////////////////////////

    virtual bool isRunning() const = 0;

    virtual const std::string& getPassphrase() const = 0;

    virtual std::chrono::microseconds getTimeoutLimit() const = 0;

    virtual std::uint16_t getLocalPort() const = 0;  

    virtual const RN_ConnectorInterface& getServerConnector() const = 0;

    virtual PZInteger getClientIndex() const  = 0;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_CLIENT_INTERFACE_HPP