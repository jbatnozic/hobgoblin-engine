
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"
#include "Rigel-tcp-client.hpp"

#include <stdexcept>
#include <iostream>

namespace RIGELNETW_CONFIG_NS_NAME {

    #define MAX(x, y) ((x>=y)?(x):(y))

    TCPClient::TCPClient(const std::string & passphrase_)
        : write_dummy_vec(1)
        , passphrase(passphrase_) {
    
        reset();

        socket.setBlocking(false); 

        }

    TCPClient::TCPClient(const std::string & passphrase_, IpAddress ip, Port port, unsigned interval,
                         unsigned timeout_ms)
        : TCPClient(passphrase_) {
        
        connect(ip, port, interval, timeout_ms);

        }

    TCPClient::~TCPClient() {

        reset();

        }

    // Client management:
    void TCPClient::connect(const IpAddress ip, Port port, unsigned interval, unsigned timeout_ms) {
	
        if (connecting) throw std::logic_error("rn::TcpClient::connect - Already trying to connect!");
        if (ready)      throw std::logic_error("rn::TcpClient::connect - Already connected!");

	    socket.connect(ip, port, sf::milliseconds((sf::Int32)timeout_ms));

	    timeout_clock.restart();

	    timeout_limit = sf::milliseconds(timeout_ms);

	    connecting = true;

        this->interval = interval;

        #ifdef RIGELNETW_CONFIG_SEND_STATS
        temp_updt_cnt = 0;
        temp_byte_cnt = 0;
        temp_send_cnt = 0;
        temp_clock.restart();
        #endif

        #ifndef RIGELNETW_CONFIG_NO_PASSPHRASE
        packet << passphrase; // Send RAW passphrase
        #endif

	    }

    void TCPClient::update(bool skip_send) {

        if (!(ready || connecting)) return;

        #ifdef RIGELNETW_CONFIG_SEND_STATS
        temp_updt_cnt += 1;
        #endif

	    bool old_ready = ready;

        // [EVENTS] Clear event queue (must be first)
        events.clear();

        // [TIMEOUT] Disconnect if over timeout limit
        if (ready && timeout_limit != sf::Time::Zero) {
            
            if (prod_clock.getElapsedTime() >= timeout_limit) {
                
                queue_event( EventFactory::create_conn_timeout() );
                disconnect();
                return;

                }

            }

        // [PING] Ping the server if connected:
        if (ready) {

            if (!waiting_for_ping_back) {
                server_ping();
                }

            }

	    // [UPLOAD] Send data to TCPServer + ping/prod
        if (!skip_send) {

            interval_cnt = (interval_cnt + 1) % interval; 

	        if ((ready /*|| connecting*/) && (interval_cnt == 0)) {

                // Ping/prod
                if (ready) {

                    server_send_uord();

                    update_counter += 1;

                    }

                #ifdef RIGELNETW_CONFIG_SEND_STATS
                temp_byte_cnt += packet.getDataSize();
                temp_send_cnt += 1;

                if (temp_clock.getElapsedTime().asMilliseconds() > 3000) {

                    std::cout << "Sum over last 3 secods:\n";
                    std::cout << "  send_cnt = " << temp_send_cnt << "\n";
                    std::cout << "  byte_cnt = " << temp_byte_cnt << "\n";
                    std::cout << "  updt_cnt = " << temp_updt_cnt << "\n";

                    temp_updt_cnt = 0;
                    temp_send_cnt = 0;
                    temp_byte_cnt = 0;
                    temp_clock.restart();

                    }
                #endif

                // Send packets:
		        RETRY:

		        switch ( socket.send(packet) ) {
				
			        case sf::Socket::Done:
				        ready = true;
				        break;

			        case sf::Socket::Partial:
				        goto RETRY;
				        break;			    

			        case sf::Socket::NotReady:
                    case sf::Socket::Error:    // PEP
				        goto POSTPONE;
                        //std::cout << "Postpone send...\n";
				        break;

			        case sf::Socket::Disconnected:
				        ready = false;
				        break;

			        }

		        packet.clear();	

		        POSTPONE: { }

		        }

            }

	    // [DOWNLOAD] Receive data from the server
	    if (ready || connecting)	{
	
		    sf::Packet rec_packet;

		    switch ( socket.receive(rec_packet) ) {

			    case sf::Socket::Done:
				    ready = true;
                    if (waiting_for_passphrase && rec_packet.getDataSize() > 0) {
                        
                        #ifndef RIGELNETW_CONFIG_NO_PASSPHRASE
                        std::string temp;
                        rec_packet >> temp;

                        if (temp != passphrase) {
                            
                            queue_event( EventFactory::create_bad_passphrase(temp) );
                            disconnect();
                            return;

                            }
                        else
                        #endif
                            waiting_for_passphrase = false;

                        }
				    unpack_exec(rec_packet, NodeType::Client_TCP);
				    break;

			    case sf::Socket::NotReady:
			    case sf::Socket::Error:    // PEP
				    break;

			    case sf::Socket::Disconnected:
				    ready = false;		
				    break;

			    default:
				    break;

			    } // End switch
	
		    }

	    // [EVENTS] Timeout connection attempt
	    if (!ready && connecting && timeout_clock.getElapsedTime() > timeout_limit) {

            queue_event( EventFactory::create_attempt_timeout(timeout_clock.getElapsedTime().asMilliseconds()) );

            disconnect();

		    }
	
	    // [EVENTS] Notify of status updates
	    if (!old_ready && ready) {

		    connecting = false;

            prod_clock.restart();

            queue_event( EventFactory::create_connect() );

		    }
	    else if (old_ready && !ready) {

            queue_event( EventFactory::create_disconnect() );

            disconnect();

		    }

	    }

