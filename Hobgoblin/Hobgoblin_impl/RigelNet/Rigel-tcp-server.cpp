
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"
#include "Rigel-tcp-server.hpp"
#include "Rigel-registry.hpp"

#include <initializer_list>
#include <stdexcept>
#include <iostream>

namespace RIGELNETW_CONFIG_NS_NAME {

    #define MAX(x, y) ((x>=y)?(x):(y))

    TCPServer::TCPServer(size_t size_, const std::string & passphrase_)
        : running(false)
        , size(size_)
        , client_vec(size_)
        , packet_vec(size_)
        , passphrase(passphrase_) {
			
        listener.setBlocking(0);

        //Initialize client slots
        for (size_t i = 0; i < size; i += 1) {

            client_vec[i] = std::make_unique<ClientSlot>();

            }

        }

    TCPServer::TCPServer(size_t size_, const std::string & passphrase_, Port port, unsigned interval,
              unsigned timeout_ms)
        : TCPServer(size_, passphrase_) {
        
        start(port, interval, timeout_ms);

        }

    TCPServer::~TCPServer() {

        stop();

        }

    // Server management:
    bool TCPServer::start(Port port_, unsigned interval_, unsigned timeout_ms_) {

        if (running) throw std::logic_error("rn::TcpServer::start - Server already running!");

	    if (listener.listen(port_) != sf::Socket::Status::Done) return false;

        // If connected successfully:
        port = listener.getLocalPort();
        interval = MAX(interval_, 1);
        interval_cnt = 1;
        timeout_limit = timeout_ms_;
        update_ordinal = 1u;
        running = true;

        #ifdef RIGELNETW_CONFIG_SEND_STATS
        temp_update_cnt = 0;
        temp_byte_cnt = 0;
        temp_send_cnt = 0;
        temp_clock.restart();
        #endif

        reset_slots();

        return true;

	    }

    void TCPServer::stop() {
	
        if (!running) return;

        listener.close();

	    reset_slots();

        running = false;

	    }

    bool TCPServer::is_running() const {
        
        return running;

        }

    void TCPServer::update(bool skip_send) {

        if (!running) return;

        #ifdef RIGELNETW_CONFIG_SEND_STATS
        temp_update_cnt += 1;
        #endif

        // [EVENTS] Clear event queue (must be first)
        events.clear();

        // [TIMEOUT] Kick timed-out clients
        if (timeout_limit == 0) goto NO_TIMEOUTS;

        for (size_t i = 0; i < size; i += 1) {
            
            if ((client_vec[i]->status) == ClientSlot::Working) {

                if (get_client_last_contact(i) >= (int)timeout_limit) {
                    
                    queue_event( EventFactory::create_conn_timeout(i) );
                    client_vec[i]->reset();

                    }

                }

            }

        NO_TIMEOUTS:

        // [LISTEN] Accept incoming connection requests, if possible
        for (size_t i = 0; i < size; i += 1) {

            if ((client_vec[i]->status == ClientSlot::Free) &&
                (listener.accept(client_vec[i]->socket) == sf::Socket::Done)) {

                client_vec[i]->status = ClientSlot::Working;

                client_vec[i]->prod_clock.restart();

                #ifndef RIGELNETW_CONFIG_NO_PASSPHRASE
                client_vec[i]->packet << passphrase; // Send RAW passphrase
                #endif

                client_send_index(i);

                client_upload_reg(i);

                queue_event( EventFactory::create_connect(i) );          

                }

            }

        // [PING] Ping connected clients:
        for (size_t i = 0; i < size; i += 1) {

            if (client_vec[i]->status == ClientSlot::Working) {	

                if (!client_vec[i]->waiting_for_ping_back) {
                    client_ping(i);
                    }

                }

            }
        
        // [UPLOAD] Send prepared data to connected clients, check for DC:
        if (!skip_send) {

            interval_cnt = (interval_cnt + 1) % interval;

            if (interval_cnt == 0) {

                // ***
                for (size_t i = 0; i < size; i += 1) {

                    if (client_vec[i]->status == ClientSlot::Working) {	

                        client_send_uord(i);

                        // Send packet:
                        bool status = client_upload_packet(i);

                        // Client disconnected:
                        if (!status) {
                            
                            client_vec[i]->reset();

                            queue_event( EventFactory::create_disconnect(i) );

                            }

                        }

                    }
                // ***

                update_ordinal += 1;

                }

            }

        // [DOWNLOAD] Receive data from clients
        for (size_t i = 0; i < size; i += 1) {

            sf::Packet rec_packet;

            if (client_vec[i]->status == ClientSlot::Working) {	

                if ( (client_vec[i]->socket).receive(rec_packet) == sf::Socket::Done ) {

                    #ifndef RIGELNETW_CONFIG_NO_PASSPHRASE
                    if (client_vec[i]->waiting_for_passphrase && rec_packet.getDataSize() > 0) {
                        
                        std::string temp;

                        rec_packet >> temp;

                        //std::cout << "Packet size = " << rec_packet.getDataSize() << " (expecting passphrase)\n";

                        if (temp != passphrase) {
                            
                            queue_event( EventFactory::create_bad_passphrase(temp, i) );
                            client_vec[i]->reset();
                            rec_packet.clear();
                            continue;

                            }
                        else {
                            
                            client_vec[i]->waiting_for_passphrase = false;

                            }

                        }
                    #endif

                    sender_index = i;

                    if (rec_packet.getDataSize() != 0) {

                        unpack_exec(rec_packet, NodeType::Server_TCP);

                        }

                    }
                else {

                    // Nothing? PEP

                    }

                rec_packet.clear();

                }

            }

        }

