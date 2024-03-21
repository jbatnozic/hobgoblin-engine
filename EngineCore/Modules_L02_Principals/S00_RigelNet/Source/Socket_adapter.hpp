#ifndef UHOBGOBLIN_RN_SOCKET_ADAPTER_HPP
#define UHOBGOBLIN_RN_SOCKET_ADAPTER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Configuration.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <SFML/Network.hpp>

#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
#include <ZTCpp.hpp>
namespace zt = jbatnozic::ztcpp;
#endif

#include <cstdint>
#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

//! This class abstracts away different socket implementations that RigelNet can use.
//! In all cases, RN_SocketAdapter acts as a non-blocking socket.
//! For now, always constructs an UDP socket, regardless of specified protocol (TODO)
//! All function calls throw only on errors from which RigelNet cannot recover. Otherwise
//! they return an appropriate status code.
class RN_SocketAdapter {
public:
    enum class Status {
        OK,          //! Operation completed successfully
        NotReady,    //! Operation could not be completed (it would block, try again later)
        Disconnected //! Operation could not be completed because the remote closed the connection (TCP only)
                     //! Note: SFML sockets seem to sometimes return "Disconnected" even for UDP sockets
    };

    //! Throws TracedLogicError, only if an unsupported RN_NetworkingStack is requested.
    RN_SocketAdapter(RN_Protocol aProtocol, RN_NetworkingStack aNetworkingStack);

    //! Prepare the socket for use.
    //! Throws TracedRuntimeError on failure (realistically should not happen).
    void init(PZInteger aRecvBufferSize);

    //! Bind the socker to a local address (not too important) and port.
    //! Throws TracedRuntimeError on failure (for example if the port is taken).
    void bind(sf::IpAddress aIpAddress, std::uint16_t aLocalPort);

    //! Attempt to send a packet.
    //! Returns true on success.
    //! Throws TracedRuntimeError or TracedLogicError on unrecoverable error.
    Status send(util::Packet& aPacket,
                const sf::IpAddress& aTargetAddress,
                std::uint16_t aTargetPort);

    //! Receive data if available.
    //! Returns Status::OK if any data was received.
    //! Throws TracedRuntimeError or TracedLogicError on unrecoverable error.
    Status recv(util::Packet& aPacket, 
                sf::IpAddress& aRemoteAddress, 
                std::uint16_t& aRemotePort);

    //! Close the socket and destroy the underlying implementation.
    //! Does nothing if the socket isn't initialized.
    //! DON'T call any methods (except init() or getters) after close() is called!
    //! Shouldn't ever throw exceptions.
    void close();

    //! Returns the local port the socket was bound to.
    std::uint16_t getLocalPort() const;

    //! Returns the protocol the socket was created for.
    RN_Protocol getProtocol() const noexcept;

    //! Returns the protocol the networking stack was created for.
    RN_NetworkingStack getNetworkingStack() const noexcept;

private:
    RN_Protocol _protocol;
    RN_NetworkingStack _networkingStack;

    std::variant<
        int, // Dummy
        sf::UdpSocket,
    #ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
        zt::Socket
    #endif
    > _socket;

    //! Used to 'catch' data received by sockets
    std::vector<std::uint8_t> _recvBuffer;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_SOCKET_ADAPTER_HPP