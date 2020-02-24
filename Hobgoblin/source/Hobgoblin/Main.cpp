
#include <Hobgoblin/RigelNet/handlermgmt.hpp>
#include <Hobgoblin/RigelNet/Udp_server.hpp>
#include <Hobgoblin/RigelNet/Udp_client.hpp>
#include <Hobgoblin/RigelNet/Event.hpp>

#include <Hobgoblin/RigelNet_Macros.hpp>

#include <Hobgoblin/Utility/Stopwatch.hpp>
#include <Hobgoblin/Utility/Visitor.hpp>

#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

namespace hg = jbatnozic::hobgoblin;
using namespace hg::rn;

RN_DEFINE_HANDLER(Foo, RN_ARGS()) {
    std::cout << "Foo called\n";
}

RN_DEFINE_HANDLER(Bar, RN_ARGS(int, a)) {
    std::cout << "Bar called (" << a << ")\n";
}

RN_DEFINE_HANDLER(Baz, RN_ARGS(int, a, std::string&, s)) {
    std::cout << "Baz called\n";
    std::cout << "a = " << a << '\n';
    std::cout << "s = " << s << '\n';
}

// TODO - Test retransmit & stuff...
// TEST

#define DISTRIBUTED_TEST

int main() {
#if !defined(DISTRIBUTED_TEST)
    RN_Event event{RN_Event::ConnectionTimedOut{5}};

    event.visit(
        [](const RN_Event::BadPassphrase& ev) {
            std::cout << "Bad passphrase\n";
        },
        [](const RN_Event::AttemptTimedOut& ev) {
            std::cout << "Attempt timed out\n";
        },
        [](const RN_Event::Connected& ev) {
            std::cout << "Connected\n";
        },
        [](const RN_Event::Disconnected& ev) {
            std::cout << "Disconnected\n";
        },
        [](const RN_Event::ConnectionTimedOut& ev) {
            std::cout << "Connection timed out\n";
        },
        [](const RN_Event::Kicked& ev) {
            std::cout << "Kicked\n";
        }
    );

    std::exit(0);

    hg::util::Stopwatch stopwatch;
    auto t1 = stopwatch.getElapsedTime();
    auto t2 = stopwatch.getElapsedTime();
    assert(t2 >= t1);

    RN_IndexHandlers();

    RN_UdpServer server{2, 0, "pass"}; // Crashes when clients > size
    RN_UdpClient client1{0, "localhost", server.getLocalPort(), "pass"};
    RN_UdpClient client2{0, "localhost", server.getLocalPort(), "pass"};

    for (int i = 0; i < 10; i += 1) {
        RN_Compose_Baz(client1, 0, i, "Beetlejuice!");
        RN_Compose_Baz(client2, 0, i, "Beetlejuice!");
        client1.update();
        client2.update();
        server.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));  
    }

    RN_Compose_Foo(server, 0);
    RN_Compose_Bar(server, 0, 1);
    RN_Compose_Baz(server, 0, 1, "asdf");

    return 0;
#else 
    RN_IndexHandlers();

    int mode;
    std::cout << "1 = server, other = client\n";
    std::cin >> mode;

    if (mode == 1) { 
    // Server /////////////////////////
        RN_UdpServer server{2, 8888, "pass"};

        for (int i = 0; ; i += 1) {
            if (server.getConnectorStatus(0) == RN_ConnectorStatus::Connected) {
                //RN_Compose_Bar(server, 0, i);
            }
            server.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::cout << "\r Latency = " << server.getClientInfo(0).latency.count() << " microseconds";
        }
    }
    else {
    // Client /////////////////////////
        RN_UdpClient client{9999, "localhost", 8888, "pass"};

        for (int i = 0; ; i += 1) {
            if (client .getConnectorStatus() == RN_ConnectorStatus::Connected) {
                //RN_Compose_Bar(client, 0, i);
            }
            client.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::cout << "\r Latency = " << client.getServerInfo().latency.count() << " microseconds";
        }
    }

#endif
}