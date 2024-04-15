// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_RN_CONNECTOR_INTERFACE_HPP
#define UHOBGOBLIN_RN_CONNECTOR_INTERFACE_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Remote_info.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

enum class RN_ConnectorStatus {
    Disconnected, //! No connection established. Connector idle.
    Accepting,    //! (Server-side) Currently trying to establish a connection with a client.
    Connecting,   //! (Client-side) Currently trying to establish a connection with the server.
    Connected     //! Connection established and active.
};

class RN_ConnectorInterface {
public:
    //! Current status of the connector.
    virtual RN_ConnectorStatus getStatus() const noexcept = 0;

    //! Returns true is the connector is connected to another within the same process.
    virtual bool isConnectedLocally() const noexcept = 0;

    //! Info about the remote corresponding to this connector.
    //! Returns undefined data if the connector is in Disconnected state.
    //! Latency of the remote info is valid only when the connector is in the Connected state.
    virtual const RN_RemoteInfo& getRemoteInfo() const noexcept = 0;

    //! Size of the send buffer in bytes.
    virtual PZInteger getSendBufferSize() const = 0;

    //! Size of the receive buffer in bytes.
    virtual PZInteger getRecvBufferSize() const = 0;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_CONNECTOR_INTERFACE_HPP

// clang-format on
