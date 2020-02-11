#pragma once

#include "Rigel-SFML-networking.hpp"
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include "Rigel-handlers.hpp"
#include "Rigel-node.hpp"
#include "Rigel-registry.hpp"
#include "Rigel-udp-connector.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <initializer_list>

namespace RIGELNETW_CONFIG_NS_NAME {

    #define HDL_ARGS(type) std::initializer_list<ArgWrap<type>>

    class UDPClient : public Node {

        friend class HandlerMgr;

    public:

        UDPClient(const std::string & passphrase);
        UDPClient(const std::string & passphrase, IpAddress ip, Port port, unsigned interval = DEFAULT_INTERVAL,
                  unsigned timeout_ms = NO_TIMEOUT);
        ~UDPClient();

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

    private:

        size_t my_index;

        sf::UdpSocket socket;

        UDPConnector conn;

        //Methods/Utility
        void reset();
        void server_ping(int stage = 0);
        void server_send_uord();

        };

    template <class TypeToSend>
    void UDPClient::write(const HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args) {

        conn.write<TypeToSend>(fn_ind, handler_args);

        }

    template<class TypeToSend>
    void UDPClient::write_s(HdlInd fn_ind, HDL_ARGS(TypeToSend) handler_args) {

        conn.write_s<TypeToSend>(fn_ind, handler_args);

        }

    #undef HDL_ARGS

    } // End Rigel namespace

