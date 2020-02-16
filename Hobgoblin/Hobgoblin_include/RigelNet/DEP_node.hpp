#pragma once

#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <memory>
#include <stdexcept>
#include <initializer_list>

#include "Rigel-SFML-networking.hpp"
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include "Rigel-argwrap.hpp"
#include "Rigel-argrecv.hpp"
#include "Rigel-event.hpp"
#include "Rigel-handlers.hpp"
#include "Rigel-registry.hpp"

namespace RIGELNETW_CONFIG_NS_NAME {

    static const Port     ANY_PORT = sf::Socket::AnyPort;
    static const size_t   ALL_CLIENTS = size_t(-1);
    static const unsigned DEFAULT_INTERVAL = 2;
    static const unsigned NO_TIMEOUT = 0;

    ///////////////////////////////////////////////////////////////////////////

	class Node : public sf::NonCopyable {

        friend class HandlerMgr;
        friend class UDPConnector;

        template<class TypeToRet>
        friend TypeToRet arg(const Node *node, size_t index);

        friend rn::Packet & pkt(Node *node);

		private:

			static const size_t ARG_VECTOR_SIZE = RIGELNETW_CONFIG_MAX_ARGS;

            ArgRecv argrecv_vec[ARG_VECTOR_SIZE];

            sf::Packet user_packet;

        protected:

            //Registry
            Registry my_reg;

            //'Structors
            Node();

			virtual ~Node() = 0; //Pure virtual		

			//Methods
            bool unpack_exec(Packet &packet, NodeType::Enum node_type);

            // New impl.
            template<class TypeToSend>
            static void append_message_to_packets( const std::vector<sf::Packet*> &packets
                                                 , const size_t pktcnt
                                                 , const int fn_ind
                                                 , /* */ MsgType msg_type
                                                 , const std::initializer_list<ArgWrap<TypeToSend>> &args
                                                 ) ;

            //Event queue
            std::deque<Event> events;

            void  clear_events();
            void  queue_event(Event ev);

        public:

            //Registry
            bool reg_int_exists(const std::string& key) const;
            bool reg_dbl_exists(const std::string& key) const;
            bool reg_str_exists(const std::string& key) const;

            Int32       reg_get_int(const std::string& key) const;
            Double      reg_get_dbl(const std::string& key) const;
            std::string reg_get_str(const std::string& key) const;

            bool reg_sget_int(const std::string& key, Int32  &val) const;
            bool reg_sget_dbl(const std::string& key, Double &val) const;
            bool reg_sget_str(const std::string& key, std::string &val) const;

            //Event queue
            bool poll_event(Event &ev);
		
		};

    //-------------------------------------------------------------------------------------------//

    // New impl.
    template<class TypeToSend>
    void Node::append_message_to_packets( const std::vector<sf::Packet*> &packets
                                        , const size_t pktcnt
                                        , const int fn_ind
                                        , /* */ MsgType msg_type
                                        , const std::initializer_list<ArgWrap<TypeToSend>> &args
                                        ) {

        // Prepare typevec:
        TypeVec typevec = 0;
        size_t  shlcnt  = 0;

        for (const ArgWrap<TypeToSend> *iter = args.begin(); iter != args.end(); iter += 1) {

            typevec |= (static_cast<TypeVec>((*iter).is_string()) << shlcnt);

            shlcnt += 1;

            }

        bool has_str = (typevec > 0);

        if (has_str) msg_type += Type::String;

        // Write msg_type, fn_ind, argcount and (if needed) typevec
        for (size_t i = 0; i < pktcnt; i += 1) {
            
            (*(packets[i])) << (Int8)  msg_type;
            (*(packets[i])) << (Int16) fn_ind;
            (*(packets[i])) << (Int8)  args.size();

            if (has_str) (*packets[i]) << typevec;

            }

        // Write arguments:
        for (const ArgWrap<TypeToSend> *iter = args.begin(); iter != args.end(); iter += 1) {

            bool is_str = (*iter).is_string();

            for (size_t i = 0; i < pktcnt; i += 1) {
                
                if (!is_str) {
                    
                    (*(packets[i])) << (*iter).get_number();

                    }
                else {
                    
                    (*(packets[i])) << (*iter).get_string();

                    }

                }

            }

        }
    
	} // End Rigel namespace