    // Sending:
    void TCPServer::write(const size_t rec, const HdlInd fn_ind) {
	
        sf::Packet *pkt;

        if (rec != -1) {

            if (get_client_status(rec) != ClientSlot::Working) return;

            pkt = &(client_vec[rec]->packet);

            *pkt << (MsgType) Type::Int8;
            *pkt << (HdlInd ) fn_ind;
            *pkt << (ArgCnt ) 0;

            }
        else {

            for (int i = 0; i < (int)size; i += 1) {

                if (client_vec[i]->status == ClientSlot::Working) {

                    pkt = &(client_vec[i]->packet);

                    *pkt << (MsgType) Type::Int8;
                    *pkt << (HdlInd ) fn_ind;
                    *pkt << (ArgCnt ) 0;

                    }

                }

            }

	    }

    void TCPServer::write(const size_t rec, const HdlInd fn_ind, const Packet &pack) {
    
        sf::Packet *pkt;

        if (rec != -1) {

            if (get_client_status(rec) != ClientSlot::Working) return;

            pkt = &(client_vec[rec]->packet);

            *pkt << (MsgType) Type::Packet;
            *pkt << (HdlInd ) fn_ind;
            *pkt << (PktSize) pack.getDataSize();

            pkt->append(pack.getData(), pack.getDataSize());

            }
        else {

            for (int i = 0; i < (int)size; i += 1) {

                if (client_vec[i]->status == ClientSlot::Working) {

                    pkt = &(client_vec[i]->packet);

                    *pkt << (MsgType) Type::Packet;
                    *pkt << (HdlInd ) fn_ind;
                    *pkt << (PktSize) pack.getDataSize();

                    pkt->append(pack.getData(), pack.getDataSize());

                    }

                }

            }

        }

    void TCPServer::write_s(size_t receiver, HdlInd fn_ind) {
        
        write(receiver, fn_ind);

        }

    void TCPServer::write_s(size_t receiver, HdlInd fn_ind, const Packet &pack) {
        
        write(receiver, fn_ind, pack);

        }

    // Receiving:
    size_t TCPServer::get_sender_index() const {

        return sender_index;

        }

    // Utility:
    Port TCPServer::get_local_port() const {

        return port;

        }

    size_t TCPServer::get_size() const{
	
	    return size;
	
	    }

    void TCPServer::resize(size_t new_size) {
	
        if (new_size == size) return;

        client_vec.resize(new_size);
        packet_vec.resize(new_size);

        if (new_size > size) {
        
            for (size_t i = size; i < new_size; i += 1) {
                client_vec[i] = std::make_unique<ClientSlot>();
                }

            }

        size = new_size;

	    }

    void TCPServer::set_timeout_limit(unsigned timeout_ms) {
        
        timeout_limit = timeout_ms;

        }

    void TCPServer::set_interval(unsigned interval_) {

        interval = MAX(interval_, 1);

        }

    unsigned TCPServer::get_timeout_limit() const {
        
        return timeout_limit;

        }

    unsigned TCPServer::get_interval() const {
        
        return interval;

        }

