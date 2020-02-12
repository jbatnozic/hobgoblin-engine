
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"
#include "Rigel-udp-connector.hpp"
#include "Rigel-udp-server.hpp"

#include <assert.h>
#include <algorithm>

#include <iostream> // Temp
void WinApiSleep(unsigned time_ms); // Temp

#define TAG_NO_ACK   false
#define TAG_ACK      true

#define TAG_NO_DATA  false
#define TAG_HAS_DATA true

static
bool upload_packet(sf::UdpSocket * socket, rn::Packet & pack, rn::IpAddress ip, rn::Port port) {

    if (pack.getDataSize() == 0u) return true;

    while (true) {

        switch ( socket->send(pack, ip, port) ) {

            case sf::Socket::Done:
                //std::cout << "up - Done\n";
                //pack.clear();
                return true;
                break;

            case sf::Socket::Partial:
                continue;
                break;

            case sf::Socket::NotReady:
                std::cout << "up - NotReady\n";
                //assert(0);
                return true;
                break;

            case sf::Socket::Error:
                std::cout << "up - Error\n";
                //assert(0);
                return false;
                break;

            case sf::Socket::Disconnected:
                std::cout << "up - Disconnected\n";
                assert(0);
                break;

            }

        }

    return false; // Won't ever happen (suppresses warnings)
    
    }

namespace RIGELNETW_CONFIG_NS_NAME {
    
    UDPConnector::UDPConnector(sf::UdpSocket * sock, const std::string & passphrase)
        : socket(sock)
        , passphrase(passphrase)
        , write_dummy_vec(1, nullptr) {

        reset();

        }

    void UDPConnector::reset(unsigned interval_, unsigned timeout_ms_) {

        interval = (interval_ > 1) ? (interval_) : (1);
        timeout_ms = timeout_ms_;

        remote_ip = sf::IpAddress::None;
        remote_port = 0u;

        state = Idle;

        waiting_for_ping_back = false;

        unsafe_send_packet.clear();
        unsafe_recv_buffer.clear();
        send_buffer.clear();
        recv_buffer.clear();
        ack_vector.clear();

        }

    void UDPConnector::disconnect(bool notify_remote) {
        
        if (notify_remote && state == Connected) {
            
            unsafe_send_packet.clear();

            unsafe_send_packet << Uint8(UDPMsgType::Disconnect);

            upload_packet(socket, unsafe_send_packet, remote_ip, remote_port);
            
            }

        //std::cout << "Disconnect\n"; // Temp

        remote_ip = sf::IpAddress::None;
        remote_port = 0u;

        state = Idle;

        waiting_for_ping_back = false;

        unsafe_send_packet.clear();
        unsafe_recv_buffer.clear();
        send_buffer.clear();
        recv_buffer.clear();
        ack_vector.clear();

        }

    void UDPConnector::accept(IpAddress addr, Port port) {
        
        assert(state == Idle);

        // std::cout << "UDPConnector accepting connection request...\n";

        remote_ip = addr;
        remote_port = port;
        state = Accepting;

        timeout_clock.restart();

        }

    void UDPConnector::connect(IpAddress ip, Port port, unsigned interval_, unsigned timeout_ms_) {

        reset(interval_, timeout_ms_);

        state = Connecting;

        remote_ip = ip;
        remote_port = port;

        timeout_clock.restart();

        }

    void UDPConnector::update(Node * node, size_t slot_index, bool skip_send) {

        switch (state) {
            
            case Connecting:
            // Send periodic HELLO messages to the "server", until a CONNECT message is received

                // STUB -- Check for timeout!
                if (connection_timed_out()) {
                    
                    reset(interval, timeout_ms);
                    node->queue_event( EventFactory::create_attempt_timeout(timeout_ms) );
                    return;

                    }

                if (!skip_send) {
                    
                    interval_cnt = (interval_cnt + 1) % (interval * 10);

                    if (interval_cnt == 0) {
                        
                        unsafe_send_packet.clear();

                        unsafe_send_packet << Uint8(UDPMsgType::Hello) << passphrase;

                        if (!upload_packet(socket, unsafe_send_packet, remote_ip, remote_port)) {
                            // STUB - Disconnect
                            return;
                            }

                        }

                    }
                break;

            case Accepting: 
            // Send periodic CONNECT messages to the "client", until a DATA message is received

                // STUB -- Check for timeout!
                if (connection_timed_out()) {

                    reset(interval, timeout_ms);
                    node->queue_event( EventFactory::create_conn_timeout(slot_index) );
                    return;

                    }

                if (!skip_send) {

                    interval_cnt = (interval_cnt + 1) % (interval * 10);

                    if (interval_cnt == 0) {

                        unsafe_send_packet.clear();

                        unsafe_send_packet << Uint8(UDPMsgType::Connect) << passphrase;

                        if (!upload_packet(socket, unsafe_send_packet, remote_ip, remote_port)) {
                            // STUB - Disconnect
                            return;
                            }

                        }

                    }
                break;

            case Connected:

                // STUB -- Check for timeout!
                if (connection_timed_out()) {

                    reset(interval, timeout_ms);
                    node->queue_event( EventFactory::create_conn_timeout(slot_index) );
                    return;

                    }

                if (!skip_send) {

                    interval_cnt = (interval_cnt + 1) % interval;

                    if (interval_cnt == 0) {

                        upload_all();

                        }

                    }
                break;

            case Idle:
            default:
                // Do nothing
                break;
            
            }

        }

