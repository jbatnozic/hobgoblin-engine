
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics.hpp>

#include <cstdlib>

namespace gr = hg::gr;

int main(int argc, char* argv[]) {
    gr::RenderWindow window{hg::win::VideoMode{800, 800}, "Antimony"};
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            ev.visit([&](const hg::win::Event::Closed&) {
                window.close();
            });
        }

        window.clear();
        window.display();
    }

    return EXIT_SUCCESS;
}