    std::string TCPServer::get_passphrase() const {
        
        return passphrase;

        }

    void TCPServer::set_passphrase(const std::string & passphrase_) {
        
        if (running) throw std::logic_error("rn::TcpServer::set_passphrase - Can't change passphrase while running!");

        passphrase = passphrase_;
        
        }

    // Client management:
    void TCPServer::client_kick(size_t n, bool force_kick) {

        if (get_client_status(n) != ClientSlot::Working) return;

        if (!force_kick) {

            write(n, HandlerMgr::hv_kick_order);
            client_upload_packet(n);

            }
        else {

            client_vec[n]->reset();

            queue_event( EventFactory::create_disconnect(n) );

            }

        }

    void TCPServer::client_swap(size_t cl1, size_t cl2) {

        if (cl1 >= size || cl2 >= size) throw std::out_of_range("rn::TcpServer::client_swap - Index out of bounds.");

        client_vec[cl1].swap( client_vec[cl2] );

        }

    bool TCPServer::get_client_status(size_t n) const {
	
        if (n >= size) throw std::out_of_range("rn::TcpServer::get_client_status - Index out of bounds.");

	    return ((client_vec[n]->status) == ClientSlot::Working);
	
	    }
    
    Port TCPServer::get_client_port(size_t n) const {
	
	    if (get_client_status(n) != ClientSlot::Working) return 0;

	    return (client_vec[n]->get_port());
	
	    }

    int  TCPServer::get_client_latency(size_t n) const {

	    if (get_client_status(n) != ClientSlot::Working) return -1;

	    return (client_vec[n]->latency).asMilliseconds();
	
	    }

    int  TCPServer::get_client_last_contact(size_t n) const {
    
        if (get_client_status(n) != ClientSlot::Working) return -1;

        return client_vec[n]->prod_clock.getElapsedTime().asMilliseconds();
    
        }

    IpAddress TCPServer::get_client_ip(size_t n) const {

        if (get_client_status(n) != ClientSlot::Working) return sf::IpAddress::None;

        return (client_vec[n]->get_ip());

        }

    // Update numbers:
    Uint32 TCPServer::get_client_update_number(size_t n) const {
    
        if (get_client_status(n) != ClientSlot::Working) return 0;

        return client_vec[n]->update_ordinal;
    
        }

    Uint32 TCPServer::get_server_update_number() const {
    
        return update_ordinal;
    
        }

    //Registry:
    #define RegP RegPermission

    void TCPServer::reg_set_int(const std::string& key, Int32  val) {
    
        my_reg.set_int(key, val);

        write<Int32>(ALL_CLIENTS, HandlerMgr::hv_reg_set_int, {key, val});

        }

    void TCPServer::reg_set_dbl(const std::string& key, Double val) {
    
        my_reg.set_dbl(key, val);

        write<Double>(ALL_CLIENTS, HandlerMgr::hv_reg_set_dbl, {key, val});

        }

    void TCPServer::reg_set_str(const std::string& key, const std::string& val) {
    
        my_reg.set_str(key, val);

        write<Int8>(ALL_CLIENTS, HandlerMgr::hv_reg_set_str, {key, val});

        }

    void TCPServer::reg_del_int(const std::string& key) {
        
        my_reg.del_int(key);

        write<Int8>(ALL_CLIENTS, HandlerMgr::hv_reg_del_int, {key});

        }

    void TCPServer::reg_del_dbl(const std::string& key) {

        my_reg.del_int(key);

        write<Int8>(ALL_CLIENTS, HandlerMgr::hv_reg_del_dbl, {key});

        }

    void TCPServer::reg_del_str(const std::string& key) {

        my_reg.del_int(key);

        write<Int8>(ALL_CLIENTS, HandlerMgr::hv_reg_del_str, {key});

        }

    void TCPServer::reg_clear_int() {
        
        my_reg.clear_int();

        write(ALL_CLIENTS, HandlerMgr::hv_reg_clear_int);

        }

    void TCPServer::reg_clear_dbl() {

        my_reg.clear_dbl();

        write(ALL_CLIENTS, HandlerMgr::hv_reg_clear_dbl);

        }

    void TCPServer::reg_clear_str() {

        my_reg.clear_str();

        write(ALL_CLIENTS, HandlerMgr::hv_reg_clear_str);

        }

    void TCPServer::reg_clear_all() {

        my_reg.clear_all();

        write(ALL_CLIENTS, HandlerMgr::hv_reg_clear_all);

        }

