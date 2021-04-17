#ifndef UHOBGOBLIN_RN_SERVER_INTERFACE_HPP
#define UHOBGOBLIN_RN_SERVER_INTERFACE_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Connector_interface.hpp>

#include <chrono>
#include <cstdint>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

class RN_ServerInterface {
public:
    ///////////////////////////////////////////////////////////////////////////
    // SERVER CONTROL                                                        //
    ///////////////////////////////////////////////////////////////////////////

    virtual void start(std::uint16_t localPort, std::string passphrase) = 0;

    virtual void stop() = 0;

    virtual bool isRunning() const = 0;

    virtual void update(RN_UpdateMode mode) = 0;

    virtual void resize(PZInteger newSize) = 0;

    virtual void setTimeoutLimit(std::chrono::microseconds limit) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // CLIENT MANAGEMENT                                                     //
    ///////////////////////////////////////////////////////////////////////////

    virtual const RN_ConnectorInterface& getClientConnector(PZInteger clientIndex) const = 0;

    virtual void swapClients(PZInteger index1, PZInteger index2) = 0;

    virtual void kickClient(PZInteger clientIndex) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // STATE INSPECTION                                                      //
    ///////////////////////////////////////////////////////////////////////////

    virtual PZInteger getSize() const = 0;

    virtual const std::string& getPassphrase() const = 0;

    virtual std::chrono::microseconds getTimeoutLimit() const = 0;

    virtual std::uint16_t getLocalPort() const = 0;    

    virtual PZInteger getSenderIndex() const = 0;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_SERVER_INTERFACE_HPP