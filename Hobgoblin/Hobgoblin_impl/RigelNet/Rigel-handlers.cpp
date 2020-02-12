
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include "Rigel-handlers.hpp"
#include "Rigel-argfunc.hpp"
#include "Rigel-node.hpp"
#include "Rigel-tcp-client.hpp"
#include "Rigel-tcp-server.hpp"
#include "Rigel-udp-client.hpp"
#include "Rigel-udp-server.hpp"

#include <iostream> // Temp
//#include <string>
#include <stdexcept>

#define _RN_IS_SERVER(type) ((type) == NodeType::Server_TCP || (type) == NodeType::Server_UDP)
#define _RN_IS_CLIENT(type) ((type) == NodeType::Client_TCP || (type) == NodeType::Client_UDP)
#define _RN_ASSERT(expression) do { if (!(expression)) throw std::exception(); } while (0)

namespace RIGELNETW_CONFIG_NS_NAME {

    std::vector<Handler> * HandlerMgr::access_handler_vector() {
    
        static std::vector<rn::Handler> handler_vector(hv_reserved + CONFIG_HDLVEC_INIT_N);

        return &handler_vector;

        }

    void HandlerMgr::initialize() {

        handler_set_internal(hv_ping, ping_node);

        handler_set_internal(hv_set_update_number, set_update_number);

        handler_set_internal(hv_reg_set_int, reg_set_int);
        handler_set_internal(hv_reg_set_dbl, reg_set_dbl);
        handler_set_internal(hv_reg_set_str, reg_set_str);

        handler_set_internal(hv_reg_del_int, reg_del_int);
        handler_set_internal(hv_reg_del_dbl, reg_del_dbl);
        handler_set_internal(hv_reg_del_str, reg_del_str);

        handler_set_internal(hv_req_reg_set_int, req_reg_set_int);
        handler_set_internal(hv_req_reg_set_dbl, req_reg_set_dbl);
        handler_set_internal(hv_req_reg_set_str, req_reg_set_str);

        handler_set_internal(hv_req_reg_del_int, req_reg_del_int);
        handler_set_internal(hv_req_reg_del_dbl, req_reg_del_dbl);
        handler_set_internal(hv_req_reg_del_str, req_reg_del_str);

        handler_set_internal(hv_reg_clear_int, reg_clear_int);
        handler_set_internal(hv_reg_clear_dbl, reg_clear_dbl);
        handler_set_internal(hv_reg_clear_str, reg_clear_str);
        handler_set_internal(hv_reg_clear_all, reg_clear_str);

        handler_set_internal(hv_kick_order, kick_order);
        handler_set_internal(hv_set_client_index, set_client_index);

        }

    void HandlerMgr::handler_set_internal(const int index, Handler f) {

        size_t n = static_cast<size_t>( hv_reserved + index );

        auto *hv = access_handler_vector();

        if (hv->size() < n + 1) hv->resize(n + 1);

        (*hv)[n] = f;

        //std::cout << "Handler set to index " << (int)n << "/" << hv->size() << std::endl;

        }

    void HandlerMgr::handler_set(const int index, Handler f) {

        handler_set_internal(static_cast<const int>(index), f);
		
	    }

    Handler HandlerMgr::handler_get(const int index) {
		
        size_t n = static_cast<size_t>( hv_reserved + index );

        auto *hv = access_handler_vector();

        //std::cout << "Getting handler of index " << (int)n << std::endl;

        if (hv->size() < n + 1) return nullptr;

	    return (*hv)[n];

	    }

    //Handlers:
    void HandlerMgr::ping_node(rn::Node *receiver, NodeType::Enum node_type) {

        // Parameters:
        // char - stage

        Int8 stage = arg<Int8>(receiver, 0);

        bool is_server = _RN_IS_SERVER(node_type);

        if (node_type == NodeType::Server_TCP || node_type == NodeType::Client_TCP) {

            switch (is_server*10 + stage) {

                    case 00: //Client_TCP received stage 0:
                        static_cast<TCPClient*>(receiver)->write_s<Int8>(HandlerMgr::hv_ping, {1});
                        break;

                    case 10: //Server_TCP received stage 0:
                        static_cast<TCPServer*>(receiver)->write_s<Int8>( static_cast<TCPServer*>(receiver)->get_sender_index(), HandlerMgr::hv_ping, {1} );
                        break;

                    case 01: //Client_TCP received stage 1:
                        static_cast<TCPClient*>(receiver)->server_ping(1);
                        break;

                    case 11: //Server_TCP received stage 1:
                        static_cast<TCPServer*>(receiver)->client_ping( static_cast<TCPServer*>(receiver)->get_sender_index(), 1 );
                        break;

                    default:
                        break;		

                }

            }
        else if (node_type == NodeType::Server_UDP || node_type == NodeType::Client_UDP) {

            switch (is_server*10 + stage) {

                case 00: //Client_UDP received stage 0:
                    static_cast<UDPClient*>(receiver)->write_s<Int8>(HandlerMgr::hv_ping, {1});
                    break;

                case 10: //Server_UDP received stage 0:
                    static_cast<UDPServer*>(receiver)->write_s<Int8>( static_cast<UDPServer*>(receiver)->get_sender_index(), HandlerMgr::hv_ping, {1} );
                    break;

                case 01: //Client_UDP received stage 1:
                    static_cast<UDPClient*>(receiver)->server_ping(1);
                    break;

                case 11: //Server_UDP received stage 1:
                    static_cast<UDPServer*>(receiver)->client_ping( static_cast<UDPServer*>(receiver)->get_sender_index(), 1 );
                    break;

                default:
                    break;		

                }

            }
        else {
            
            _RN_ASSERT(0);

            }

        }

