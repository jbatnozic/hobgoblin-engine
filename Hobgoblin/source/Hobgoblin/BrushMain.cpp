
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Graphics/Brush.hpp>
#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Views.hpp>

using namespace hg::gr;

static constexpr int S = sizeof(sf::RenderStates);

#define WINDOW_W 800
#define WINDOW_H 800

int main() {
    sf::RenderWindow window{sf::VideoMode(WINDOW_W, WINDOW_H), "Collision detection"};
    MultiViewRenderTargetAdapter adapter{window};
    Brush br{&adapter};

    adapter.setViewCount(2);
    adapter.getView(0).setSize({200, 200});
    adapter.getView(0).setCenter({200, 200});
    adapter.getView(0).setViewport({0, 0, 0.5f, 0.5f});

    adapter.getView(1).setSize({200, 200});
    adapter.getView(1).setCenter({200, 200});
    adapter.getView(1).setViewport({0.5f, 0.5f, 0.5f, 0.5f});

    while (window.isOpen()) {

        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }

        // clear -> draw -> display

        window.clear(sf::Color::Black);

        br.drawCircle(100, 100, 100);

        window.display();
    }

}