    void TCPServer::reg_set_cl_perm(const std::string& key, size_t n, RegP permission) {
    
        if (get_client_status(n) != ClientSlot::Working) return;

        if (permission == REG_RDONLY) {
            
            (*(client_vec[n]->whitelist)).erase(key);

            }
        else {

            (*(client_vec[n]->whitelist))[key] = permission;

            }

        }

    RegP TCPServer::reg_get_cl_perm(const std::string& key, size_t n) {

        if (get_client_status(n) != ClientSlot::Working) return REG_ERROR;

        auto iter = (client_vec[n]->whitelist)->find(key);

        if (iter == (client_vec[n]->whitelist)->end()) {
            
            return REG_RDONLY;

            }
        else {

            return (*iter).second;
            
            }

        }

    #undef RegP

    // Private:
    void TCPServer::reset_slots() {
        
        for (size_t i = 0; i < size; i += 1) {

            client_vec[i]->reset();

            }

        }

    void TCPServer::client_ping(size_t n, int stage) {

        if (get_client_status(n) != ClientSlot::Working) return;

        switch (stage) {

                case 0: //Send ping to client
                    (client_vec[n]->waiting_for_ping_back) = true;
                    (client_vec[n]->ping_clock).restart();
                    write<Int8>(n, HandlerMgr::hv_ping, {(Int8)0});
                    break;

                case 1: //Got pinged back
                    (client_vec[n]->latency) = (client_vec[n]->ping_clock).getElapsedTime();
                    (client_vec[n]->waiting_for_ping_back)=false;
                    break;

                default:
                    break;

            }

        }

    void TCPServer::client_set_uord(size_t n, unsigned update_ordinal) {

        client_vec[n]->update_ordinal = update_ordinal;

        }

    void TCPServer::client_send_uord(size_t n) {

        write<Uint32>(n, HandlerMgr::hv_set_update_number, {update_ordinal});

        }

    void TCPServer::client_send_index(size_t n) {

        write<Uint32>(n, HandlerMgr::hv_set_client_index, {n});

        }

    void TCPServer::client_reset_pc(size_t n) {

        client_vec[n]->prod_clock.restart();

        }

    void TCPServer::client_upload_reg(size_t n) {

        //Integer map
        for (std::pair<std::string, int> i : my_reg.map_int) {

            write<Int32>(n, HandlerMgr::hv_reg_set_int, {i.first, i.second}); 

            }

        //Double map
        for (std::pair<std::string, double> i : my_reg.map_dbl) {

            write<Double>(n, HandlerMgr::hv_reg_set_dbl, {i.first, i.second});

            }

        //String map
        for (std::pair<std::string, std::string> i : my_reg.map_str) {

            write<Int8>(n, HandlerMgr::hv_reg_set_str, {i.first, i.second}); 

            }

        }

    bool TCPServer::client_upload_packet(size_t n) {

        // Note: Returns FALSE if the client has disconnected, TRUE otherwise.

        #ifdef RIGELNETW_CONFIG_SEND_STATS
        temp_send_cnt += 1;
        temp_byte_cnt += client_vec[n]->packet.getDataSize();

        if (temp_clock.getElapsedTime().asMilliseconds() > 3000) {
            
            std::cout << "Sum over last 3 secods:\n";
            std::cout << "  send_cnt = " << temp_send_cnt   << "\n";
            std::cout << "  byte_cnt = " << temp_byte_cnt   << "\n";
            std::cout << "  updt_cnt = " << temp_update_cnt << "\n";

            temp_update_cnt = 0;
            temp_send_cnt = 0;
            temp_byte_cnt = 0;
            temp_clock.restart();
            
            }
        #endif

        RETRY:
        
        switch ( (client_vec[n]->socket).send(client_vec[n]->packet) ) {

                case sf::Socket::Done:
                    (client_vec[n]->packet).clear();
                    return true;
                    break;

                case sf::Socket::NotReady:
                case sf::Socket::Error:    //PEP
                    // POSTPONE //
                    //std::cout << "Postpone send...\n";
                    return true;
                    break;

                case sf::Socket::Partial:
                    goto RETRY;
                    break;

                case sf::Socket::Disconnected:
                    return false;
                    break;

            }

        return true; // Won't ever happen (surpresses warnings)...

        } 

    #undef MAX

    } // End Rigel namespace