#include <iostream>
#include <Windows.h> // Temp
#include <iomanip>
#include <ctime>

#include "Rigel-networking.hpp"
#include "Rigel-macro.hpp"

using namespace std;

struct TEST {
    
    enum Enum {
        
        print = 0,
        print_str = 1

        };

    };

void WinApiSleep(unsigned time_ms) {
    
    Sleep(time_ms);
    
    }

int random(int _max) {

    return std::lround( ((double)std::rand()/RAND_MAX)*(double)_max );

    }

void print(rn::Node *receiver, rn::NodeType::Enum node_type) {

    int num = rn::arg<rn::Int32>(receiver, 0);
    
    cout << "Received number: " << num << endl;

    }

void print_str(rn::Node *receiver, rn::NodeType::Enum node_type) {

    string str = rn::arg<rn::Lpcstr>(receiver, 0);

    cout << "Received string: " << str << endl;

    }

void print_str_2(rn::Node *receiver, rn::NodeType::Enum node_type) {

    string str1 = rn::arg<rn::Lpcstr>(receiver, 0);
    string str2 = rn::arg<rn::Lpcstr>(receiver, 1);

    cout << "Received string: " << str1 << " | " << str2 << endl;

    }

void hdl_packet(rn::Node *receiver, rn::NodeType::Enum node_type) {
    
    auto packet = rn::pkt(receiver);

    int   i;
    float f;
    
    packet >> i >> f;

    cout << "Received data: " << i << " | " << f <<endl;

    }

RN_STATIC_HANDLER_INIT{TEST::print, print};
RN_STATIC_HANDLER_INIT{TEST::print_str, print_str};
RN_STATIC_HANDLER_INIT{2, print_str_2};
RN_STATIC_HANDLER_INIT{3, hdl_packet};

void print_time() {
    
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::cout << std::put_time(&tm, "%H-%M-%S") << ": ";

    }

