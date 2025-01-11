// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/GridGoblin.hpp>

#include <GridGoblin/Rendering/Top_down_los_calculator.hpp>

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Input.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Log_with_scoped_stopwatch.hpp>
#include <Hobgoblin/Utility/Randomization.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>
#include <Hobgoblin/Window.hpp>

#include <chrono>

#include <Hobgoblin/Logging.hpp>

namespace jbatnozic {
namespace gridgoblin {
namespace {

#define LOG_ID "LoSTest"

namespace hg = jbatnozic::hobgoblin;

namespace {
void DrawChunk(hg::gr::Canvas& aCanvas, const World& aWorld, ChunkId aChunkId) {
    const auto  cellRes = aWorld.getCellResolution();
    const auto* chunk   = aWorld.getChunkAtId(aChunkId);
    if (chunk == nullptr) {
        return;
    }

    hg::math::Vector2f start{aChunkId.x * chunk->getCellCountX() * cellRes,
                             aChunkId.y * chunk->getCellCountY() * cellRes};

    hg::gr::RectangleShape rect{
        {cellRes, cellRes}
    };
    rect.setOutlineColor(hg::gr::COLOR_BLACK);
    rect.setOutlineThickness(1.f);

    for (hg::PZInteger y = 0; y < chunk->getCellCountY(); y += 1) {
        for (hg::PZInteger x = 0; x < chunk->getCellCountX(); x += 1) {
            rect.setPosition(start.x + x * cellRes, start.y + y * cellRes);
            const auto& cell = chunk->getCellAtUnchecked(x, y);
            if (cell.isWallInitialized()) {
                rect.setFillColor(hg::gr::COLOR_BLACK);
            } else {
                rect.setFillColor(hg::gr::COLOR_TRANSPARENT);
            }
            aCanvas.draw(rect);
        }
    }
}
} // namespace

// old: 28
#define CELL_COUNT_X     120
#define CELL_COUNT_Y     120
#define CELLRES          24.f
#define CELL_PROBABILITY 4

void RunLineOfSightTestImpl() {
    hg::log::SetMinimalLogSeverity(hg::log::Severity::Info);

    WorldConfig config{.chunkCountX                 = 1,
                       .chunkCountY                 = 1,
                       .cellsPerChunkX              = CELL_COUNT_X,
                       .cellsPerChunkY              = CELL_COUNT_Y,
                       .cellResolution              = CELLRES,
                       .maxCellOpenness             = 0,
                       .maxLoadedNonessentialChunks = 1};

    World world{config};

    // Generate world:
    {
        hg::util::DoWith32bitRNG([](std::mt19937& aRng) {
            // aRng.seed(hg::util::Generate32bitSeed());
            aRng.seed(0xDEADBEEF);
        });
        hg::util::DoWith64bitRNG([](std::mt19937_64& aRng) {
            // aRng.seed(hg::util::Generate64bitSeed());
            aRng.seed(hg::util::Generate64bitSeed());
            aRng.seed(0xDEADBEEFDEADBEEF);
        });

        auto perm = world.getPermissionToEdit();
        world.edit(*perm, [&world](World::Editor& aEditor) {
            for (int y = 0; y < world.getCellCountY(); y += 1) {
                for (int x = 0; x < world.getCellCountX(); x += 1) {
                    aEditor.setFloorAt({x, y}, CellModel::Floor{0});
                    if (hg::util::GetRandomNumber(0, 99) < CELL_PROBABILITY &&
                        x != CELL_COUNT_X / 2 - 1 && x != CELL_COUNT_X / 2) {
                        aEditor.setWallAt({x, y}, CellModel::Wall{0, 0, Shape::FULL_SQUARE});
                    }
                }
            }
        });
    }

    TopDownLineOfSightCalculator losCalc{world};
    HG_LOG_INFO(LOG_ID, "Running calc()...");
    {
        HG_LOG_WITH_SCOPED_STOPWATCH_MS(INFO, LOG_ID, "calc() took {}ms", elapsed_time_ms);
        losCalc.calc({CELL_COUNT_X * CELLRES * 0.5f, CELL_COUNT_Y * CELLRES * 0.5f},
                     {CELL_COUNT_X * CELLRES, CELL_COUNT_Y * CELLRES},
                     {CELL_COUNT_X * CELLRES * 0.5f, CELL_COUNT_Y * CELLRES * 0.5f});
    }
    HG_LOG_INFO(LOG_ID,
                "Triangles: {}, Comparisons: {}",
                losCalc.getTriangleCount(),
                losCalc.getTriangleComparisons());

    hg::gr::Image image;
    HG_LOG_INFO(LOG_ID, "Generating image...");
    {
        HG_LOG_WITH_SCOPED_STOPWATCH_MS(INFO, LOG_ID, "Image generation took {}ms", elapsed_time_ms);
        image.create(hg::ToPz(CELL_COUNT_X * CELLRES), hg::ToPz(CELL_COUNT_Y * CELLRES));
        for (int y = 0; y < hg::ToPz(CELL_COUNT_X * CELLRES); y += 1) {
            for (int x = 0; x < hg::ToPz(CELL_COUNT_Y * CELLRES); x += 1) {
                const auto v = losCalc.testVisibilityAt({(float)x, (float)y});
                if (!v.has_value() || *v == false) {
                    image.setPixel(x, y, hg::gr::COLOR_BLACK.withAlpha(150));
                } else {
                    image.setPixel(x, y, hg::gr::COLOR_TRANSPARENT);
                }
            }
        }
    }

    hg::gr::Texture texture;
    HG_LOG_INFO(LOG_ID, "Loading texture...");
    texture.loadFromImage(image);

    hg::gr::Sprite spr;
    spr.setTexture(&texture, true);

    hg::gr::RenderWindow window{
        hg::win::VideoMode{1280, 1280},
        "GridWorld"
    };
    window.setFramerateLimit(120);
    // window.setVerticalSyncEnabled(true);
    window.getView().setSize({CELL_COUNT_X * CELLRES, CELL_COUNT_Y * CELLRES});
    window.getView().setCenter({CELL_COUNT_X * CELLRES * 0.5f, CELL_COUNT_Y * CELLRES * 0.5f});

    hg::util::Stopwatch swatch;

    while (window.isOpen()) {
        const auto frameTime = swatch.restart<std::chrono::microseconds>();

        bool           mouseLClick = false;
        bool           mouseRClick = false;
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
                });
        } // end event processing

        {
            using namespace hg::in;
            const auto lr = (float)CheckPressedPK(PK_D) - (float)CheckPressedPK(PK_A);
            const auto ud = (float)CheckPressedPK(PK_S) - (float)CheckPressedPK(PK_W);
            // window.getView().move({lr * 1.f, ud * 1.f});
        }

        window.clear(hg::gr::Color{155, 155, 155});
        DrawChunk(window, world, {0, 0});
        window.draw(spr);

        // const auto t1 = std::chrono::steady_clock::now();
        // const auto t2 = std::chrono::steady_clock::now();
        // std::cout << "Time to render: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 -
        // t1).count() / 1000.0 << "ms "
        //           << "frame time: " << frameTime.count() / 1000.0 << "ms.\n";

        window.display();
    }
}

} // namespace
} // namespace gridgoblin
} // namespace jbatnozic

void RunLineOfSightTest() {
    jbatnozic::gridgoblin::RunLineOfSightTestImpl();
}
