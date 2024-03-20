
#include "Socket_adapter.hpp"

#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

namespace {

inline
bool UseSfSocket(RN_Protocol protocol, RN_NetworkingStack networkingStack) {
    return (networkingStack == RN_NetworkingStack::Default);
}

#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
inline
bool UseZtSocket(RN_Protocol protocol, RN_NetworkingStack networkingStack) {
    return (networkingStack == RN_NetworkingStack::ZeroTier);
}
#endif
} // namespace

RN_SocketAdapter::RN_SocketAdapter(RN_Protocol aProtocol, RN_NetworkingStack aNetworkingStack)
    : _protocol{aProtocol}
    , _networkingStack{aNetworkingStack}
    , _socket{0}
{
    if (UseSfSocket(_protocol, _networkingStack)) {
        _socket.emplace<sf::UdpSocket>();
    }
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    else if (UseZtSocket(_protocol, _networkingStack)) {
        _socket.emplace<zt::Socket>();
    }
#endif
    else {
        HG_UNREACHABLE("Unsupported networking stack requested. "
                       "(Did you compile RigelNet with the correct configuration?)");
    }
}

void RN_SocketAdapter::init(PZInteger aRecvBufferSize) {
    if (UseSfSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<sf::UdpSocket>(_socket);
        socket.setBlocking(false);
    }
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    else if (UseZtSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<zt::Socket>(_socket);
        const auto res = socket.init(zt::SocketDomain::InternetProtocol_IPv4,
                                     zt::SocketType::Datagram);
        // This will only throw if the ZeroTier service wasn't initialized, 
        // or if some strange unrecoverable error happens to the socket.
        if (res.hasError()) {
            HG_THROW_TRACED(TracedRuntimeError, res.getError().errorCode, res.getError().message);
        }

        _recvBuffer.resize(pztos(aRecvBufferSize));
    }
#endif
}

void RN_SocketAdapter::bind(sf::IpAddress aIpAddress, std::uint16_t aLocalPort) {
    if (UseSfSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<sf::UdpSocket>(_socket);
        if (socket.bind(aLocalPort, aIpAddress) != sf::Socket::Done) {
            HG_THROW_TRACED(TracedRuntimeError, 0, "Failed to bind port.");
        }
    }
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    else if (UseZtSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<zt::Socket>(_socket);
        const auto res = socket.bind(zt::IpAddress::ipv4FromString(aIpAddress.toString()), aLocalPort);
        if (res.hasError()) {
            HG_THROW_TRACED(TracedRuntimeError, res.getError().errorCode, res.getError().message);
        }
    }
#endif
}

RN_SocketAdapter::Status RN_SocketAdapter::send(util::Packet& aPacket,
                                                const sf::IpAddress& aTargetAddress,
                                                std::uint16_t aTargetPort) {
    if (aPacket.getDataSize() == 0u) return Status::OK;

    if (UseSfSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<sf::UdpSocket>(_socket);
    
        // TODO (temporary solution)
        switch (socket.send(aPacket.getData(), pztos(aPacket.getDataSize()), aTargetAddress, aTargetPort)) {
        case sf::Socket::Done:
            return Status::OK;

        case sf::Socket::NotReady:
            return Status::NotReady;

        case sf::Socket::Partial:
            // SFML's UDP socket can't return this status
            HG_UNREACHABLE("Received unexpected sf::Socket::Partial status from UDP socket.");

        case sf::Socket::Disconnected:
            return Status::Disconnected;

        case sf::Socket::Error:
        default:
            HG_THROW_TRACED(TracedRuntimeError, 0, "Socket reached an unrecoverable error state.");
        }
    }
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    else if (UseZtSocket(_protocol, _networkingStack)) {
        // TODO: Socket disconnected is not handled properly (throws exception instead
        // of returning Status::Disconnected. Not a huge concern as RigelNet only
        // supports UDP for now anyway.

        auto& socket = std::get<zt::Socket>(_socket);

        const auto pollres = socket.pollEvents(zt::PollEventBitmask::ReadyToSend);
        if (pollres.hasError()) {
            HG_THROW_TRACED(TracedRuntimeError, pollres.getError().errorCode, pollres.getError().message);
        }
        if ((*pollres & zt::PollEventBitmask::ReadyToSend) == 0) {
            return Status::NotReady;
        }

        const auto res = socket.sendTo(aPacket.getData(), aPacket.getDataSize(),
                                       zt::IpAddress::ipv4FromString(aTargetAddress.toString()), aTargetPort);
        if (res.hasError()) {
            HG_THROW_TRACED(TracedRuntimeError, res.getError().errorCode, res.getError().message);
        }

        return Status::OK;
    }
#endif
    else {
        HG_UNREACHABLE("Unsupported networking stack requested. "
                       "(Did you compile RigelNet with the correct configuration?)");
    }
}

