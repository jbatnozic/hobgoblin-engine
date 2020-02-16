
#include <Hobgoblin_include/RigelNet/udp_connector.hpp>

#include <cassert>
#include <iostream> // TODO Temp.

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {
namespace detail {

namespace {

constexpr std::uint32_t UDP_MESSAGE_TYPE_HELLO      = 0x3BF0E110;
constexpr std::uint32_t UDP_MESSAGE_TYPE_CONNECT    = 0x83C96CA4;
constexpr std::uint32_t UDP_MESSAGE_TYPE_DISCONNECT = 0xD0F235AB;
constexpr std::uint32_t UDP_MESSAGE_TYPE_DATA       = 0xA765B8F6;

constexpr bool UPLOAD_PACKET_SUCCESS = true;
constexpr bool UPLOAD_PACKET_FAILURE = false;

bool UploadPacket(sf::UdpSocket& socket, RN_Packet& packet, sf::IpAddress ip, std::uint16_t port) {
    if (packet.getDataSize() == 0u) return UPLOAD_PACKET_SUCCESS;

    while (true) {
        switch (socket.send(packet, ip, port)) {

        case sf::Socket::Done:
            //std::cout << "up - Done\n";
            return UPLOAD_PACKET_SUCCESS;
            break;

        case sf::Socket::Partial:
            continue;

        case sf::Socket::NotReady:
            //std::cout << "up - NotReady\n";
            //assert(0);
            return UPLOAD_PACKET_SUCCESS;
            break;

        case sf::Socket::Error:
            //std::cout << "up - Error\n";
            //assert(0);
            return UPLOAD_PACKET_FAILURE;
            break;

        case sf::Socket::Disconnected:
            //std::cout << "up - Disconnected\n";
            assert(0);
            break;

        }
    }

    assert(0);
    return UPLOAD_PACKET_FAILURE;
}

} // namespace

RN_UdpConnector::RN_UdpConnector(sf::UdpSocket& socket, const std::string& passphrase)
    : _socket{socket}
    , _passphrase{passphrase}
    , _state{State::Disconnected}
{
}

void RN_UdpConnector::tryAccept(sf::IpAddress addr, std::uint16_t port, RN_Packet& packet) {
    assert(_state == State::Disconnected);

    std::uint32_t msgType;
    std::string receivedPassphrase;

    if ((packet >> msgType)
        && msgType == UDP_MESSAGE_TYPE_HELLO
        && (packet >> receivedPassphrase)
        && receivedPassphrase == _passphrase) {

        _remoteInfo.ipAddress = addr;
        _remoteInfo.port = port;
        _remoteInfo.timeoutClock.restart();

        _state = State::Accepting;

    }
    else {
        // TODO - Notify of erroneous message from unknown sender...
    }
}

void RN_UdpConnector::connect(sf::IpAddress addr, std::uint16_t port) {
    assert(_state == State::Disconnected);

    // reset(interval_, timeout_ms_); TODO ???

    _remoteInfo.ipAddress = addr;
    _remoteInfo.port = port;
    _remoteInfo.timeoutClock.restart();

    _state = State::Connecting;
}

void RN_UdpConnector::reset(bool) {
    // TODO
}

void RN_UdpConnector::update(RN_Node& node, PZInteger slotIndex, bool doUpload) {
    switch (_state) {
    case State::Accepting:
        execAccepting(node, slotIndex, doUpload);
        break;

    case State::Connecting:
        execConnecting(node, slotIndex, doUpload);
        break;

    case State::Connected:
        execConnected(node, slotIndex, doUpload);
        break;

    case State::Disconnected:
        // Do nothing
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnector::receivedPacket(RN_Packet& packet) {
    assert(_state != State::Disconnected);

    const auto msgType = packet.extractValue<std::uint32_t>();
    if (!packet) {
        // TODO - Handle error
    }

    switch (_state) {
    case State::Connecting:
        if (msgType == UDP_MESSAGE_TYPE_CONNECT) {
            const std::string receivedPassphrase = packet.extractValue<std::string>();
            if (!packet) {
                // TODO - Handle error
            }

            if (receivedPassphrase == _passphrase) {
                // Client connected to server
                initializeSession();
                // node->queue_event(EventFactory::create_connect()); TODO
            }
            else {
                // disconnect(false); TODO
                // node->queue_event(EventFactory::create_bad_passphrase(temp)); TODO
            }
        }
        else {
            // disconnect(false); TODO
            // node->queue_event(EventFactory::create_disconnect()); TODO
        }
        break;

    case State::Accepting:
    case State::Connected:
        if (msgType == UDP_MESSAGE_TYPE_HELLO || msgType == UDP_MESSAGE_TYPE_CONNECT) {
            // Ignore
        }
        else if (msgType == UDP_MESSAGE_TYPE_DATA) {
            if (_state == State::Accepting) {
                initializeSession(); // New connection confirmed
                // node->queue_event(EventFactory::create_connect(slot_index)); TODO
            }
            // handle_data(pack, node, type); TODO
        }
        else { // Otherwise, disconnect
            // TODO - Handle error
            // disconnect(false);
            // node->queue_event(EventFactory::create_disconnect(slot_index));
        }
        break;

    default:
        assert(0 && "Unreachable");
        break;

    }
}

const RN_RemoteInfo& RN_UdpConnector::getRemoteInfo() const noexcept {
    return _remoteInfo;
}

// Private

void RN_UdpConnector::execAccepting(RN_Node& node, PZInteger slotIndex, bool doUpload) {
    // Send periodic CONNECT messages to the "client", until a DATA message is received

    if (connectionTimedOut()) {
        //reset();
        //node->queue_event(EventFactory::create_conn_timeout(slot_index));
        return;
    }

    if (doUpload) {
        RN_Packet packet;
        packet << UDP_MESSAGE_TYPE_CONNECT << _passphrase;
        if (UploadPacket(_socket, packet, _remoteInfo.ipAddress, _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {
            // TODO Disconnect
        }
    }
}

void RN_UdpConnector::execConnecting(RN_Node& node, PZInteger slotIndex, bool doUpload) {
    // Send periodic HELLO messages to the server, until a CONNECT message is received

    if (connectionTimedOut()) {
        //reset(interval, timeout_ms);
        //node->queue_event(EventFactory::create_attempt_timeout(timeout_ms));
        return;
    }

    if (doUpload) {
        RN_Packet packet;
        packet << UDP_MESSAGE_TYPE_HELLO << _passphrase;
        if (UploadPacket(_socket, packet, _remoteInfo.ipAddress, _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {
            // TODO Disconnect
        }
    }
}

void RN_UdpConnector::execConnected(RN_Node& node, PZInteger slotIndex, bool doUpload) {
    if (connectionTimedOut()) {
        // reset(interval, timeout_ms);
        // node->queue_event(EventFactory::create_conn_timeout(slot_index));
        return;
    }

    if (doUpload) {
        uploadAllData();
    }
}

bool RN_UdpConnector::connectionTimedOut() const {
    // TODO Fix once hg::Clock is implemented
    const auto microsec = _remoteInfo.timeoutClock.getElapsedTime().asMicroseconds();
    if (_timeoutLimit <= std::chrono::microseconds{0} || microsec <= 0) {
        return false;
    }
    if (microsec >= _timeoutLimit.count()) {
        return true;
    }
    return false;
}

void RN_UdpConnector::uploadAllData() {
    /*
    size_t send_cnt = 0;
        for (size_t i = 0; i < send_buffer.size(); i += 1) {
            
            auto & item = send_buffer[i];

            if (item.tag == TAG_ACK) continue;

            if (i < send_buffer.size() - 1)  {
                
                if (item.clock.getElapsedTime().asMilliseconds() > std::min(latency.asMilliseconds() * 2, 400)) { // STUB -- Could be better
                    // min{2 * latency, 4 * frame_duration * interval}
                    //std::cout << "Retransmit after " << item.clock.getElapsedTime().asMilliseconds() << "ms \n";
                    }
                else
                    continue; // Too early

                }

            if (!upload_packet(socket, item.packet, remote_ip, remote_port)) {
                // STUB -- Disconnect
                return;
                }

            item.clock.restart();

            if ((send_cnt += 1) >= 16) break;
            
            } // End_for

        send_buffer.emplace_back();
        send_buffer.back().packet << Uint8(UDPMsgType::Data) << Uint32( send_buffer_head_index + send_buffer.size() - 1 );
    */
    PZInteger uploadCounter = 0;
    for (auto& taggedPacket : _sendBuffer) {
        if (taggedPacket.acknowledged) {
            continue;
        }
    }
}

void RN_UdpConnector::prepareAck(std::uint32_t ordinal) {

}

void RN_UdpConnector::receivedAck(std::uint32_t ordinal) {
    if (ordinal < _sendBufferHeadIndex) {
        return; // Already acknowledged before
    }

    const auto ind = (ordinal - _sendBufferHeadIndex);
    if (ind >= _sendBuffer.size()) {
        // TODO -- Error
    }

    //send_buffer[ind].tag = TAG_ACK;
    //send_buffer[ind].packet.clear();
}

void RN_UdpConnector::initializeSession() {
    std::cout << "Session Initialized\n";
    _state = State::Connected;
    _remoteInfo.timeoutClock.restart();
    // TODO Something for message buffers
}

} // namespace detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>