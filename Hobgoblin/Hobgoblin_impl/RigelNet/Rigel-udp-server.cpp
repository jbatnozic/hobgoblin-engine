
#include "Rigel-SFML-networking.hpp"
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include "Rigel-udp-server.hpp"
#include "Rigel-node.hpp"
#include "Rigel-client-slot.hpp"
#include "Rigel-registry.hpp"
#include "Rigel-argwrap.hpp"
#include "Rigel-udp-connector.hpp"

#include <string>
#include <vector>
#include <memory>
#include <initializer_list>

#include <iostream> // Temp

namespace RIGELNETW_CONFIG_NS_NAME {
    
    UDPServer::UDPServer(size_t size_, const std::string & passphrase_)
        : running(false)
        , size(size_)
        , client_vec(size_)
        , packet_vec(size_)
        , passphrase(passphrase_) {

            socket.setBlocking(0);

            //Initialize client slots
            for (size_t i = 0; i < size; i += 1) {

                client_vec[i] = std::make_unique<UDPConnector>(&socket, passphrase_);

                }
        
        }

    UDPServer::~UDPServer() {
        
        // STUB

        }

    bool UDPServer::start(Port port_, unsigned interval_, unsigned timeout_ms_) {

        if (running) throw std::logic_error("rn::TUDPServer::start - Server already running!");

        if (socket.bind(port_) != sf::Socket::Status::Done) return false;

        // If connected successfully:
        port = socket.getLocalPort();
        interval = (interval_ > 1) ? (interval_) : (1);
        interval_cnt = 1;
        timeout_limit = timeout_ms_;
        update_ordinal = 1u;
        running = true;

        reset_connectors(interval, timeout_limit);

        return true;

        }

    void UDPServer::update(bool skip_send) {
        
        if (!running) return;

        // [EVENTS] Clear event queue (must be first)
        events.clear();

        // [UPLOAD] Send prepared data to connected clients
        for (size_t i = 0; i < size; i += 1) {

            if (get_client_status(i) == true) {	

                if (!client_vec[i]->waiting_for_ping_back) {
                    client_ping(i);
                    }

                client_send_uord(i);

                }

            client_vec[i]->update(this, skip_send);

            }

        // [DOWNLOAD] Receive connection requests and data from clients
        sf::Packet rec_packet;
        sf::IpAddress rec_ip;
        Port rec_port;

        while (true) {

            if (socket.receive(rec_packet, rec_ip, rec_port) == sf::Socket::Status::Done) {

                int ind = find_connector(rec_ip, rec_port);

                if (ind != -1) {
                    client_vec[ind]->packet_recieved(rec_packet, this, NodeType::Server_UDP, size_t(ind));
                    }
                else {
                    handle_unknown_packet(rec_packet, rec_ip, rec_port);
                    }

                rec_packet.clear();

                }
            else {
                break;
                }

            }

        for (size_t i = 0; i < size; i += 1) {
            sender_index = i;
            client_vec[i]->unpack_exec(this, NodeType::Server_UDP);
            }

        }

    // Sending:
    void UDPServer::write(size_t receiver, HdlInd fn_ind) {
        
        if (receiver == ALL_CLIENTS) {

            for (size_t i = 0; i < size; i += 1) {

                client_vec[i]->write(fn_ind);

                }

            }
        else if (get_client_status(receiver) == true) {

            client_vec[receiver]->write(fn_ind);

            }

        }

    void UDPServer::write(size_t receiver, HdlInd fn_ind, const Packet &pack) {
        
        if (receiver == ALL_CLIENTS) {

            for (size_t i = 0; i < size; i += 1) {

                client_vec[i]->write(fn_ind, pack);

                }

            }
        else if (get_client_status(receiver) == true) {

            client_vec[receiver]->write(fn_ind, pack);

            }

        }

    void UDPServer::write_s(size_t receiver, HdlInd fn_ind) {
        
        if (receiver == ALL_CLIENTS) {

            for (size_t i = 0; i < size; i += 1) {

                client_vec[i]->write_s(fn_ind);

                }

            }
        else if (get_client_status(receiver) == true) {

            client_vec[receiver]->write_s(fn_ind);

            }

        }

    void UDPServer::write_s(size_t receiver, HdlInd fn_ind, const Packet &pack) {
        
        if (receiver == ALL_CLIENTS) {

            for (size_t i = 0; i < size; i += 1) {

                client_vec[i]->write_s(fn_ind, pack);

                }

            }
        else if (get_client_status(receiver) == true) {

            client_vec[receiver]->write_s(fn_ind, pack);

            }

        }

    size_t UDPServer::get_sender_index() const {
        
        return sender_index;

        }
    
    bool UDPServer::get_client_status(size_t n) const {
        
        if (n >= size) throw std::out_of_range("rn::UDPServer::get_client_status - Index out of bounds.");

        return ((client_vec[n]->get_state()) == UDPConnector::Connected);

        }

    int  UDPServer::get_client_latency(size_t n) const {
        
        if (get_client_status(n) != true) return -1;

        return (client_vec[n]->latency).asMilliseconds();

        }

    IpAddress UDPServer::get_client_ip(size_t n) const {
        
        if (get_client_status(n) != true) return sf::IpAddress::None;

        return (client_vec[n]->get_remote_ip());

        }

