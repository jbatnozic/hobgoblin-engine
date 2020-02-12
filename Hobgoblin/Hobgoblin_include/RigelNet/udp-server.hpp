#pragma once

#include "Rigel-SFML-networking.hpp"
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include "Rigel-node.hpp"
#include "Rigel-client-slot.hpp"
#include "Rigel-registry.hpp"
#include "Rigel-argwrap.hpp"
#include "Rigel-udp-connector.hpp"

#include <string>
#include <vector>
#include <memory>
#include <initializer_list>

namespace RIGELNETW_CONFIG_NS_NAME {

    #define HDL_ARGS(type) std::initializer_list<ArgWrap<type>>
    #define RegP RegPermission

    class UDPServer : public Node {

        friend class HandlerMgr;
        friend class UDPConnector;

    public:

        UDPServer() = delete;
        UDPServer(size_t size, const std::string & passphrase);
        UDPServer(size_t size, const std::string & passphrase, Port port, unsigned interval = DEFAULT_INTERVAL,
                  unsigned timeout_ms = NO_TIMEOUT);

        ~UDPServer();

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

    private: // -----------------------------------------------------------------------------------

        //Attributes
        bool running;
        size_t size;
        size_t sender_index;
        Port port;

        unsigned interval;
        unsigned interval_cnt;
        unsigned timeout_limit;
        Uint32 update_ordinal;

        sf::UdpSocket socket;

        std::vector<std::unique_ptr<UDPConnector>> client_vec;
        std::vector<sf::Packet*> packet_vec; // Doesn't own the packets

        std::string passphrase;

        //Methods
        int find_connector(IpAddress addr, Port port) const;

        void handle_unknown_packet(Packet & pack, IpAddress addr, Port port);

        void reset_connectors(unsigned interval = DEFAULT_INTERVAL, unsigned timeout_ms = NO_TIMEOUT);

        void client_ping(size_t n, int stage = 0);

        void client_upload_reg(size_t n);

        void client_send_uord(size_t n);
        void client_send_index(size_t n);

        };

    template <class TypeToSend>
    void UDPServer::write(size_t rec, HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args) {

        if (rec == ALL_CLIENTS) {
            
            for (size_t i = 0; i < size; i += 1) {
                
                client_vec[i]->write<TypeToSend>(fn_ind, handler_args);

                }

            }
        else if (get_client_status(rec) == true) {
            
            client_vec[rec]->write<TypeToSend>(fn_ind, handler_args);

            }

        }

    template <class TypeToSend>
    void UDPServer::write_s(size_t rec, HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args) {

        if (rec == ALL_CLIENTS) {

            for (size_t i = 0; i < size; i += 1) {

                client_vec[i]->write_s<TypeToSend>(fn_ind, handler_args);

                }

            }
        else if (get_client_status(rec) == true) {

            client_vec[rec]->write_s<TypeToSend>(fn_ind, handler_args);

            }

        }

    #undef HDL_ARGS
    #undef RegP

    } // End Rigel namespace