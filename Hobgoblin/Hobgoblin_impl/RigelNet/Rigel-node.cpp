
//#include <iostream> // Temp!

#include <string>
#include <memory>

#include "Rigel-config.hpp"
#include "Rigel-types.hpp"
#include "Rigel-node.hpp"
#include "Rigel-registry.hpp"
#include "Rigel-event.hpp"

namespace RIGELNETW_CONFIG_NS_NAME {

    Node::Node()
        : my_reg()
        , events()
        { }

    Node::~Node() { }
    
    bool Node::unpack_exec(Packet &packet, NodeType::Enum node_type) {
        
        MsgType    msg_type;
        HdlInd     fn_index;
        ArgCnt     argcount;
        TypeVec    typevec;
        bool       is_string;

        //Main parsing loop:
        while (packet.endOfPacket() == false) {

            //Determine type:
            packet >> msg_type;
            packet >> fn_index;

            if (msg_type != Type::Packet) {

                packet >> argcount;

                //See if there are strings in the packet:
                if (Type::message_has_strings(msg_type)) {

                    packet >> typevec;
                    msg_type -= Type::String;

                    }
                else {

                    typevec = 0;

                    }

                //Extract arguments:
                for (int i = 0; i < argcount; i += 1) {

                    is_string = typevec & 1;
                    typevec >>= 1;

                    if (is_string) {

                        packet >> argrecv_vec[i].string;

                        }
                    else {

                        switch (msg_type) {

                                case Type::Int8:
                                    packet >> argrecv_vec[i].number.d_int8;
                                    break;

                                case Type::Uint8:
                                    packet >> argrecv_vec[i].number.d_uint8;
                                    break;

                                case Type::Int16:
                                    packet >> argrecv_vec[i].number.d_int16;
                                    break;

                                case Type::Uint16:
                                    packet >> argrecv_vec[i].number.d_uint16;
                                    break;

                                case Type::Int32:
                                    packet >> argrecv_vec[i].number.d_int32;
                                    break;

                                case Type::Uint32:
                                    packet >> argrecv_vec[i].number.d_uint32;
                                    break;

                                case Type::Int64:
                                    packet >> argrecv_vec[i].number.d_int64;
                                    break;

                                case Type::Uint64:
                                    packet >> argrecv_vec[i].number.d_uint64;
                                    break;

                                case Type::Float:
                                    packet >> argrecv_vec[i].number.d_float;
                                    break;

                                case Type::Double:
                                    packet >> argrecv_vec[i].number.d_double;
                                    break;

                                default:
                                    break;		

                            }

                        }

                    }

                //Try to execute:
                Handler f = HandlerMgr::handler_get(fn_index);

                if (f != nullptr) {

                    try {
                        f(this, node_type);
                        }
                    catch(...) {
                        // STUB
                        // queue_event( Event(Event::Type::HandlerExcept, "Handler threw exception", 0, fn_index) );
                        return false;
                        }

                    }
                else {

                    // STUB
                    // queue_event( Event(Event::Type::HandlerExcept, "Invalid handler", 0, fn_index) );
                    return false;

                    }

                }
            else {

                PktSize pack_size;
                size_t  cnt;
                Uint64  uint64;
                Uint8   uint8;

                packet >> pack_size;

                user_packet.clear();

                size_t limit = (pack_size / sizeof(uint64)) * sizeof(uint64);

                for (cnt = 0; cnt < limit; cnt += sizeof(uint64)) {
                    
                    packet >> uint64;
                    user_packet << uint64;

                    }

                while (cnt < pack_size) {
                    
                    packet >> uint8;
                    user_packet << uint8;

                    cnt += 1;

                    }

                //Try to execute:
                Handler f = HandlerMgr::handler_get(fn_index);

                if (f != nullptr) {

                    try {
                        f(this, node_type);
                        }
                    catch(...) {
                        // STUB
                        // queue_event( Event(Event::Type::HandlerExcept, "Handler threw exception", 0, fn_index) );
                        return false;
                        }

                    }
                else {

                    // STUB
                    // queue_event( Event(Event::Type::HandlerExcept, "Invalid handler", 0, fn_index) );
                    return false;

                    }

                user_packet.clear();

                }

            } // End while

            return true;

        }
    
    //Registry
    bool Node::reg_int_exists(const std::string& key) const {
    
        return my_reg.int_exists(key);

        }

    bool Node::reg_dbl_exists(const std::string& key) const {
    
        return my_reg.dbl_exists(key);

        }

    bool Node::reg_str_exists(const std::string& key) const {
    
        return my_reg.str_exists(key);

        }

    Int32  Node::reg_get_int(const std::string& key) const {
    
        return my_reg.get_int(key);

        }

    Double Node::reg_get_dbl(const std::string& key) const {
    
        return my_reg.get_dbl(key);

        }

    std::string Node::reg_get_str(const std::string& key) const {
    
        return my_reg.get_str(key);

        }

    bool Node::reg_sget_int(const std::string& key, Int32  &val) const {
        
        return my_reg.sget_int(key, val);

        }

    bool Node::reg_sget_dbl(const std::string& key, Double &val) const {
        
        return my_reg.sget_dbl(key, val);

        }

    bool Node::reg_sget_str(const std::string& key, std::string &val) const {
        
        return my_reg.sget_str(key, val);

        }

    //Event queue
    void Node::clear_events() {
    
        events.clear();
    
        }

    void Node::queue_event(Event ev) {
    
        events.push_back(ev);

        }

    bool Node::poll_event(Event &event_) {
    
        if (events.empty()) {
        
            return false;
        
            }
        else {
        
            event_ = events.front();

            events.pop_front();

            return true;
        
            }
    
        }

    }