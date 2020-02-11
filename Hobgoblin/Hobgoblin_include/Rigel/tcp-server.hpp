#pragma once

#include "Rigel-SFML-networking.hpp"
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include "Rigel-node.hpp"
#include "Rigel-client-slot.hpp"
#include "Rigel-registry.hpp"
#include "Rigel-argwrap.hpp"

#include <string>
#include <vector>
#include <memory>
#include <initializer_list>

namespace RIGELNETW_CONFIG_NS_NAME {

    #define HDL_ARGS(type) std::initializer_list<ArgWrap<type>>
    #define RegP RegPermission

    class TCPServer : public Node {

        friend class HandlerMgr;

	    private:

		    //Attributes
            bool running;
		    size_t size;
		    size_t sender_index;
		    Port port;

            unsigned interval;
            unsigned interval_cnt;
            unsigned timeout_limit;
            Uint32 update_ordinal;

		    sf::TcpListener listener;

            std::vector<std::unique_ptr<ClientSlot>> client_vec;
            std::vector<Packet*> packet_vec; // Doesn't own the packets

            std::string passphrase;

            #ifdef RIGELNETW_CONFIG_SEND_STATS
            int temp_update_cnt;
            int temp_byte_cnt;
            int temp_send_cnt;
            sf::Clock temp_clock;
            #endif

		    //Methods
            void reset_slots();

		    void client_ping(size_t n, int stage = 0);

            void client_reset_pc(size_t n);
            void client_upload_reg(size_t n);

            bool client_upload_packet(size_t n);

            void client_set_uord(size_t n, unsigned update_ordinal);
            void client_send_uord(size_t n);
            void client_send_index(size_t n);

	    public: // -----------------------------------------------------------------------------------

        TCPServer() = delete;
        TCPServer(size_t size, const std::string & passphrase);
        TCPServer(size_t size, const std::string & passphrase, Port port, unsigned interval = DEFAULT_INTERVAL,
                  unsigned timeout_ms = NO_TIMEOUT);

        ~TCPServer();

        // Server management:
        bool start(Port port = ANY_PORT, unsigned interval = DEFAULT_INTERVAL, unsigned timeout_ms = NO_TIMEOUT);
        void stop();
        bool is_running() const;
        void update(bool skip_send = false);

        // Sending:
        template<class TypeToSend>
        void write(size_t receiver, HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args);

        void write(size_t receiver, HdlInd fn_ind);

        void write(size_t receiver, HdlInd fn_ind, const Packet &pack);

        // Sending, secure versions:
        template<class TypeToSend>
        void write_s(size_t receiver, HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args);

        void write_s(size_t receiver, HdlInd fn_ind);

        void write_s(size_t receiver, HdlInd fn_ind, const Packet &pack);

        // Receiving:
        size_t get_sender_index() const;

        // Utility:
        Port   get_local_port() const;
        size_t get_size() const;
        void   resize(size_t new_size);

        void set_timeout_limit(unsigned timeout_ms);
        void set_interval(unsigned interval);

        unsigned get_timeout_limit() const;
        unsigned get_interval() const;

        std::string get_passphrase() const;
        void set_passphrase(const std::string & passphrase); // Only when not running

        // Client management:
        void client_kick(size_t client_index, bool force_kick = false);
        void client_swap(size_t client_index_1, size_t client_index_2);

        bool get_client_status(size_t client_index) const; // true = Connected
        Port get_client_port(size_t client_index) const;
        int  get_client_latency(size_t client_index) const;      
        int  get_client_last_contact(size_t client_index) const;

        IpAddress get_client_ip(size_t n) const;

        // Update numbers:
        Uint32 get_client_update_number(size_t client_index) const;
        Uint32 get_server_update_number() const;

        // Registry:
        void reg_set_int(const std::string& key, Int32  val);
        void reg_set_dbl(const std::string& key, Double val);
        void reg_set_str(const std::string& key, const std::string& val);

        void reg_del_int(const std::string& key);
        void reg_del_dbl(const std::string& key);
        void reg_del_str(const std::string& key);

        void reg_clear_int();
        void reg_clear_dbl();
        void reg_clear_str();
        void reg_clear_all();

        void reg_set_cl_perm(const std::string& key, size_t client_index, RegP permission);
        RegP reg_get_cl_perm(const std::string& key, size_t client_index);

	    };

    template <class TypeToSend>
    void TCPServer::write(size_t rec, HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args) {
    
        MsgType basic_msg_type = RIGELNETW_ENUMERATE_TYPE(TypeToSend);

        // TEMP - Better assert
        if (handler_args.size() > RIGELNETW_CONFIG_MAX_ARGS) throw std::logic_error("ServerTCP::tsend - Trying to send too many arguments.");

        // Prepare packet_vec:
        size_t pktcnt = 0;

        if (rec != -1) {

            if (get_client_status(rec) == ClientSlot::Working) {

                packet_vec[0] = &(client_vec[rec]->packet);
                pktcnt += 1;

                }

            }
        else {

            for (size_t i = 0; i < size; i += 1) {

                if (client_vec[i]->status == ClientSlot::Working) {

                    packet_vec[pktcnt] = &(client_vec[i]->packet);
                    pktcnt += 1;

                    }

                }

            }

        if (pktcnt == 0) return;

        //printf("tsend diag: msg_type = %d, pktcnt = %d, fn_ind = %d, argc = %d\n", (int)basic_msg_type, (int)pktcnt, (int)fn_ind, (int)handler_args.size());

        // Append mesages to all packets in the vector:
        append_message_to_packets<TypeToSend>(packet_vec, pktcnt, fn_ind, basic_msg_type, handler_args); 

        }

    template <class TypeToSend>
    void TCPServer::write_s(size_t rec, HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args) {

        write<TypeToSend>(rec, fn_ind, handler_args);

        }

    #undef HDL_ARGS
    #undef RegP

    } // End Rigel namespace