    void UDPConnector::packet_recieved(Packet & pack, Node * node, NodeType::Enum type, size_t slot_index) {
        
        assert(state != Idle);

        Uint8 ptype;

        switch (state) {

            case Connecting:
                pack >> ptype;
                if (ptype == UDPMsgType::Connect) {
                    std::string temp;
                    pack >> temp;
                    if (temp == passphrase) {
                        // "Client" connected to "server"
                        initialize_session();
                        node->queue_event( EventFactory::create_connect() );
                        }
                    else {
                        disconnect(false);
                        node->queue_event( EventFactory::create_bad_passphrase(temp) );
                        }
                    }
                else {
                    disconnect(false);
                    node->queue_event( EventFactory::create_disconnect() );
                    }
                break;

            case Accepting:
            case Connected:
                pack >> ptype;
                if (ptype == UDPMsgType::Hello || ptype == UDPMsgType::Connect) {
                    // Ignore
                    }
                else if (ptype == UDPMsgType::Data) {

                    if (state == Accepting) {
                        initialize_session(); // New connection confirmed
                        node->queue_event( EventFactory::create_connect(slot_index) );
                        }

                    handle_data(pack, node, type);

                    }
                else if (ptype == UDPMsgType::UnsafeData) {

                    if (state == Accepting) {
                        initialize_session(); // New connection confirmed
                        node->queue_event( EventFactory::create_connect(slot_index) );
                        }

                    handle_unsafe_data(pack, node, type);

                    }
                else { // Otherwise, disconnect
                    disconnect(false);
                    node->queue_event( EventFactory::create_disconnect(slot_index) );
                    }
                break;

            case Idle:
            default:
                // Do nothing
                break;

            }

        }

    void UDPConnector::unpack_exec(Node * node, NodeType::Enum type) {
        
        // Safe:
        while (!recv_buffer.empty() && recv_buffer[0].tag == TAG_HAS_DATA) {
            
            if (!node->unpack_exec(recv_buffer[0].packet, type)) {
                // STUB -- Error
                }
            
            recv_buffer.pop_front();

            recv_buffer_head_index += 1u;

            }

        // Unsafe:
        for (auto & item : unsafe_recv_buffer) {
            if (!node->unpack_exec(item, type)) {
                // STUB -- Error
                }
            }
        unsafe_recv_buffer.clear();

        // Clean up send_buffer if any new acks were received:
        while (!send_buffer.empty() && send_buffer[0].tag == TAG_ACK) {
            
            send_buffer.pop_front();
            send_buffer_head_index += 1u;
            
            }

        if (send_buffer.empty()) {

            send_buffer.emplace_back();
            send_buffer.back().packet << Uint8(UDPMsgType::Data) << Uint32( send_buffer_head_index + send_buffer.size() - 1 );

            }

        }

    UDPConnector::StateEnum UDPConnector::get_state() const {
        
        return state;

        }

    IpAddress UDPConnector::get_remote_ip() const {
        
        return remote_ip;

        }

    Port UDPConnector::get_remote_port() const {
        
        return remote_port;

        }

    void UDPConnector::change_socket(sf::UdpSocket * sock) {
        
        socket = sock;

        }
    
    // Sending:
    void UDPConnector::write(HdlInd fn_ind) {
        
        if (state != Connected) return;

        unsafe_send_packet << (MsgType) Type::Int8;
        unsafe_send_packet << (HdlInd ) fn_ind;
        unsafe_send_packet << (ArgCnt ) 0;

        }

    void UDPConnector::write(HdlInd fn_ind, const Packet &pack) {
        
        assert(0);

        }

