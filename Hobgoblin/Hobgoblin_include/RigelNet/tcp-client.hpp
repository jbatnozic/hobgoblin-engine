#pragma once

#include "Rigel-SFML-networking.hpp"
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include "Rigel-handlers.hpp"
#include "Rigel-node.hpp"
#include "Rigel-registry.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <initializer_list>

namespace RIGELNETW_CONFIG_NS_NAME {

    #define HDL_ARGS(type) std::initializer_list<ArgWrap<type>>

    class TCPClient : public Node {
	
        friend class HandlerMgr;

	    private:

            //Status
            size_t my_index;
            bool connecting;
            bool ready; // isConnected
            bool waiting_for_ping_back;
            bool waiting_for_passphrase;

            unsigned interval_cnt;
            unsigned interval;

            unsigned update_ordinal;
            unsigned update_counter;

            std::string passphrase;

            #ifdef RIGELNETW_CONFIG_SEND_STATS
            int temp_updt_cnt;
            int temp_byte_cnt;
            int temp_send_cnt;
            sf::Clock temp_clock;
            #endif

            //Server info
            sf::IpAddress server_ip;
            Port server_port;

            //Socket
		    sf::TcpSocket socket;
		    Packet packet;
            std::vector<Packet*> write_dummy_vec;

            //Time
		    sf::Clock timeout_clock;
		    sf::Time  timeout_limit;
		
		    sf::Clock ping_clock;
		    sf::Time  latency;
		
            sf::Clock prod_clock;

		    //Methods/Utility
            void reset();
            void server_ping(int stage = 0);
            void server_send_uord();	

	    public:

            TCPClient(const std::string & passphrase);
            TCPClient(const std::string & passphrase, IpAddress ip, Port port, unsigned interval = DEFAULT_INTERVAL,
                      unsigned timeout_ms = NO_TIMEOUT);
            ~TCPClient();

            // Client management:
            void connect(IpAddress ip, Port port, unsigned interval = DEFAULT_INTERVAL, unsigned timeout_ms = NO_TIMEOUT);
            void update(bool skip_send = false);
            void disconnect();

            bool is_connecting() const;
            bool is_ready() const;

            // Sending:
            template<class TypeToSend>
            void write(HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args);

            void write(HdlInd fn_ind);

            void write(HdlInd fn_ind, const Packet &pack);

            // Sending, secure versions:
            template<class TypeToSend>
            void write_s(HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args);

            void write_s(HdlInd fn_ind);

            void write_s(HdlInd fn_ind, const Packet &pack);

            // Utility:
            size_t get_index() const;

            int get_server_latency() const;
            int get_server_last_contact() const;

            void set_timeout_limit(unsigned timeout_ms);
            void set_interval(unsigned interval);

            std::string get_passphrase() const;
            void set_passphrase(const std::string & passphrase); // Only when not running

            // Update numbers:
            unsigned get_server_update_number() const;
            unsigned get_client_update_number() const;

            //Registry
            void reg_req_set_int(const std::string& key, Int32  val);
            void reg_req_set_dbl(const std::string& key, Double val);
            void reg_req_set_str(const std::string& key, const std::string& val);

            void reg_req_del_int(const std::string& key);
            void reg_req_del_dbl(const std::string& key);
            void reg_req_del_str(const std::string& key);

	    };

    template <class TypeToSend>
    void TCPClient::write(const HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args) {

        if (!ready) return;

        MsgType basic_msg_type = RIGELNETW_ENUMERATE_TYPE(TypeToSend);

        write_dummy_vec[0] = &packet;

        // TEMP - Better assert
        if (handler_args.size() > RIGELNETW_CONFIG_MAX_ARGS) throw std::logic_error("ClientTCP::write - Trying to send too many arguments.");

        // Append mesages to all packets in the vector:
        append_message_to_packets<TypeToSend>(write_dummy_vec, 1, fn_ind, basic_msg_type, handler_args); 

        }

    template<class TypeToSend>
    void TCPClient::write_s(HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args) {
        
        write<TypeToSend>(fn_ind, handler_args);

        }

    #undef HDL_ARGS

    } // End Rigel namespace
