
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Graphics.hpp>

#include <chrono>
#include <functional>
#include <thread>
#include <iostream>
#include <vector>

namespace gr = hg::gr;

namespace {

constexpr auto SAMPLE_SPRITE_FILE_PATH = "C:\\Users\\Jovan-PC\\Desktop\\Screenshot_1.png";

void CloseWindowIfCloseClickedOrEnterPressed(hg::win::Window& aWindow, const hg::win::Event& aEvent) {
    aEvent.visit(
        [&aWindow](hg::win::Event::Closed) {
        aWindow.close();
        },
        [&aWindow](hg::win::Event::KeyPressed aEvent) {
            if (aEvent.physicalKey == hg::in::PK_ENTER) {
                aWindow.close();
            }
        }
    );
}

void TestDrawingShapesAndSprites() {
    gr::RenderWindow window{};
    window.create(hg::win::VideoMode{400, 400}, "Window");
    window.setFramerateLimit(60);

    {
        window.setViewCount(2);

        window.setView(0, gr::View{{100.f, 200.f}, {200, 400}});
        window.getView(0).setViewport({0.f, 0.f, 0.5f, 1.f}); // left size
        window.getView(0).setEnabled(true);

        window.setView(1, gr::View{{300.f, 200.f}, {200, 400}});
        window.getView(1).setViewport({0.5f, 0.f, 0.5f, 1.f}); // right side
        window.getView(1).setEnabled(true);
    }

    gr::Texture texture;
    texture.loadFromFile(SAMPLE_SPRITE_FILE_PATH);

    gr::Sprite sprite{texture};
    sprite.setPosition(200.f, 200.f);

    gr::CircleShape circle{64.f};
    circle.setOrigin(64.f, 64.f);
    circle.setPosition(200.f, 200.f);
    circle.setFillColor(gr::COLOR_DARK_ORANGE.withAlpha(125));

    gr::RectangleShape rect{{64.f, 64.f}};
    rect.setPosition(0.f, 0.f);
    rect.setFillColor(gr::COLOR_CHARTREUSE.withAlpha(125));

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            CloseWindowIfCloseClickedOrEnterPressed(window, ev);
        }

        window.clear(gr::COLOR_BLACK);
        window.draw(sprite);
        window.draw(circle);
        window.draw(rect);
        window.display();

        sprite.rotate(hg::math::AngleF::fromDegrees(1.f));
    }
}

void TestDrawingMultisprites() {
    gr::RenderWindow window{};
    window.create(hg::win::VideoMode{400, 400}, "Window");
    window.setFramerateLimit(30);

    window.setView(gr::View{{100.f, 100.f}, {200, 200}});
    window.getView().setViewport({0.f, 0.f, 1.f, 1.f});
    window.getView().setEnabled(true);

    gr::Texture texture;
    texture.loadFromFile(SAMPLE_SPRITE_FILE_PATH);
    const auto texW = static_cast<std::uint16_t>(texture.getSize().x);
    const auto texH = static_cast<std::uint16_t>(texture.getSize().y);
    const auto halfTexW = static_cast<std::uint16_t>(texW / 2);

    const gr::MultispriteBlueprint blueprint{texture, std::vector<gr::TextureRect>{
        gr::TextureRect{0, 0, halfTexW, texH},
        gr::TextureRect{halfTexW, 0, halfTexW, texH},
    }};

    auto multisprite = blueprint.multispr();

    double theta = 0.0;
    while (window.isOpen()) {
        theta += 0.01;

        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            CloseWindowIfCloseClickedOrEnterPressed(window, ev);
        }

        window.clear(gr::COLOR_BLACK);
        multisprite.setScale(
            static_cast<float>(0.5 + std::abs(std::sin(theta) / 2.0)),
            static_cast<float>(0.5 + std::abs(std::sin(theta) / 2.0))
        );
        window.draw(multisprite);
        window.display();

        multisprite.advanceSubsprite(0.05f);
    }
}

void TestDrawingVertexArrays() {
    gr::RenderWindow window{};
    window.create(hg::win::VideoMode{400, 400}, "Window");
    window.setFramerateLimit(60);

    window.setView(gr::View{{100.f, 100.f}, {200, 200}});
    window.getView().setViewport({0.f, 0.f, 1.f, 1.f});
    window.getView().setEnabled(true);

    gr::VertexArray varr{};
    varr.primitiveType = gr::PrimitiveType::TriangleStrip;

    varr.vertices.push_back(gr::Vertex{{32.f, 32.f}, gr::COLOR_RED});
    varr.vertices.push_back(gr::Vertex{{32.f, 64.f}, gr::COLOR_YELLOW});
    varr.vertices.push_back(gr::Vertex{{64.f, 48.f}, gr::COLOR_GREEN});
    varr.vertices.push_back(gr::Vertex{{80.f, 128.f}, gr::COLOR_BLUE});
    varr.vertices.push_back(gr::Vertex{{128.f, 80.f}, gr::COLOR_PURPLE});
    varr.vertices.push_back(gr::Vertex{{196.f, 196.f}, gr::COLOR_LIME});

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            CloseWindowIfCloseClickedOrEnterPressed(window, ev);
        }

        window.clear(gr::COLOR_BLACK);
        window.draw(varr);
        window.display();
    }
}

void TestSpriteLoader() {
    gr::SpriteLoader loader;
    loader.startTexture(2048, 2048)
        ->addSprite("mons", "C:/Users/Jovan-PC/Desktop/Mons.png")
        ->addSprite("troidco", "C:/Users/Jovan-PC/Desktop/Troidco.png")
        ->finalize(gr::TexturePackingHeuristic::BestAreaFit);

    gr::RenderWindow window{};
    window.create(hg::win::VideoMode{800, 800}, "Window");
    window.setFramerateLimit(60);

    window.setView(gr::View{{512.f, 512.f}, {1024, 1024}});
    window.getView().setViewport({0.f, 0.f, 1.f, 1.f});
    window.getView().setEnabled(true);

    auto monsSpr = loader.getMultiBlueprint("mons").multispr();
    auto troidcoSpr = loader.getMultiBlueprint("troidco").multispr();
    troidcoSpr.move(200.f, 200.f);

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            CloseWindowIfCloseClickedOrEnterPressed(window, ev);
        }

        window.clear(gr::COLOR_BLACK);
        window.draw(monsSpr);
        window.draw(troidcoSpr);
        window.display();
    }
}

const std::vector<void(*)()> TESTS = {
    &TestDrawingShapesAndSprites,
    &TestDrawingMultisprites,
    &TestDrawingVertexArrays,
    &TestSpriteLoader
};

} // namespace

int main(int argc, char* argv[]) {
    for (const auto& test : TESTS) {
        test();
    }
    return EXIT_SUCCESS;
}