    // Sending, secure versions:
    void UDPConnector::write_s(HdlInd fn_ind) {
        
        //return;

        if (state != Connected) return;

        send_buffer.back().packet << (MsgType) Type::Int8;
        send_buffer.back().packet << (HdlInd ) fn_ind;
        send_buffer.back().packet << (ArgCnt ) 0;

        }

    void UDPConnector::write_s(HdlInd fn_ind, const Packet &pack) {
        
        //return;

        assert(0);

        }

    // Private: ///////////////////////////////////////////////////////////////

    void UDPConnector::upload_all() {

        // SAFE:

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

        ///////////////////////////////////////////////////////////////////////

        // UNSAFE:

        if (!upload_packet(socket, unsafe_send_packet, remote_ip, remote_port)) {
            // STUB - Disconnect
            return;
            }
        unsafe_send_packet.clear();

        unsafe_send_packet << Uint8(UDPMsgType::UnsafeData) << Uint32(unsafe_send_ord);

        for (auto & i : ack_vector) unsafe_send_packet << Uint32(i);
        ack_vector.clear();
        unsafe_send_packet << Uint32(-1);

        unsafe_send_ord += 1u;

        }

    void UDPConnector::prepare_ack(Uint32 ordinal) {

        ack_vector.push_back(ordinal);

        }

    void UDPConnector::ack_received(Uint32 ordinal) {

        if (ordinal < send_buffer_head_index) return; // Already acknowledged before

        size_t ind = (ordinal - send_buffer_head_index);

        if (ind >= send_buffer.size()) { /* STUB -- ERROR*/ }

        send_buffer[ind].tag = TAG_ACK;
        send_buffer[ind].packet.clear();

        }

    void UDPConnector::handle_data(Packet & pack, Node * node, NodeType::Enum type) {
        
        Uint32 ordinal;
        pack >> ordinal;

        if (ordinal < recv_buffer_head_index) {
            // Old data - acknowledge and ignore
            // std::cout << "Got duplicate data\n";
            goto ACKNOWLEDGE;
            }

        size_t ind = (ordinal - recv_buffer_head_index);

        // STUB -- Maximum index!!! (~100?)
        /*if (recv_buffer.size() > 1000) {
            std::cout << "STUCK!!! recv_buffer_head_index = " << recv_buffer_head_index << "\n";
            WinApiSleep(100000u);
            }*/

        if (ind >= recv_buffer.size()) recv_buffer.resize(ind + 1u);

        if (recv_buffer[ind].tag == TAG_NO_DATA) {

            recv_buffer[ind].packet = std::move(pack);
            recv_buffer[ind].tag    = TAG_HAS_DATA;

            }

        ACKNOWLEDGE:

        prepare_ack(ordinal);

        pack.clear();

        }

    void UDPConnector::handle_unsafe_data(Packet & pack, Node * node, NodeType::Enum type) {
        
        Uint32 ordinal;
        pack >> ordinal;

        if (ordinal <= unsafe_recv_ord) { // Old data, ignore it...
            //std::cout << "Got duplicate UNSAFE data\n";
            pack.clear();
            return;
            }

        // Get acks:
        while (true) {

            Uint32 ack;
            if (!(pack >> ack)) {
                // Error -- STUB
                }
            
            if (ack != Uint32(-1)) 
                ack_received(ack);
            else
                break;

            }

        // Get data:
        unsafe_recv_ord = ordinal;
        unsafe_recv_buffer.emplace_back( std::move(pack) );
        //node->unpack_exec(pack, type);
        //pack.clear();

        }

    void UDPConnector::initialize_session() {

        state = Connected;

        timeout_clock.restart();

        // UNSAFE: //
        unsafe_recv_ord = 0u;
        unsafe_send_ord = 1u;
        
        unsafe_send_packet.clear();

        unsafe_send_packet << Uint8(UDPMsgType::UnsafeData) << Uint32(unsafe_send_ord);

        ack_vector.clear();

        unsafe_send_packet << Uint32(-1);

        unsafe_send_ord += 1u;

        // SAFE: //
        send_buffer_head_index = 1u;
        recv_buffer_head_index = 1u;

        recv_buffer.clear();
        send_buffer.clear();

        send_buffer.emplace_back();
        send_buffer.back().packet << Uint8(UDPMsgType::Data) << Uint32( send_buffer_head_index + send_buffer.size() - 1 );

        }

    bool UDPConnector::connection_timed_out() {
        
        return (Int32(timeout_ms) != 0 && timeout_clock.getElapsedTime() >= sf::milliseconds(timeout_ms));

        }

    }

#undef TAG_NO_DATA
#undef TAG_HAS_DATA
#undef TAG_NO_ACK
#undef TAG_ACK