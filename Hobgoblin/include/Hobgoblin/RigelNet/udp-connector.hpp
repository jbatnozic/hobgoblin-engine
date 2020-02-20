#pragma once

#include "Rigel-SFML-networking.hpp"
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include "Rigel-node.hpp"
#include "Rigel-handlers.hpp"
#include "Rigel-registry.hpp"

#include <deque>
#include <vector>
#include <initializer_list>

namespace RIGELNETW_CONFIG_NS_NAME {
    
    struct UDPMsgType {
        
        enum Enum {

            Hello      = 110,
            Connect    = 111,
            Disconnect = 112,
            Data       = 113,
            UnsafeData = 114
            
            };

        };

    struct TaggedPacket {
        
        Packet packet;
        bool   tag; // True = has data
        sf::Clock clock;

        TaggedPacket()
            : packet()
            , tag(false)
            , clock()
            { }
        
        };

    ///////////////////////////////////////////////////////////////////////////

    class UDPConnector {
        
        friend class HandlerMgr;

    public:

        enum StateEnum {
            
            Idle,
            Accepting,
            Connecting,
            Connected
            
            };

        sf::Time latency;
        bool waiting_for_ping_back;

        sf::Clock timeout_clock; // prod_clock
        sf::Clock ping_clock;

        UDPConnector() = delete;
        UDPConnector(sf::UdpSocket * sock, const std::string & passphrase);

        void reset(unsigned interval = DEFAULT_INTERVAL, unsigned timeout_ms = NO_TIMEOUT);

        void disconnect(bool notify_remote);

        void accept(IpAddress addr, Port port);

        void connect(IpAddress ip, Port port, unsigned interval = DEFAULT_INTERVAL, unsigned timeout_ms = NO_TIMEOUT);

        void update(Node * node, size_t slot_index, bool skip_send = false);

        void packet_recieved(Packet & pack, Node * node, NodeType::Enum type, size_t slot_index);

        void unpack_exec(Node * node, NodeType::Enum type);

        // Utility:
        StateEnum get_state() const;

        IpAddress get_remote_ip() const;

        Port get_remote_port() const;

        void change_socket(sf::UdpSocket * sock);

        // Sending:
        template<class TypeToSend>
        void write(HdlInd fn_ind, std::initializer_list<ArgWrap<TypeToSend>> handler_args);

        void write(HdlInd fn_ind);

        void write(HdlInd fn_ind, const Packet &pack);

        // Sending, secure versions:
        template<class TypeToSend>
        void write_s(HdlInd fn_ind, std::initializer_list<ArgWrap<TypeToSend>> handler_args);

        void write_s(HdlInd fn_ind);

        void write_s(HdlInd fn_ind, const Packet &pack);

    private:

        StateEnum state;

        unsigned interval;
        unsigned interval_cnt;

        unsigned timeout_ms;

        unsigned update_ordinal;
        unsigned update_counter;

        std::string passphrase;

        IpAddress remote_ip;
        Port remote_port;

        sf::UdpSocket * socket; // Doesn't own the socket, only uses it
        
        Packet unsafe_send_packet; // Needs to be a deque
        std::deque<Packet> unsafe_recv_buffer;
        size_t unsafe_send_ord;
        size_t unsafe_recv_ord;

        std::vector<Packet*> write_dummy_vec;

        std::deque<TaggedPacket> send_buffer;
        std::deque<TaggedPacket> recv_buffer;
        size_t send_buffer_head_index;
        size_t recv_buffer_head_index;

        std::vector<Uint32> ack_vector;

        void upload_all();
        void prepare_ack(Uint32 ordinal);
        void ack_received(Uint32 ordinal);
        void handle_data(Packet & pack, Node * node, NodeType::Enum type);
        void handle_unsafe_data(Packet & pack, Node * node, NodeType::Enum type);
        void initialize_session();
        bool connection_timed_out();

        };

    template <class TypeToSend>
    void UDPConnector::write(const HdlInd fn_ind, std::initializer_list<ArgWrap<TypeToSend>> handler_args) {

        if (state != Connected) return;

        MsgType basic_msg_type = RIGELNETW_ENUMERATE_TYPE(TypeToSend);

        write_dummy_vec[0] = &unsafe_send_packet;

        // TEMP - Better assert
        if (handler_args.size() > RIGELNETW_CONFIG_MAX_ARGS) throw std::logic_error("UDPConnector::write - Trying to send too many arguments.");

        // Append mesages to all packets in the vector:
        Node::append_message_to_packets<TypeToSend>(write_dummy_vec, 1, fn_ind, basic_msg_type, handler_args); 

        }

    template <class TypeToSend>
    void UDPConnector::write_s(const HdlInd fn_ind, std::initializer_list<ArgWrap<TypeToSend>> handler_args) {

        //return;

        if (state != Connected) return;

        MsgType basic_msg_type = RIGELNETW_ENUMERATE_TYPE(TypeToSend);

        write_dummy_vec[0] = &(send_buffer.back().packet);

        // TEMP - Better assert
        if (handler_args.size() > RIGELNETW_CONFIG_MAX_ARGS) throw std::logic_error("UDPConnector::write_s - Trying to send too many arguments.");

        // Append mesages to all packets in the vector:
        Node::append_message_to_packets<TypeToSend>(write_dummy_vec, 1, fn_ind, basic_msg_type, handler_args); 

        }
    
    }