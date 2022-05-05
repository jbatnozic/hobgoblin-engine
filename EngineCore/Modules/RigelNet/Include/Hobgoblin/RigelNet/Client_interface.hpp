#ifndef UHOBGOBLIN_RN_CLIENT_INTERFACE_HPP
#define UHOBGOBLIN_RN_CLIENT_INTERFACE_HPP

#include <Hobgoblin/RigelNet/Connector_interface.hpp>
#include <Hobgoblin/RigelNet/Node_interface.hpp>

#include <chrono>
#include <cstdint>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

class RN_ServerInterface;

class RN_ClientInterface : public RN_NodeInterface {
public:
    virtual ~RN_ClientInterface() = default;

    ///////////////////////////////////////////////////////////////////////////
    // CLIENT CONTROL                                                        //
    ///////////////////////////////////////////////////////////////////////////

    //! Start attempting to establish a connection with a remote host.
    //! The local socket will be bound to port 'localPort' (use 0 to let
    //! the OS assign any free port to the application).
    virtual void connect(std::uint16_t localPort,
                         sf::IpAddress serverIp,
                         std::uint16_t serverPort) = 0;

    //! Attempt to connect to a server running on the same machine as in the same 
    //! process as this client.
    virtual void connectLocal(RN_ServerInterface& server) = 0;

    virtual void disconnect(bool notifyRemote) = 0;

    //! Set a time limit after which the connection will be dropped (closed)
    //! if the remote host stops respoding. A value of 0, or negative, will be
    //! treated as infinity (connection never times out).
    virtual void setTimeoutLimit(std::chrono::microseconds limit) = 0;

    virtual void setRetransmitPredicate(RN_RetransmitPredicate pred) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // STATE INSPECTION                                                      //
    ///////////////////////////////////////////////////////////////////////////

    //! Returns true if the client is connected to a server or currently trying
    //! to establish a connection, and false otherwise.
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