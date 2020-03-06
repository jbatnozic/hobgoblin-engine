#ifndef UHOBGOBLIN_RN_CONNECTOR_HPP
#define UHOBGOBLIN_RN_CONNECTOR_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Remote_info.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

enum class RN_ConnectorStatus {
    Disconnected,
    Accepting,
    Connecting,
    Connected
};

template <class T>
class RN_Connector {
public:
    const RN_RemoteInfo& getRemoteInfo() const noexcept
        CRTP_METHOD(const T, getRemoteInfo)

    RN_ConnectorStatus getStatus() const noexcept
        CRTP_METHOD(const T, getStatus)

    PZInteger getSendBufferSize() const
        CRTP_METHOD(const T, getSendBufferSize)

    PZInteger getRecvBufferSize() const
        CRTP_METHOD(const T, getRecvBufferSize)
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_CONNECTOR_HPP