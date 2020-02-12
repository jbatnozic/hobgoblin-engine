
#include "Rigel-SFML-networking.hpp"
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include "Rigel-udp-client.hpp"
#include "Rigel-node.hpp"
#include "Rigel-registry.hpp"
#include "Rigel-argwrap.hpp"
#include "Rigel-udp-connector.hpp"

#include <string>
#include <vector>
#include <memory>
#include <initializer_list>

#include <assert.h>

#include <iostream> // Temp

namespace RIGELNETW_CONFIG_NS_NAME {
    
    UDPClient::UDPClient(const std::string & passphrase)
        : socket()
        , conn(&socket, passphrase) {
        
        socket.setBlocking(0);

        }

    UDPClient::UDPClient(const std::string & passphrase, IpAddress ip, Port port, unsigned interval, unsigned timeout_ms)
        : socket()
        , conn(&socket, passphrase) {

        socket.setBlocking(0);

        connect(ip, port, interval, timeout_ms);
        
        }

    UDPClient::~UDPClient() {
        
        conn.disconnect(true);

        }

    // Sending:
    void UDPClient::write(HdlInd fn_ind) {
        
        conn.write(fn_ind);

        }

    void UDPClient::write(HdlInd fn_ind, const Packet &pack) {
        
        conn.write(fn_ind, pack);

        }

    void UDPClient::write_s(HdlInd fn_ind) {
        
        conn.write_s(fn_ind);

        }

    void UDPClient::write_s(HdlInd fn_ind, const Packet &pack) {
        
        conn.write_s(fn_ind, pack);

        }

    // Client management:
    bool UDPClient::is_ready() const {
        
        return (conn.get_state() == UDPConnector::Connected);
        
        }

    bool UDPClient::is_connecting() const {

        return (conn.get_state() == UDPConnector::Connecting);

        }

    void UDPClient::server_ping(int stage) {

        switch (stage) {

            case 0: //Send ping to ServerTCP
                conn.waiting_for_ping_back = true;
                conn.ping_clock.restart();
                write_s<Int8>(HandlerMgr::hv_ping, {(Int8)0});
                break;

            case 1: //Got pinged back
                conn.latency = conn.ping_clock.getElapsedTime();
                conn.waiting_for_ping_back = false;
                break;

            default:
                break;

            }

        }

    void UDPClient::connect(IpAddress ip, Port port, unsigned interval, unsigned timeout_ms) {
        
        if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) assert(0);

        conn.connect(ip, port, interval, timeout_ms);

        }

    void UDPClient::update(bool skip_send) {
        
        if (conn.get_state() == UDPConnector::Idle) return;

        // [EVENTS] Clear event queue (must be first)
        events.clear();

        // [UPLOAD] Ping / upload...
        if (conn.get_state() == UDPConnector::Connected) {
            
            if (!conn.waiting_for_ping_back)
                server_ping();

            server_send_uord();

            }

        conn.update(this, skip_send);

        // [DOWNLOAD] Receive and data from the server
        sf::Packet rec_packet;
        sf::IpAddress rec_ip;
        Port rec_port;

        while (true) {

            if (socket.receive(rec_packet, rec_ip, rec_port) == sf::Socket::Status::Done) {

                if (rec_ip != conn.get_remote_ip() || rec_port != conn.get_remote_port()) continue;

                conn.packet_recieved(rec_packet, this, NodeType::Client_UDP, size_t(-1));

                rec_packet.clear();

                }
            else {
                break;
                }

            }

        conn.unpack_exec(this, NodeType::Client_UDP);

        }

    void UDPClient::disconnect() {
        
        }
    
    int UDPClient::get_server_latency() const {
        
        if (conn.get_state() != UDPConnector::Connected) return -1;

        return conn.latency.asMilliseconds();

        }

    // Registry:
    void UDPClient::reg_req_set_int(const std::string& key, Int32  val) {

        write_s<Int32>(HandlerMgr::hv_req_reg_set_int, {key, val});

        }

    void UDPClient::reg_req_set_dbl(const std::string& key, Double val) {

        write_s<Double>(HandlerMgr::hv_req_reg_set_dbl, {key, val});

        }

    void UDPClient::reg_req_set_str(const std::string& key, const std::string& val) {

        write_s<Int8>(HandlerMgr::hv_req_reg_set_str, {key, val});

        }

    void UDPClient::reg_req_del_int(const std::string& key) {

        write_s<Int8>(HandlerMgr::hv_req_reg_del_int, {key});

        }

    void UDPClient::reg_req_del_dbl(const std::string& key) {

        write_s<Int8>(HandlerMgr::hv_req_reg_del_dbl, {key});

        }

    void UDPClient::reg_req_del_str(const std::string& key) {

        write_s<Int8>(HandlerMgr::hv_req_reg_del_str, {key});

        }

    // Private:
    void UDPClient::server_send_uord() {
        
        write<Uint32>(HandlerMgr::hv_set_update_number, {1u}); // STUB

        }

    }