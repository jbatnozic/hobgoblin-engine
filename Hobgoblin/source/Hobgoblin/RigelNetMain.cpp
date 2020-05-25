
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/RigelNet/handlermgmt.hpp>
#include <Hobgoblin/RigelNet/Udp_server.hpp>
#include <Hobgoblin/RigelNet/Udp_client.hpp>
#include <Hobgoblin/RigelNet/Event.hpp>

#include <Hobgoblin/RigelNet_Macros.hpp>

#include <Hobgoblin/Utility/Time_utils.hpp>
#include <Hobgoblin/Utility/Visitor.hpp>

#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

using namespace hg::rn;

RN_DEFINE_HANDLER(Foo, RN_ARGS()) {
    RN_NODE_IN_HANDLER().visit(
        [](RN_UdpServer& server) {
            std::cout << "Foo called on server\n";
        },
        [](RN_UdpClient& client) {
            std::cout << "Foo called on client\n";
        }
    );
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

//#define DISTRIBUTED_TEST

int main() {
#if !defined(DISTRIBUTED_TEST)
    hg::util::Stopwatch stopwatch;
    auto t1 = stopwatch.getElapsedTime();
    auto t2 = stopwatch.getElapsedTime();
    assert(t2 >= t1);

    RN_IndexHandlers();

    RN_UdpServer server{2, 0, "pass"}; // Crashes when clients > size
    RN_UdpClient client1{0, "localhost", server.getLocalPort(), "pass"};
    RN_UdpClient client2{0, "localhost", server.getLocalPort(), "pass"};

    for (int i = 0; i < 10; i += 1) {
        Compose_Baz(client1, 0, i, "Beetlejuice!");
        Compose_Baz(client2, 0, i, "Beetlejuice!");
        client1.update(RN_UpdateMode::Receive);
        client2.update(RN_UpdateMode::Receive);
        server.update(RN_UpdateMode::Receive);

        client1.update(RN_UpdateMode::Send);
        client2.update(RN_UpdateMode::Send);
        server.update(RN_UpdateMode::Send);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));  
    }

    Compose_Foo(server, 0);
    Compose_Bar(server, 0, 1);
    Compose_Baz(server, 0, 1, "asdf");

#else 
    RN_IndexHandlers();

    int mode;
    std::cout << "1 = server, other = client\n";
    std::cin >> mode;

    std::vector<hg::PZInteger> rec = {0};

    if (mode == 1) { 
    // Server /////////////////////////
        RN_UdpServer server{2, 8888, "pass"};
        server.setTimeoutLimit(std::chrono::microseconds{2'000'000});

        for (int i = 0; ; i += 1) {
            if (server.getClient(0).getStatus() != RN_ConnectorStatus::Connected) {
                i = -1;
            }

            server.update(RN_UpdateMode::Receive);

            // step();
            if (server.getClient(0).getStatus() == RN_ConnectorStatus::Connected) {
                Compose_Foo(server, rec);
            }

            server.update(RN_UpdateMode::Send);

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::cout << "Latency = " << server.getClient(0).getRemoteInfo().latency.count() 
                      << "us; SBS = " << server.getClient(0).getSendBufferSize() << '\n';
        }
    }
    else {
    // Client /////////////////////////
        RN_UdpClient client{9999, "localhost", 8888, "pass"};
        client.setTimeoutLimit(std::chrono::microseconds{2'000'000});

        for (int i = 0; ; i += 1) {
            if (client.getServer().getStatus() != RN_ConnectorStatus::Connected) {
                i = -1;
            }

            client.update(RN_UpdateMode::Receive);

            // step();
            if (client.getServer().getStatus() == RN_ConnectorStatus::Connected) {
                Compose_Foo(client, 0);
            }

            client.update(RN_UpdateMode::Send);

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::cout << "Latency = " << client.getServer().getRemoteInfo().latency.count() 
                      << "us; SBS = " << client.getServer().getSendBufferSize() << '\n';
        }
    }
#endif
    return EXIT_SUCCESS;
}
