
#include <GridWorld/Coord_conversion.hpp>
#include <GridWorld/Isometric_renderer.hpp>

#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Window.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Input.hpp>

#include <array>
#include <iostream>

#include <Hobgoblin/Logging.hpp>

namespace hg = jbatnozic::hobgoblin;

void DrawIsometricSquareAt(hg::gr::Canvas& aCanvas, float aSize, hg::gr::Color aColor, hg::math::Vector2f aPosition) {
    const float coords[][2] = {
        {0.f, 0.f},
        {0.f, aSize},
        {aSize, aSize},
        {aSize, 0.f},
        {0.f, 0.f}
    };

    using hg::gr::Vertex;
    using hg::gr::VertexArray;

    VertexArray va;
    va.primitiveType = hg::gr::PrimitiveType::LineStrip;

    for (int i = 0; i < 5; i += 1) {
        auto iso = gridworld::ScreenCoordinatesToIsometric({coords[i][0] + aPosition.x, 
                                                            coords[i][1] + aPosition.y});
        va.vertices.push_back(Vertex{iso, aColor});
    }

    aCanvas.draw(va);
}

// Draws an isometric grid
void Func() {
    hg::gr::RenderTexture texture;
    texture.create({800, 800});
    texture.setView(hg::gr::View{{400.f, 0.f}, {800.f, 800.f}});
    texture.clear(hg::gr::COLOR_BLACK);

    static constexpr float size = 32.f;

    for (int i = 0; i < 10; i += 1) {
        for (int t = 0; t < 10; t += 1) {
            DrawIsometricSquareAt(texture, size, hg::gr::COLOR_RED, {(float)t * size, (float)i * size});
        }
    }
    DrawIsometricSquareAt(texture, size, hg::gr::COLOR_GREEN, {(float)0, (float)0});

    texture.display();
    texture.getTexture().copyToImage().saveToFile("iso.png");
}

#define SPR_STONE_TILE 0

int main() {
    hg::log::SetMinimalLogSeverity(hg::log::Severity::Info);

    hg::gr::SpriteLoader loader;
    loader.startTexture(1024, 1024)
        ->addSprite(SPR_STONE_TILE, HG_TEST_ASSET_DIR "/isometric-stone-tile.png")
        ->finalize(hg::gr::TexturePackingHeuristic::BestAreaFit);

    gridworld::World world{10, 20, 32.f};
    for (int y = 0; y < 10; y += 1) {
        for (int x = 0; x < 10; x += 1) {
            world.getCellAt(x, y).floor.spriteId = SPR_STONE_TILE;
        }
    }
    for (int y = 2; y < 20; y += 3) {
        world.getCellAtUnchecked(2, y).wall.emplace();
        world.getCellAtUnchecked(7, y).wall.emplace();
    }

    
    const auto light = world.createLight(-1, {300, 300});

    gridworld::IsometricRenderer renderer{world, loader};

    hg::gr::RenderWindow window{hg::win::VideoMode{800, 800}, "GridWorld"};
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            ev.visit(
                [&](hg::win::Event::Closed&) {
                    window.close();
                },
                [&](hg::win::Event::KeyPressed& aKey) {
                    switch (aKey.physicalKey) {
                    case hg::in::PK_LEFT:
                        window.getView().move({-16.f, 0.f});
                        break;
                    case hg::in::PK_RIGHT:
                        window.getView().move({+16.f, 0.f});
                        break;
                    case hg::in::PK_UP:
                        window.getView().move({0.f, -16.f});
                        break;
                    case hg::in::PK_DOWN:
                        window.getView().move({0.f, +16.f});
                        break;
                    }
            });
        } // end event processing

        window.clear(hg::gr::COLOR_BLACK);

        const auto mousePos  = hg::win::GetMousePositionRelativeToWindow(window);
        const auto isoCoords = gridworld::ScreenCoordinatesToIsometric(window.mapPixelToCoords(mousePos));
        world.updateLight(light, isoCoords, hg::math::AngleF::zero());
        const auto* lightTex = world._renderLight(light);

        renderer.start(window.getView(0));
        renderer.render(window);

        //static constexpr float MAGIC = 0.33432761f;
        static constexpr float MAGIC = 0.333333333f;

        hg::gr::Multisprite spr{&(lightTex->getTexture())};
        spr.addSubsprite({0, 0, 300, 300});

#if 0
        const auto origin = hg::math::Vector2f{(float)lightTex->getSize().x * MAGIC * 2, (float)lightTex->getSize().y * MAGIC * 2};
        spr.setOrigin(origin);
        spr.setPosition(gridworld::IsometricCoordinatesToScreen(isoCoords));
        spr.setRotation(hg::math::AngleF::fromDegrees(45.f));
        spr.setSkew({MAGIC, MAGIC}); // TODO - where does constant this come from???
        spr.setScale({1.06f, 1.06f});
#else
        const auto origin = hg::math::Vector2f{lightTex->getSize().x / 2.f, lightTex->getSize().y / 2.f};
        spr.setOrigin(origin);
        spr.setPosition(isoCoords);
#endif
        const hg::gr::Transform magicDimetricTransform = { // rotate by 45 degrees and squash! (when setting position do NOT transform it further!)
              1.f,   1.f, 0.f,
            -0.5f,  0.5f, 0.f,
              0.f,   0.f, 1.f
        };

        spr.setColor({255, 255, 255, 155});
        window.draw(spr, magicDimetricTransform);

        window.display();
    }

    world.destroyLight(light);

    return 0;
}
