
#include <GridWorld/Coord_conversion.hpp>
#include <GridWorld/Rendering/Dimetric_renderer.hpp>
#include <GridWorld/Dimetric_transform.hpp>
#include <GridWorld/Rendering/Lighting_renderer_2d.hpp>
#include <GridWorld/Rendering/Line_of_sight_renderer_2d.hpp>

#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Window.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Input.hpp>

#include <GL/glew.h>

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
#define SPR_WALL       1
#define SPR_WALL_SHORT 2
#define SPR_LIGHT      9

int main() try {
    hg::log::SetMinimalLogSeverity(hg::log::Severity::Info);

    hg::gr::SpriteLoader loader;
    loader.startTexture(1024, 1024)
        ->addSprite(SPR_STONE_TILE, (HG_TEST_ASSET_DIR "/isometric-stone-tile.png"))
        ->addSprite(SPR_WALL,       (HG_TEST_ASSET_DIR "/isometric-wall.png"))
        ->addSprite(SPR_WALL_SHORT, (HG_TEST_ASSET_DIR "/isometric-wall-short.png"))
        ->addSprite(SPR_LIGHT,      (HG_TEST_ASSET_DIR "/light.png"))
        ->finalize(hg::gr::TexturePackingHeuristic::BestAreaFit);

    gridworld::World world{40, 40, 32.f};
    for (int y = 0; y < 40; y += 1) {
        for (int x = 0; x < 40; x += 1) {
            world.updateCellAt(x, y, gridworld::model::Cell::Floor{SPR_STONE_TILE});
        }
    }
    for (int y = 2; y < 20; y += 3) {
        world.updateCellAt(2, y, gridworld::model::Cell::Wall{SPR_WALL, SPR_WALL_SHORT, gridworld::model::Shape::FULL_SQUARE});
        world.updateCellAt(7, y, gridworld::model::Cell::Wall{SPR_WALL, SPR_WALL_SHORT, gridworld::model::Shape::FULL_SQUARE});
    }

    const auto light = world.createLight(SPR_LIGHT, {300, 300});

    hg::gr::RenderWindow window{hg::win::VideoMode{1280, 950}, "GridWorld"};
    window.setFramerateLimit(60);
    window.getView().setSize({1280, 950});

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    gridworld::LightingRenderer2D tdlRenderer{world, loader, 1024, gridworld::LightingRenderer2D::FOR_DIMETRIC};
    gridworld::DimetricRenderer renderer{world, loader, tdlRenderer};

    gridworld::LineOfSightRenderer2D losRenderer{world, 1024, gridworld::LineOfSightRenderer2D::FOR_DIMETRIC};

    while (window.isOpen()) {
        bool mouseLClick = false;
        bool mouseRClick = false;
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            ev.visit(
                [&](hg::win::Event::Closed&) {
                    window.close();
                },
                [&](hg::win::Event::MouseButtonPressed& aButton) {
                    if (aButton.button == hg::in::MB_LEFT) {
                        mouseLClick = true;
                    }
                    if (aButton.button == hg::in::MB_RIGHT) {
                        mouseRClick = true;
                    }
                }
            );
        } // end event processing

        {
            using namespace hg::in;
            const auto lr = (float)CheckPressedPK(PK_D) - (float)CheckPressedPK(PK_A);
            const auto ud = (float)CheckPressedPK(PK_S) - (float)CheckPressedPK(PK_W);
            window.getView().move({lr * 16.f, ud * 16.f});
        }

        window.clear(hg::gr::Color{0, 0, 55});

        const auto mousePos  = hg::win::GetMousePositionRelativeToWindow(window);
        const auto isoCoords = gridworld::ScreenCoordinatesToIsometric(window.mapPixelToCoords(mousePos));
        world.updateLight(light, isoCoords, hg::math::AngleF::zero());

        {
            const auto xx = static_cast<int>(isoCoords.x / world.getCellResolution());
            const auto yy = static_cast<int>(isoCoords.y / world.getCellResolution());

            if (xx >= 0 && xx < world.getCellCountX() &&
                yy >= 0 && yy < world.getCellCountY()) {

                if (mouseLClick) {
                    world.updateCellAtUnchecked({xx, yy}, gridworld::model::Cell::Wall{
                        SPR_WALL, SPR_WALL_SHORT, gridworld::model::Shape::FULL_SQUARE
                    });
                } else if (mouseRClick) {
                    world.updateCellAtUnchecked({xx, yy}, std::optional<gridworld::model::Cell::Wall>{});
                }
            }
        }

        renderer.start(window.getView(0), /* POV */ isoCoords);
        losRenderer.start(window.getView(0).getCenter(), window.getView(0).getSize(), {100.f, 100.f}, 0.f);

        renderer.render(window);
        losRenderer.render();

        if (true) {
            hg::math::Vector2f scale;
            const auto& tex = losRenderer.__gwimpl_getTexture(&scale);
            hg::gr::Sprite spr2{&tex};
            spr2.setOrigin({tex.getSize().x * 0.5f, tex.getSize().y * 0.5f});
            spr2.setScale(scale);
            spr2.setPosition(gridworld::ScreenCoordinatesToIsometric(window.getView(0).getCenter()));
            spr2.setColor({255, 255, 255, 155});
            window.draw(spr2, gridworld::DIMETRIC_TRANSFORM);

            const auto visibility = losRenderer.testVisibilityAt(isoCoords);
            if (visibility) {
                std::cout << "Visibility: " << (*visibility ? "yes" : "no") << '\n';
            } else {
                std::cout << "Visibility: n/a" << '\n';
            }
        }

        window.display();
    }

    world.destroyLight(light);

    return 0;
}
catch (const std::exception& ex) {
    std::cout << "Exception caught: " << ex.what() << '\n';
}
