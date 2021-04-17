
#include "Socket_adapter.hpp"

#include <Hobgoblin/Utility/Exceptions.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
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
        ZTCPP_THROW_ON_ERROR(res, util::TracedException);

        _recvBuffer.resize(pztos(aRecvBufferSize));
    }
#endif
}

void RN_SocketAdapter::bind(sf::IpAddress aIpAddress, std::uint16_t aLocalPort) {
    if (UseSfSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<sf::UdpSocket>(_socket);
        if (socket.bind(aLocalPort, aIpAddress) != sf::Socket::Done) {
            throw util::TracedRuntimeError("Could not bind port");
        }
    }
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    else if (UseZtSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<zt::Socket>(_socket);
        const auto res = socket.bind(zt::IpAddress::ipv4FromString(aIpAddress.toString()), aLocalPort);
        ZTCPP_THROW_ON_ERROR(res, util::TracedException);
    }
#endif
}

bool RN_SocketAdapter::send(util::Packet& aPacket,
                            const sf::IpAddress& aTargetAddress,
                            std::uint16_t aTargetPort) {
    if (aPacket.getDataSize() == 0u) return true;

    if (UseSfSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<sf::UdpSocket>(_socket);
    RETRY:
        switch (socket.send(aPacket, aTargetAddress, aTargetPort)) {
        case sf::Socket::Done:
            return true;

        case sf::Socket::Partial:
            goto RETRY;

        case sf::Socket::NotReady:
            return true; // TODO ?????

        case sf::Socket::Error:
        case sf::Socket::Disconnected:
            return false;
        }

        assert(false && "Unreachable");
        return false;
    }
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    else if (UseZtSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<zt::Socket>(_socket);
        const auto res = socket.sendTo(aPacket.getData(), aPacket.getDataSize(),
                                       zt::IpAddress::ipv4FromString(aTargetAddress.toString()), aTargetPort);
        ZTCPP_THROW_ON_ERROR(res, util::TracedException);
        return true;
    }
#endif
}

bool RN_SocketAdapter::recv(util::Packet& aPacket, 
                            sf::IpAddress& aRemoteAddress, 
                            std::uint16_t& aRemotePort) {
    if (UseSfSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<sf::UdpSocket>(_socket);
        const auto res = socket.receive(aPacket, aRemoteAddress, aRemotePort);
        return (res == sf::Socket::Done);
    }
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    else if (UseZtSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<zt::Socket>(_socket);

        const auto pollres = socket.pollEvents(zt::PollEventBitmask::ReadyToReceiveAny);
        ZTCPP_THROW_ON_ERROR(pollres, util::TracedException);
        if ((*pollres & zt::PollEventBitmask::ReadyToReceiveAny) == 0) {
            // Not ready to receive
            return false;
        }

        zt::IpAddress senderIp;
        const auto res = socket.receiveFrom(_recvBuffer.data(), _recvBuffer.size(),
                                            senderIp, aRemotePort);
        ZTCPP_THROW_ON_ERROR(res, util::TracedException);
        aPacket.clear();
        aPacket.append(_recvBuffer.data(), *res);

        aRemoteAddress = sf::IpAddress(senderIp.toString());
        return true;
    }
#endif
    return false;
}

void RN_SocketAdapter::close() {
    if (UseSfSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<sf::UdpSocket>(_socket);
        socket.unbind();
    }
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    else if (UseZtSocket(_protocol, _networkingStack)) {
        auto& socket = std::get<zt::Socket>(_socket);
        const auto res = socket.close();
        ZTCPP_THROW_ON_ERROR(res, util::TracedException);
    }
#endif
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
    return 0;
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