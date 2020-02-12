#pragma once

#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <initializer_list>

namespace RIGELNETW_CONFIG_NS_NAME {

    // Utilities for Handler management 

	class Node;
	class TCPServer;
	class TCPClient;

	//Function pointer for a handler function
	using Handler = void (*)(rn::Node *receiver, NodeType::Enum node_type);

	class HandlerMgr final {

		friend class Node;
		friend class TCPServer;
		friend class TCPClient;
        friend class UDPServer;
        friend class UDPClient;

        private:

            static std::vector<Handler> * access_handler_vector();     

			static void handler_set_internal(const int index, Handler f);

            HandlerMgr() = delete;

            enum EnumHandler {

                // Engine handlers go from -hv_reserved to -1

                  hv_reserved = 20 // CAUTION

                , hv_ping = (-hv_reserved)
                , hv_set_update_number

                , hv_reg_set_int
                , hv_reg_set_dbl
                , hv_reg_set_str

                , hv_reg_del_int
                , hv_reg_del_dbl
                , hv_reg_del_str

                , hv_req_reg_set_int
                , hv_req_reg_set_dbl
                , hv_req_reg_set_str

                , hv_req_reg_del_int
                , hv_req_reg_del_dbl
                , hv_req_reg_del_str

                , hv_reg_clear_int
                , hv_reg_clear_dbl
                , hv_reg_clear_str
                , hv_reg_clear_all

                , hv_kick_order
                , hv_set_client_index

                , hv_sentinel

                };

            static_assert(hv_sentinel <= 0, "RIGEL - hv_count must be <= 0!");

            //Handlers          
            static void ping_node(rn::Node *receiver, NodeType::Enum node_type);
            static void set_update_number(rn::Node *receiver, NodeType::Enum node_type);

            static void reg_set_int(rn::Node *receiver, NodeType::Enum node_type);
            static void reg_set_dbl(rn::Node *receiver, NodeType::Enum node_type);
            static void reg_set_str(rn::Node *receiver, NodeType::Enum node_type);

            static void reg_del_int(rn::Node *receiver, NodeType::Enum node_type);
            static void reg_del_dbl(rn::Node *receiver, NodeType::Enum node_type);
            static void reg_del_str(rn::Node *receiver, NodeType::Enum node_type);

            static void req_reg_set_int(rn::Node *receiver, NodeType::Enum node_type);
            static void req_reg_set_dbl(rn::Node *receiver, NodeType::Enum node_type);
            static void req_reg_set_str(rn::Node *receiver, NodeType::Enum node_type);

            static void req_reg_del_int(rn::Node *receiver, NodeType::Enum node_type);
            static void req_reg_del_dbl(rn::Node *receiver, NodeType::Enum node_type);
            static void req_reg_del_str(rn::Node *receiver, NodeType::Enum node_type);

            static void reg_clear_int(rn::Node *receiver, NodeType::Enum node_type);
            static void reg_clear_dbl(rn::Node *receiver, NodeType::Enum node_type);
            static void reg_clear_str(rn::Node *receiver, NodeType::Enum node_type);
            static void reg_clear_all(rn::Node *receiver, NodeType::Enum node_type);

            static void kick_order(rn::Node *receiver, NodeType::Enum node_type);
            static void set_client_index(rn::Node *receiver, NodeType::Enum node_type);

        public:

            static void    handler_set(const int handler_index, Handler f);
            static Handler handler_get(const int handler_index);
            static void    initialize();
		
		};

    } // End Rigel namespace