int main2(int argc, char **argv) {

    int choice = 5;
    rn::Port port;
    string ip;
    sf::Packet dummypack;

    srand((unsigned)time(0));

    rn::HandlerMgr::initialize();

    cout << "Input choice (1 = Server, 2 = Client, [Other] = Exit):" << endl;

    cin >> choice;

    if (choice == 2) {

        cout << "Input host IP and port (separated by a space):" << endl;

        cin >> ip;
        cin >> port;

        }

    //TCP Test:
    switch (choice) {

        case 1: { // SERVER

            rn::TCPServer server{4, "rigel_pp", 8888, 4, 5000};

            server.reg_set_int("Aa", 9);
            server.reg_set_dbl("Bb", 65.73);
            server.reg_set_str("Cc", "kgggdkp");

            int i = 0;

            while (1) {

                Sleep(10);

                i = (i + 1) % 500;

                if (GetAsyncKeyState(VK_RCONTROL) & 1) {

                    //server.send(-1, rn::Type::OnlyString, 2, RN_CONT_2("AaBbCcDd ... << SOMETHING >>", " ...  Bla, Bla, Bla!!!") );

                    cout << "Server sent something..." << endl;

                    //server.send(0, 3, sf::Packet() << (sf::Uint32) 64 << (float) 4.75f);

                    rn::Packet pkt;

                    pkt << (rn::Uint32)64 << (rn::Float)4.75f;

                    server.write<rn::Int32>(0, 0, {64});
                    server.write<RN_ANY>(0, 1, {"Some random string..."});
                    server.write(0, 3, pkt);

                    }

                if (GetAsyncKeyState('0') & 1) {
                    
                    server.reg_set_cl_perm("Aa", 0, rn::REG_WR_INT);
                    cout << "Authorized client 0 to change Registry key Aa (integers only).\n";

                    server.reg_set_cl_perm("Aa", 1, rn::REG_WR_INT);
                    cout << "Authorized client 1 to change Registry key Aa (integers only).\n";

                    server.reg_set_cl_perm("Aa", 2, rn::REG_WR_INT);
                    cout << "Authorized client 2 to change Registry key Aa (integers only).\n";

                    //server.client_kick(0);

                    //cout << "Kicked client 0.\n";
                    
                    }                

                if (GetAsyncKeyState(VK_RSHIFT) & 1) {

                    int r = random(100);

                    server.reg_set_int("rval", r);

                    cout << "Changed Random value to " << r << ".\n";

                    }

                if (i == 0) {

                    cout << "Ping = [" << server.get_client_latency(0) << ", " << server.get_client_latency(1) << ", " << server.get_client_latency(2) << "], ";
                    cout << "Random value = " << server.reg_get_int("rval") << endl;

                    }

                //UPDATE:
                server.update();

                //POLL EVENTS:
                rn::Event ev;

                while (server.poll_event(ev)) {

                    switch (ev.type) {

                        case rn::Event::Type::Connect: {
                            
                            int slot = ev.connect.client_index;

                            print_time();
                            cout << "EVENT: New client connected to slot " << slot << " (IP = " << server.get_client_ip(slot) << ").\n";

                            }
                            break;
                        
                        case rn::Event::Type::Disconnect: {

                            print_time();
                            cout << "EVENT: Client " << ev.disconnect.client_index << " has just disconnected.\n";

                            }
                            break;

                        case rn::Event::Type::ConnTimeout: {

                            print_time();
                            cout << "EVENT: Client " << ev.conn_timeout.client_index << " timed out.\n";

                            }
                            break;

                        case rn::Event::Type::IllegalRegReq: {
                            
                            print_time();
                            cout << "EVENT: Client " << ev.illegal_reg_req.client_index
                                 << " tried to change registry key " << ev.illegal_reg_req_key()
                                 << " illegally.\n";

                            }
                            break;

                        case rn::Event::Type::BadPassphrase: {
                        
                            print_time();
                            cout << "EVENT: Client " << ev.bad_passphrase.client_index
                                 << " disconnected (passphrases didn't match).\n";
                            cout << "[" << ev.bad_passphrase_str() << "].\n";

                            }
                            break;

                        default:
                            break;
                        
                        }

                    }

                }

            }
            break;

        case 2: { // CLIENT ///////////////////////////////////////////////////

            rn::TCPClient client{"rigel_pp", ip, port, 4, 5000};

            int i = 0;

            while (1) {

                Sleep(10);

                i = (i + 1) % 500;          

                if (GetAsyncKeyState(VK_LCONTROL) & 1) {
                    
                    int r = random(100);
                    
                    //client.write<rn::Int32>(0, {r});
                    //cout << "Sent number " << r << " to server.\n";

                    client.reg_req_set_int("Aa", r);
                    cout << "Request sent to server to set Registry key Aa to " << r << ".\n";

                    }

                if (i == 0) {

                    cout << "Ping = " << client.get_server_latency() << "ms, ";
                    cout << "Aa = "   << client.reg_get_int("Aa") << ", ";
                    cout << "Bb = "   << client.reg_get_dbl("Bb") << ", ";
                    cout << "Cc = "   << client.reg_get_str("Cc") << ", ";
                    cout << "Random value = " << client.reg_get_int("rval") << endl;

                    }

                //UPDATE:
                client.update();

                //POLL EVENTS:
                rn::Event ev;

                while (client.poll_event(ev)) {

                    switch (ev.type) {

                            case rn::Event::Type::Connect:

                                print_time();
                                cout << "EVENT: Connection to server established.\n";

                                break;

                            case rn::Event::Type::Disconnect:

                                print_time();
                                cout << "EVENT: Lost connection to server.\n";

                                break;

                            case rn::Event::Type::Kicked:

                                print_time();
                                cout << "EVENT: Kicked from the server.\n";

                                break;

                            case rn::Event::Type::AttempTimeout:

                                print_time();
                                cout << "EVENT: Connection attempt timed out after " << ev.attempt_timeout.time_ms << " milliseconds.\n";

                                break;

                            case rn::Event::Type::ConnTimeout:

                                print_time();
                                cout << "EVENT: Server timed out.\n";

                                break;

                            case rn::Event::Type::BadPassphrase:

                                print_time();
                                cout << "EVENT: Disconnected from server (passphrases didn't match).\n";
                                cout << "[" << ev.bad_passphrase_str() << "].\n";
                              
                                break;

                            default:
                                break;

                        }

                    }

                }

            }
            break;

        default:
            break;

        }

    return 0;

    }