    // Pep
    void HandlerMgr::set_update_number(rn::Node *receiver, NodeType::Enum node_type) {
    
        Uint32 un = arg<Uint32>(receiver, 0);

        if (node_type == NodeType::Server_TCP) {

            TCPServer *s = static_cast<TCPServer*>(receiver);
            s->client_set_uord( s->get_sender_index(), un);
            s->client_reset_pc( s->get_sender_index() );

            }
        else if (node_type == NodeType::Client_TCP) {

            static_cast<TCPClient*>(receiver)->update_ordinal = un;
            static_cast<rn::TCPClient*>(receiver)->prod_clock.restart();

            }
        else if (node_type == NodeType::Server_UDP) {

            UDPServer *s = static_cast<UDPServer*>(receiver);
            s->client_vec[s->get_sender_index()]->update_ordinal = un;
            s->client_vec[s->get_sender_index()]->timeout_clock.restart();

            }
        else if (node_type == NodeType::Client_UDP) {

            static_cast<UDPClient*>(receiver)->conn.update_ordinal = un;
            static_cast<UDPClient*>(receiver)->conn.timeout_clock.restart();

            }
        else {
            
            _RN_ASSERT(0);
            
            }

        }

    void HandlerMgr::reg_set_int(rn::Node *receiver, NodeType::Enum node_type) {
    
        std::string key = arg<Lpcstr>(receiver, 0);
        Int32       val = arg<Int32 >(receiver, 1);

        receiver->my_reg.set_int(key, val);

        }

    void HandlerMgr::reg_set_dbl(rn::Node *receiver, NodeType::Enum node_type) {

        std::string key = arg<Lpcstr>(receiver, 0);
        Double      val = arg<Double>(receiver, 1);

        _RN_ASSERT( _RN_IS_CLIENT(node_type) );

        receiver->my_reg.set_dbl(key, val);

        }

    void HandlerMgr::reg_set_str(rn::Node *receiver, NodeType::Enum node_type) {

        std::string key = arg<Lpcstr>(receiver, 0);
        std::string val = arg<Lpcstr>(receiver, 1);

        _RN_ASSERT( _RN_IS_CLIENT(node_type) );

        receiver->my_reg.set_str(key, val);

        }

    void HandlerMgr::reg_del_int(rn::Node *receiver, NodeType::Enum node_type) {
        
        std::string key = arg<Lpcstr>(receiver, 0);

        _RN_ASSERT( _RN_IS_CLIENT(node_type) );

        receiver->my_reg.del_int(key);

        }

    void HandlerMgr::reg_del_dbl(rn::Node *receiver, NodeType::Enum node_type) {
        
        std::string key = arg<Lpcstr>(receiver, 0);

        _RN_ASSERT( _RN_IS_CLIENT(node_type) );

        receiver->my_reg.del_dbl(key);

        }

    void HandlerMgr::reg_del_str(rn::Node *receiver, NodeType::Enum node_type) {
        
        std::string key = arg<Lpcstr>(receiver, 0);

        _RN_ASSERT( _RN_IS_CLIENT(node_type) );

        receiver->my_reg.del_str(key);

        }

    void HandlerMgr::req_reg_set_int(rn::Node *receiver, NodeType::Enum node_type) {

        std::string key = arg<Lpcstr>(receiver, 0);
        Int32       val = arg<Int32 >(receiver, 1);

        if (node_type == NodeType::Server_TCP) { // TCP   

            TCPServer *s = static_cast<TCPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_WR_INT) == 0) {
            
                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;
            
                }

