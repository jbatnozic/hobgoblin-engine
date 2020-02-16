
#include <Hobgoblin_include/RigelNet/udp_server.hpp>
#include <Hobgoblin_include/RigelNet/udp_client.hpp>

#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

using namespace jbatnozic::hobgoblin::rn;

int main() {
    RN_UdpServer server{1, 0, "pass"};
    RN_UdpClient client{0, "localhost", server.getLocalPort(), "pass"};

    for (int i = 0; i < 10; i += 1) {
        client.update();
        server.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}