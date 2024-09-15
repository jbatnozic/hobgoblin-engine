// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_RN_SERVER_INTERFACE_HPP
#define UHOBGOBLIN_RN_SERVER_INTERFACE_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Connector_interface.hpp>
#include <Hobgoblin/RigelNet/Node_interface.hpp>
#include <Hobgoblin/RigelNet/Retransmit_predicate.hpp>

#include <chrono>
#include <cstdint>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

class RN_ServerInterface : public RN_NodeInterface {
public:
    virtual ~RN_ServerInterface() = default;

    ///////////////////////////////////////////////////////////////////////////
    // SERVER CONTROL                                                        //
    ///////////////////////////////////////////////////////////////////////////

    virtual void start(std::uint16_t localPort) = 0;

    virtual void stop(bool aNotifyClients = true) = 0;

    virtual void resize(PZInteger newSize) = 0;

    virtual void setTimeoutLimit(std::chrono::microseconds limit) = 0;

    virtual void setRetransmitPredicate(RN_RetransmitPredicate pred) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // CLIENT MANAGEMENT                                                     //
    ///////////////////////////////////////////////////////////////////////////

    virtual const RN_ConnectorInterface& getClientConnector(PZInteger clientIndex) const = 0;

    virtual RN_ConnectorInterface& getClientConnector(PZInteger clientIndex) = 0;

    //! Equivalent to `getClientConnector(aClientIndex).disconnect(aNotifyRemote, aMessage)`.
    virtual void kickClient(PZInteger          aClientIndex,
                            bool               aNotifyRemote = true,
                            const std::string& aMessage      = "") = 0;

    ///////////////////////////////////////////////////////////////////////////
    // STATE INSPECTION                                                      //
    ///////////////////////////////////////////////////////////////////////////

    virtual bool isRunning() const = 0;

    virtual PZInteger getSize() const = 0;

    virtual const std::string& getPassphrase() const = 0;

    virtual std::chrono::microseconds getTimeoutLimit() const = 0;

    virtual std::uint16_t getLocalPort() const = 0;

    virtual int getSenderIndex() const = 0;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_SERVER_INTERFACE_HPP
