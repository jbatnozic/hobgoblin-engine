#pragma once

#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include <string>

namespace RIGELNETW_CONFIG_NS_NAME {

    struct EventFactory;

    #define event_void char rigelnetw_event_void

    class Event {
        
        friend struct EventFactory;

        public:

            // Type enumeration:
            struct Type {

                enum Enum {

                    Null,
                    Connect,
                    Disconnect,
                    AttempTimeout,
                    ConnTimeout,
                    IllegalRegReq,
                    Kicked,
                    BadPassphrase,

                    Count

                    };

                };

            // [Server]: New client (data = client index)
            // [Client]: Connected to server (data = void)
            struct EvConnect {
                size_t client_index;
                };

            // [Server]: Client disconnected (data = client index)
            // [Client]: Disconnected from server (data = void)
            struct EvDisconnect {
                size_t client_index;
                };

            // [Server]: n/a
            // [Client]: Connection attempt timed out (data = void)
            struct EvAttemptTimeout {
                unsigned time_ms;
                };

            // [Server]: Client connection timed out (data = client index)
            // [Client]: Server connection timed out (data = void)
            struct EvConnTimeout {
                size_t client_index;
                };

            // [Server]: Illegal registry change request (data = client number, registry key)
            // [Client]: n/a
            struct EvIllegalRegReq {
                size_t client_index;
                };

            // [Server]: n/a
            // [Client]: Kicked from server (data = void)
            struct EvKicked {
                event_void;
                };

            // [Server]: Passphrase mismatch (data = client_index)
            // [Client]: Passphrase mismatch (data = void)
            struct EvBadPassphrase {
                size_t client_index;
                };

            // Fields:
            Type::Enum type;

            union {
                
                EvConnect        connect;
                EvDisconnect     disconnect;
                EvAttemptTimeout attempt_timeout;
                EvConnTimeout    conn_timeout;
                EvIllegalRegReq  illegal_reg_req;
                EvKicked         kicked;
                EvBadPassphrase  bad_passphrase;

                };

            Event();

            // Special methods for getting strings:
            std::string illegal_reg_req_key() const;
            std::string bad_passphrase_str() const;

        private:

            std::string m_string;

            Event(Type::Enum type);

        };

    #undef event_void
    
    struct EventFactory {

        static Event create_connect(size_t client_index = size_t(-1));
        static Event create_disconnect(size_t client_index = size_t(-1));
        static Event create_attempt_timeout(unsigned time_ms);
        static Event create_conn_timeout(size_t client_index = size_t(-1));
        static Event create_illegal_reg_req(size_t client_index, std::string& key);
        static Event create_kicked(void);
        static Event create_bad_passphrase(const std::string & pp, size_t client_index = size_t(-1));

        };
    
    } // End Rigel namespace