RN_SocketAdapter::Status RN_SocketAdapter::recv(util::Packet& aPacket, 
                                                sf::IpAddress& aRemoteAddress, 
                                                std::uint16_t& aRemotePort) {
    if (UseSfSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<sf::UdpSocket>(_socket);

        // TODO: temporary solution
        sf::Packet sfPacket;
        const auto status = socket.receive(sfPacket, aRemoteAddress, aRemotePort);
        aPacket.appendBytes(sfPacket.getData(), stopz(sfPacket.getDataSize()));

        switch (status) {
        case sf::Socket::Done:
            return Status::OK;

        case sf::Socket::NotReady:
            return Status::NotReady;

        case sf::Socket::Partial:
            // SFML's UDP socket can't return this status
            HG_UNREACHABLE("Received unexpected sf::Socket::Partial status from UDP socket.");

        case sf::Socket::Disconnected:
            return Status::Disconnected;

        case sf::Socket::Error:
        default:
            HG_THROW_TRACED(TracedRuntimeError, 0, "Socket reached an unrecoverable error state.");
        }
    }
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    else if (UseZtSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<zt::Socket>(_socket);

        const auto pollres = socket.pollEvents(zt::PollEventBitmask::ReadyToReceiveAny);
        if (pollres.hasError()) {
            HG_THROW_TRACED(TracedRuntimeError, pollres.getError().errorCode, pollres.getError().message);
        }
        if ((*pollres & zt::PollEventBitmask::ReadyToReceiveAny) == 0) {
            // Not ready to receive
            return Status::NotReady;
        }

        zt::IpAddress senderIp;
        const auto res = socket.receiveFrom(_recvBuffer.data(), _recvBuffer.size(),
                                            senderIp, aRemotePort);
        if (res.hasError()) {
            HG_THROW_TRACED(TracedRuntimeError, res.getError().errorCode, res.getError().message);
        }
        if (*res == 0) {
            // Nothing received (not expected to ever happen but let's be safe)
            return Status::NotReady;
        }

        aPacket.clear();
        aPacket.appendBytes(_recvBuffer.data(), stopz(*res));

        aRemoteAddress = sf::IpAddress(senderIp.toString());

        return Status::OK;
    }
#endif
    else {
        HG_UNREACHABLE("Unsupported networking stack requested. "
                       "(Did you compile RigelNet with the correct configuration?)");
    }
}

void RN_SocketAdapter::close() {
    // Note: This method swallows all errors as we don't expect to use the socket afterwards

    if (UseSfSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<sf::UdpSocket>(_socket);
        socket.unbind();
    }
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    else if (UseZtSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<zt::Socket>(_socket);
        socket.close();
    }
#endif
    else {
        HG_UNREACHABLE("Unsupported networking stack requested. "
                       "(Did you compile RigelNet with the correct configuration?)");
    }
}

std::uint16_t RN_SocketAdapter::getLocalPort() const {
    if (UseSfSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<sf::UdpSocket>(_socket);
        return socket.getLocalPort();
    }
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    else if (UseZtSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<zt::Socket>(_socket);
        return socket.getLocalPort();
    }
#endif
    else {
        HG_UNREACHABLE("Unsupported networking stack requested. "
                       "(Did you compile RigelNet with the correct configuration?)");
    }
}

RN_Protocol RN_SocketAdapter::getProtocol() const noexcept {
    return _protocol;
}

RN_NetworkingStack RN_SocketAdapter::getNetworkingStack() const noexcept {
    return _networkingStack;
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>