            s->reg_set_int(key, val);

            }
        else if (node_type == NodeType::Server_UDP) { // UDP
        
            UDPServer *s = static_cast<UDPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_WR_INT) == 0) {

                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;

                }

            s->reg_set_int(key, val);

            }
        else {
            
            _RN_ASSERT(0);
            
            }

        }

    void HandlerMgr::req_reg_set_dbl(rn::Node *receiver, NodeType::Enum node_type) {

        std::string key = arg<Lpcstr>(receiver, 0);
        Double      val = arg<Double>(receiver, 1);

        if (node_type == NodeType::Server_TCP) { // TCP      

            TCPServer *s = static_cast<TCPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_WR_DBL) == 0) {

                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;

                }

            s->reg_set_dbl(key, val);

            }
        else if (node_type == NodeType::Server_UDP) { // UDP

            UDPServer *s = static_cast<UDPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_WR_DBL) == 0) {

                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;

                }

            s->reg_set_dbl(key, val);

            }
        else {

            _RN_ASSERT(0);

            }

        }

    void HandlerMgr::req_reg_set_str(rn::Node *receiver, NodeType::Enum node_type) {

        std::string key = arg<Lpcstr>(receiver, 0);
        std::string val = arg<Lpcstr>(receiver, 1);    

        if (node_type == NodeType::Server_TCP) { // TCP  

            TCPServer *s = static_cast<TCPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_WR_STR) == 0) {

                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;

                }

            s->reg_set_str(key, val);

            }
        else if (node_type == NodeType::Server_UDP) { // UDP

            UDPServer *s = static_cast<UDPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_WR_STR) == 0) {

                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;

                }

            s->reg_set_str(key, val);

            }
        else {

            _RN_ASSERT(0);

            }

        }

    void HandlerMgr::req_reg_del_int(rn::Node *receiver, NodeType::Enum node_type) {
        
        std::string key = arg<Lpcstr>(receiver, 0);  

        if (node_type == NodeType::Server_TCP) { // TCP

            TCPServer *s = static_cast<TCPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_RM_INT) == 0) {

                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;

                }

            s->reg_del_int(key);

            }
        else if (node_type == NodeType::Server_UDP) { // UDP

            UDPServer *s = static_cast<UDPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_RM_INT) == 0) {

                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;

                }

            s->reg_del_int(key);

            }
        else {

            _RN_ASSERT(0);

            }

        }

    void HandlerMgr::req_reg_del_dbl(rn::Node *receiver, NodeType::Enum node_type) {
        
        std::string key = arg<Lpcstr>(receiver, 0);  

        if (node_type == NodeType::Server_TCP) { // TCP

            TCPServer *s = static_cast<TCPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_RM_DBL) == 0) {

                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;

                }

            s->reg_del_dbl(key);

            }
        else if (node_type == NodeType::Server_UDP) { // UDP

            UDPServer *s = static_cast<UDPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_RM_DBL) == 0) {

                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;

                }

            s->reg_del_dbl(key);

            }
        else {

            _RN_ASSERT(0);

            }

        }

    void HandlerMgr::req_reg_del_str(rn::Node *receiver, NodeType::Enum node_type) {
        
        std::string key = arg<Lpcstr>(receiver, 0);  

        if (node_type == NodeType::Server_TCP) { // TCP

            TCPServer *s = static_cast<TCPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_RM_STR) == 0) {

                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;

                }

            s->reg_del_str(key);

            }
        else if (node_type == NodeType::Server_UDP) { // UDP

            UDPServer *s = static_cast<UDPServer*>(receiver);

            size_t n = s->get_sender_index();

            auto perm = s->reg_get_cl_perm(key, n);

            if ((perm & REG_RM_STR) == 0) {

                s->queue_event( EventFactory::create_illegal_reg_req(n, key) );

                return;

                }

            s->reg_del_str(key);

            }
        else {

            _RN_ASSERT(0);

            }

        }

    void HandlerMgr::reg_clear_int(rn::Node *receiver, NodeType::Enum node_type) {
        
        _RN_ASSERT( _RN_IS_CLIENT(node_type) );

        receiver->my_reg.clear_int();

        }

    void HandlerMgr::reg_clear_dbl(rn::Node *receiver, NodeType::Enum node_type) {

        _RN_ASSERT( _RN_IS_CLIENT(node_type) );

        receiver->my_reg.clear_dbl();

        }

    void HandlerMgr::reg_clear_str(rn::Node *receiver, NodeType::Enum node_type) {

        _RN_ASSERT( _RN_IS_CLIENT(node_type) );

        receiver->my_reg.clear_str();

        }

    void HandlerMgr::reg_clear_all(rn::Node *receiver, NodeType::Enum node_type) {

        _RN_ASSERT( _RN_IS_CLIENT(node_type) );

        receiver->my_reg.clear_all();

        }

    void HandlerMgr::kick_order(rn::Node *receiver, NodeType::Enum node_type) {
    
        if (node_type == NodeType::Client_TCP) {
        
            receiver->queue_event( EventFactory::create_kicked() );

            static_cast<rn::TCPClient*>(receiver)->disconnect();

            }
        else if (node_type == NodeType::Client_UDP) {
            
            receiver->queue_event( EventFactory::create_kicked() );

            static_cast<rn::UDPClient*>(receiver)->disconnect();

            }
        else {
            
            _RN_ASSERT(0);

            }

        }

    // Todo
    void HandlerMgr::set_client_index(rn::Node *receiver, NodeType::Enum node_type) {
        
        Uint32 index = arg<Uint32>(receiver, 0);

        if (node_type == NodeType::Client_TCP) {

            static_cast<rn::TCPClient*>(receiver)->my_index = index;

            }
        else if (node_type == NodeType::Client_UDP) {

            _RN_ASSERT(0);
            //static_cast<rn::UDPClient*>(receiver)->my_index = index; STUB

            }
        else {
            
            _RN_ASSERT(0);
            
            }

        }

    } // End Rigel namespace

#undef _RN_IS_SERVER
#undef _RN_IS_CLIENT
#undef _RN_ASSERT