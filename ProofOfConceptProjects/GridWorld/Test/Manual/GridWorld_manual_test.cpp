
#include <GridWorld/Coord_conversion.hpp>
#include <GridWorld/Rendering/Dimetric_renderer.hpp>
#include <GridWorld/Rendering/Lighting_renderer_2d.hpp>

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
    for (int y = 0; y < 20; y += 1) {
        for (int x = 0; x < 10; x += 1) {
            world.updateCellAt(x, y, gridworld::model::Cell::Floor{SPR_STONE_TILE});
        }
    }
    for (int y = 2; y < 20; y += 3) {
        world.updateCellAt(2, y, gridworld::model::Cell::Wall{});
        world.updateCellAt(7, y, gridworld::model::Cell::Wall{});
    }

    const auto light = world.createLight(-1, {300, 300});

    gridworld::LightingRenderer2D tdlRenderer{world, loader, 2048, gridworld::LightingRenderer2D::FOR_DIMETRIC};
    gridworld::DimetricRenderer renderer{world, loader};

    hg::gr::RenderWindow window{hg::win::VideoMode{950, 950}, "GridWorld"};
    window.setFramerateLimit(60);
    window.getView().setSize({1024, 1024});

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

        window.clear(hg::gr::Color{0, 0, 55});

        const auto mousePos  = hg::win::GetMousePositionRelativeToWindow(window);
        const auto isoCoords = gridworld::ScreenCoordinatesToIsometric(window.mapPixelToCoords(mousePos));
        world.updateLight(light, isoCoords, hg::math::AngleF::zero());
        //const auto* lightTex = world._renderLight(light);

        tdlRenderer.start(gridworld::ScreenCoordinatesToIsometric(window.getView(0).getCenter()),
                          window.getView(0).getSize(),
                          256.f);
        renderer.start(window.getView(0), {});

        tdlRenderer.render();
        renderer.render(window);

        const hg::gr::Transform magicDimetricTransform = { // rotate by 45 degrees and squash! (when setting position do NOT transform it further!)
                  1.f,   1.f, 0.f,
                -0.5f,  0.5f, 0.f,
                  0.f,   0.f, 1.f
            };

        // Draw single light
#if 0
        {
            hg::gr::Multisprite spr{&(lightTex->getTexture())};
            spr.addSubsprite({0, 0, 300, 300});

            const auto origin = hg::math::Vector2f{lightTex->getSize().x / 2.f, lightTex->getSize().y / 2.f};
            spr.setOrigin(origin);
            spr.setPosition(isoCoords);

            spr.setColor({255, 255, 255, 155});
            window.draw(spr, magicDimetricTransform);
        }
#endif

        //{
        //    hg::gr::Sprite spr2{&(tdlRenderer.getTexture())};
        //    spr2.setScale({0.25f, 0.25f});
        //    spr2.setPosition(window.getView().getCenter());
        //    window.draw(spr2);
        //}

        {
            //const float scale = 2.f;
            const hg::gr::Transform scaleTransform = {
                2.f, 0.f, 0.f,
                0.f, 2.f, 0.f,
                0.f, 0.f, 1.f
            };

            hg::math::Vector2f tdlScale;
            const auto& tex = tdlRenderer.getTexture(&tdlScale);
            hg::gr::Sprite spr2{&tex};
            spr2.setOrigin({tex.getSize().x * 0.5f, tex.getSize().y * 0.5f});
            spr2.setScale(tdlScale);
            spr2.setPosition(gridworld::ScreenCoordinatesToIsometric(window.getView().getCenter()));
            spr2.setColor({255, 255, 255, 155});
            window.draw(spr2, magicDimetricTransform);
            //window.draw(spr2);
        }

        window.display();
    }

    world.destroyLight(light);

    return 0;
}