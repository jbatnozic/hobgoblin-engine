
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

}

RN_DEFINE_HANDLER(Baz, RN_ARGS(int, a, std::string&, s)) {
    std::cout << "Baz called\n";
    std::cout << "a = " << a << '\n';
    std::cout << "s = " << s << '\n';
}

// TODO - Test retransmit & stuff...
// TEST

int main() {
    RN_Event event_ = RN_EvConnectionTimedOut{5};
    std::visit(
        hg::util::MakeVisitor(
            [](const RN_EvBadPassphrase& ev) {
                std::cout << "Bad passphrase\n";
            },
            [](const RN_EvAttemptTimedOut& ev) {
                std::cout << "Attempt timed out\n";
            },
            [](const RN_EvConnected& ev) {
                std::cout << "Connected\n";
            },
            [](const RN_EvDisconnected& ev) {
                std::cout << "Disconnected\n";
            },
            [](const RN_EvConnectionTimedOut& ev) {
                std::cout << "Connection timed out\n";
            },
            [](const RN_EvKicked& ev) {
                std::cout << "Kicked\n";
            }
        ),
        event_
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

    // return 0;

    RN_Compose_Foo(server, 0);
    RN_Compose_Bar(server, 0, 1);
    RN_Compose_Baz(server, 0, 1, "asdf");

    return 0;
}