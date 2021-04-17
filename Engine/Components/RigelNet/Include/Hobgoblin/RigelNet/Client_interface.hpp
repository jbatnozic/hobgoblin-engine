#ifndef UHOBGOBLIN_RN_CLIENT_INTERFACE_HPP
#define UHOBGOBLIN_RN_CLIENT_INTERFACE_HPP

#include <Hobgoblin/RigelNet/Connector_interface.hpp>

#include <chrono>
#include <cstdint>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

class RN_ClientInterface {
public:
    ///////////////////////////////////////////////////////////////////////////
    // CLIENT CONTROL                                                        //
    ///////////////////////////////////////////////////////////////////////////

    virtual void connect(std::uint16_t localPort,
                         sf::IpAddress serverIp,
                         std::uint16_t serverPort, 
                         std::string passphrase) = 0;

    virtual void disconnect(bool notifyRemote) = 0;

    virtual void update(RN_UpdateMode mode) = 0;

    virtual void setTimeoutLimit(std::chrono::microseconds limit) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // STATE INSPECTION                                                      //
    ///////////////////////////////////////////////////////////////////////////

    virtual const std::string& getPassphrase() const = 0;

    virtual std::chrono::microseconds getTimeoutLimit() const = 0;

    virtual const RN_ConnectorInterface& getServerConnector() const = 0;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_CLIENT_INTERFACE_HPP