    void TCPClient::disconnect() {

        my_index = size_t(-1);
        connecting = false;
        ready = false;
        waiting_for_ping_back  = false;
        waiting_for_passphrase = true;

        interval_cnt = 0;
        
        // interval - don't reset

        update_ordinal = 0;
        update_counter = 1;

        server_ip = sf::IpAddress::None;
        server_port = 0;

        socket.disconnect();
        packet.clear();

        // timeout_limit - don't reset

        latency = sf::milliseconds(-1);

        // Inherited:
        my_reg.clear_all();

        }

    bool TCPClient::is_connecting() const {
        
        return connecting;

        }

    bool TCPClient::is_ready() const {
        
        return ready;

        }

    // Sending:
    void TCPClient::write(const HdlInd fn_ind) {
	
        if (!ready) return;

	    packet << (MsgType) Type::Int8;
	    packet << (HdlInd ) fn_ind;
	    packet << (ArgCnt ) 0;

	    }

    void TCPClient::write(const HdlInd fn_ind, const Packet &pack) {  

        if (!ready) return;

        packet << (MsgType) Type::Packet;
        packet << (HdlInd ) fn_ind;
        packet << (PktSize) pack.getDataSize();

        packet.append(pack.getData(), pack.getDataSize());       

        }

    void TCPClient::write_s(HdlInd fn_ind) {
        
        write(fn_ind);

        }

    void TCPClient::write_s(HdlInd fn_ind, const Packet &pack) {
        
        write(fn_ind, pack);

        }

    // Utility:
    size_t TCPClient::get_index() const {
        
        return my_index;

        }

    int TCPClient::get_server_latency() const  {

        return latency.asMilliseconds();

        }

    int TCPClient::get_server_last_contact() const {
    
        if (!ready) return -1;

        return prod_clock.getElapsedTime().asMilliseconds();

        }

    void TCPClient::set_timeout_limit(unsigned timeout_ms) {
        
        timeout_limit = sf::milliseconds((sf::Int32)timeout_ms);

        }

    void TCPClient::set_interval(unsigned interval_) {
        
        interval = MAX(interval_, 1);

        }

    std::string TCPClient::get_passphrase() const {

        return passphrase;

        }

    void TCPClient::set_passphrase(const std::string & passphrase_) {

        if (connecting || ready) 
            throw std::logic_error("rn::TCPClient::set_passphrase - Can't change passphrase while running!");

        passphrase = passphrase_;

        }

    // Update ordinals:
    unsigned TCPClient::get_server_update_number() const {
    
        return update_ordinal;
    
        }

    unsigned TCPClient::get_client_update_number() const {

        return update_counter;

        }

    // Registry:
    void TCPClient::reg_req_set_int(const std::string& key, Int32  val) {
    
        write<Int32>(HandlerMgr::hv_req_reg_set_int, {key, val});

        }

    void TCPClient::reg_req_set_dbl(const std::string& key, Double val) {
    
        write<Double>(HandlerMgr::hv_req_reg_set_dbl, {key, val});

        }

    void TCPClient::reg_req_set_str(const std::string& key, const std::string& val) {
    
        write<Int8>(HandlerMgr::hv_req_reg_set_str, {key, val});

        }

    void TCPClient::reg_req_del_int(const std::string& key) {
        
        write<Int8>(HandlerMgr::hv_req_reg_del_int, {key});

        }

    void TCPClient::reg_req_del_dbl(const std::string& key) {
        
        write<Int8>(HandlerMgr::hv_req_reg_del_dbl, {key});

        }

    void TCPClient::reg_req_del_str(const std::string& key) {
        
        write<Int8>(HandlerMgr::hv_req_reg_del_str, {key});

        }

    // Private:
    void TCPClient::reset() {

        my_index = size_t(-1);
        connecting = false;
        ready = false;
        waiting_for_ping_back  = false;
        waiting_for_passphrase = true;

        interval_cnt = 0;
        interval = 1;

        update_ordinal = 0;
        update_counter = 1;

        server_ip = sf::IpAddress::None;
        server_port = 0;

        socket.disconnect();
        packet.clear();

        timeout_limit = sf::milliseconds((sf::Int32)NO_TIMEOUT);

        latency = sf::milliseconds(-1);

        // Inherited:
        my_reg.clear_all();

        }

    void TCPClient::server_ping(int stage) {

        switch (stage) {

                case 0: //Send ping to ServerTCP
                    waiting_for_ping_back = true;
                    ping_clock.restart();
                    write<Int8>(HandlerMgr::hv_ping, {(Int8)0});
                    break;

                case 1: //Got pinged back
                    latency = ping_clock.getElapsedTime();
                    waiting_for_ping_back = false;
                    break;

                default:
                    break;

            }

        }

    void TCPClient::server_send_uord() {

        write<Uint32>(HandlerMgr::hv_set_update_number, {update_counter});

        }

    #undef MAX

    } // End Rigel namespace