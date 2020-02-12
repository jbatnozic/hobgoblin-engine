
#include "Rigel-networking.hpp"
#include "Rigel-macro.hpp"
#include "Rigel-udp-connector.hpp"
#include "Rigel-udp-server.hpp"
#include "Rigel-udp-client.hpp"

#include <iostream>

using std::cin;
using std::cout;
using std::endl;

void WinApiSleep(unsigned time_ms);

template<class T>
T input(std::istream &is) {
    
    T rv;
    is >> rv;
    return rv;

    }

void f(const std::string& text) {
    
    std::cout << text << "\n";

    }

int num_prev;

void hdl10(rn::Node * node, rn::NodeType::Enum type) {
    
    auto num = rn::arg<rn::Int32>(node, 0);

    if (num == num_prev + 1) {
        std::cout << "Hdl10 " << num << "\n";
        num_prev = num;
        }
    else {
        std::cout << "Ordering error!!! " << num << "\n";
        WinApiSleep(100000);
        exit(1);
        }

    }

RN_STATIC_HANDLER_INIT{10, hdl10};

int main(int argc, char **argv) {

    int choice = 5;
    rn::Port port;
    std::string ip;
    sf::Packet dummypack;
    num_prev = -1;

    rn::HandlerMgr::initialize();

    ///////////////////////////////////////////////////////////////////////////

    cout << "Input choice (1 = Server, 2 = Client, [Other] = Exit):" << endl;

    cin >> choice;

    if (choice == 2) {

        cout << "Input host IP and port (separated by a space):" << endl;

        cin >> ip;
        cin >> port;

        }

    int counter = 1;
    int msg_cnt = 0;

    if (choice == 1) { // Server

        rn::UDPServer server{3, "default"};
        
        server.start(8888u, 1, 5000);

        while (1) {

            WinApiSleep(100u);

            server.update();

            rn::Event ev;

            while (server.poll_event(ev)) {

                switch (ev.type) {

                    case rn::Event::Type::Connect: {
                    int slot = ev.connect.client_index;
                        cout << "EVENT: New client connected to slot " << slot << " (IP = " << server.get_client_ip(slot) << ").\n";
                        }
                        break;

                    case rn::Event::Type::Disconnect:
                        cout << "EVENT: Client " << ev.disconnect.client_index << " has just disconnected.\n";
                        break;

                    case rn::Event::Type::ConnTimeout:
                        cout << "EVENT: Client " << ev.conn_timeout.client_index << " timed out.\n";
                        break;

                    case rn::Event::Type::IllegalRegReq:
                        cout << "EVENT: Client " << ev.illegal_reg_req.client_index
                            << " tried to change registry key " << ev.illegal_reg_req_key()
                            << " illegally.\n";
                        break;

                    case rn::Event::Type::BadPassphrase: 
                        cout << "EVENT: Client " << ev.bad_passphrase.client_index
                            << " disconnected (passphrases didn't match).\n";
                        cout << "[" << ev.bad_passphrase_str() << "].\n";
                        break;

                    default:
                        break;

                    }

                }

            counter = (counter + 1) % 250;

            if (counter == 0) {
                
                std::cout << "Latency = " << server.get_client_latency(0) << "ms\n";
                }

            if (server.get_client_status(0) == true) {

                server.write_s<rn::Int32>(rn::ALL_CLIENTS, 10, {msg_cnt});

                msg_cnt += 1;

                }

            }

        }
    else if (choice == 2) { // Client
        
        rn::UDPClient client{"default"};

        client.connect(ip, port, 1, 5000);

        while (1) {

            WinApiSleep(100u);

            client.update();

            rn::Event ev;

            while (client.poll_event(ev)) {

                switch (ev.type) {

                    case rn::Event::Type::Connect:
                        cout << "EVENT: Connection to server established.\n";
                        break;

                    case rn::Event::Type::Disconnect:
                        cout << "EVENT: Lost connection to server.\n";
                        break;

                    case rn::Event::Type::Kicked:
                        cout << "EVENT: Kicked from the server.\n";
                        break;

                    case rn::Event::Type::AttempTimeout:
                        cout << "EVENT: Connection attempt timed out after " << ev.attempt_timeout.time_ms << " milliseconds.\n";
                        break;

                    case rn::Event::Type::ConnTimeout:
                        cout << "EVENT: Server timed out.\n";
                        break;

                    case rn::Event::Type::BadPassphrase:
                        cout << "EVENT: Disconnected from server (passphrases didn't match).\n";
                        cout << "[" << ev.bad_passphrase_str() << "].\n";
                        break;

                    default:
                        break;

                    }

                }

            counter = (counter + 1) % 250;

            if (counter == 0) {
                
                std::cout << "Latency = " << client.get_server_latency() << "ms\n";

                }

            if (client.is_ready()) {

                client.write_s<rn::Int32>(10, {msg_cnt});

                msg_cnt += 1;

                }

            }

        }

    return EXIT_SUCCESS;

    }
