
#include <Hobgoblin_include/RigelNet/handlers.hpp>
#include <Hobgoblin_include/RigelNet/udp_server.hpp>
#include <Hobgoblin_include/RigelNet/udp_client.hpp>

#include <Hobgoblin_include/RigelNet_Macros.hpp>

#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

using namespace jbatnozic::hobgoblin::rn;

RN_DEFINE_HANDLER(Foo, RN_ARGS()) {

}

RN_DEFINE_HANDLER(Bar, RN_ARGS(int, a)) {

}

RN_DEFINE_HANDLER(Baz, RN_ARGS(int, a, std::string&, s)) {

}

int main() {
    RN_IndexHandlers();

    RN_UdpServer server{1, 0, "pass"};
    RN_UdpClient client{0, "localhost", server.getLocalPort(), "pass"};

    for (int i = 0; i < 10; i += 1) {
        client.update();
        server.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;

    RN_Compose_Foo(server, 0);
    RN_Compose_Bar(server, 0, 1);
    RN_Compose_Baz(server, 0, 1, "asdf");
}