    //Registry:
    #define RegP RegPermission

    void UDPServer::reg_set_int(const std::string& key, Int32  val) {

        my_reg.set_int(key, val);

        write_s<Int32>(ALL_CLIENTS, HandlerMgr::hv_reg_set_int, {key, val});

        }

    void UDPServer::reg_set_dbl(const std::string& key, Double val) {

        my_reg.set_dbl(key, val);

        write_s<Double>(ALL_CLIENTS, HandlerMgr::hv_reg_set_dbl, {key, val});

        }

    void UDPServer::reg_set_str(const std::string& key, const std::string& val) {

        my_reg.set_str(key, val);

        write_s<Int8>(ALL_CLIENTS, HandlerMgr::hv_reg_set_str, {key, val});

        }

    void UDPServer::reg_del_int(const std::string& key) {

        my_reg.del_int(key);

        write_s<Int8>(ALL_CLIENTS, HandlerMgr::hv_reg_del_int, {key});

        }

    void UDPServer::reg_del_dbl(const std::string& key) {

        my_reg.del_int(key);

        write_s<Int8>(ALL_CLIENTS, HandlerMgr::hv_reg_del_dbl, {key});

        }

    void UDPServer::reg_del_str(const std::string& key) {

        my_reg.del_int(key);

        write_s<Int8>(ALL_CLIENTS, HandlerMgr::hv_reg_del_str, {key});

        }

    void UDPServer::reg_clear_int() {

        my_reg.clear_int();

        write_s(ALL_CLIENTS, HandlerMgr::hv_reg_clear_int);

        }

    void UDPServer::reg_clear_dbl() {

        my_reg.clear_dbl();

        write_s(ALL_CLIENTS, HandlerMgr::hv_reg_clear_dbl);

        }

    void UDPServer::reg_clear_str() {

        my_reg.clear_str();

        write_s(ALL_CLIENTS, HandlerMgr::hv_reg_clear_str);

        }

    void UDPServer::reg_clear_all() {

        my_reg.clear_all();

        write_s(ALL_CLIENTS, HandlerMgr::hv_reg_clear_all);

        }

    // TODO
    void UDPServer::reg_set_cl_perm(const std::string& key, size_t n, RegP permission) {

        /*if (get_client_status(n) != ClientSlot::Working) return;

        if (permission == REG_RDONLY) {

            (*(client_vec[n]->whitelist)).erase(key);

            }
        else {

            (*(client_vec[n]->whitelist))[key] = permission;

            }*/

        }

    // TODO
    RegP UDPServer::reg_get_cl_perm(const std::string& key, size_t n) {

        /*if (get_client_status(n) != ClientSlot::Working) return REG_ERROR;

        auto iter = (client_vec[n]->whitelist)->find(key);

        if (iter == (client_vec[n]->whitelist)->end()) {

            return REG_RDONLY;

            }
        else {

            return (*iter).second;

            }*/

        return RegP(0); // Temp

        }

    #undef RegP

    ///////////////////////////////////////////////////////////////////////////

    void UDPServer::client_ping(size_t n, int stage) {

        if (get_client_status(n) != true) return;

        switch (stage) {

            case 0: //Send ping to client
                (client_vec[n]->waiting_for_ping_back) = true;
                (client_vec[n]->ping_clock).restart();
                write_s<Int8>(n, HandlerMgr::hv_ping, {(Int8)0});
                break;

            case 1: //Got pinged back
                (client_vec[n]->latency) = (client_vec[n]->ping_clock).getElapsedTime();
                (client_vec[n]->waiting_for_ping_back)=false;
                break;

            default:
                break;

            }

        }

    void UDPServer::reset_connectors(unsigned interval_, unsigned timeout_ms_) {

        for (size_t i = 0; i < size; i += 1) {

            client_vec[i]->reset(interval_, timeout_ms_);

            }

        }

    int UDPServer::find_connector(IpAddress addr, Port port) const {
        
        for (size_t i = 0; i < size; i += 1) {
            
            if (client_vec[i]->get_remote_ip() == addr &&
                client_vec[i]->get_remote_port() == port)
                return int(i);
            
            }

        return -1;

        }

    void UDPServer::handle_unknown_packet(Packet & pack, IpAddress addr, Port port) {
        
        Uint8 ptype;
        pack >> ptype;

        std::cout << "Unknown packet...\n";

        if (ptype == UDPMsgType::Hello) {
            
            std::string temp;
            pack >> temp;

            std::cout << "Hello there :) [" << temp << "]\n";

            if (temp != passphrase) return;

            for (size_t i = 0; i < size; i += 1) {
                
                if (client_vec[i]->get_state() == UDPConnector::Idle) {
                    client_vec[i]->accept(addr, port);
                    queue_event( EventFactory::create_connect(i) );
                    break;
                    }
                
                }

            // No room - send disconnect message:
            // STUB
            
            }
        else {
            // Not a hello message - Ignore...
            }
        
        }

    void UDPServer::client_send_uord(size_t n) {
        
        write_s<Uint32>(n, HandlerMgr::hv_set_update_number, {1}); // STUB

        }

    }