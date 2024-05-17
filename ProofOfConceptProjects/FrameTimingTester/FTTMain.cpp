// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Input.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Unicode.hpp>

#include <SPeMPE/SPeMPE.hpp>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace gr = hg::gr;
namespace spe = ::jbatnozic::spempe;
using namespace hg::in;

constexpr auto LOG_ID = "FTT";

#ifdef _MSC_VER
#define TICK_RATE  60
#define FRAME_RATE 120
#else
#define TICK_RATE  60
#define FRAME_RATE 60
#endif

void SimpleTest() {
    gr::RenderWindow window{
        hg::win::VideoMode{800, 800},
        "FTT (Simple)"
    };
    window.getView().setCenter({0.f, 0.f});
    window.setFramerateLimit(TICK_RATE);

    gr::CircleShape circle{32.f};
    circle.setOrigin({32.f, 32.f});
    circle.setFillColor(gr::COLOR_RED);

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            ev.visit([&](const hg::win::Event::Closed&) {
                window.close();
            });
        }

        const auto lr = (float)CheckPressedPK(PK_D) - (float)CheckPressedPK(PK_A);
        const auto ud = (float)CheckPressedPK(PK_S) - (float)CheckPressedPK(PK_W);
        const auto multiplier = 8.f;
        window.getView().move({-lr * multiplier, -ud * multiplier});

        window.clear();
        window.draw(circle);
        window.display();
    }
}

#define PRIORITY_EDITOR_DRIVER  1
#define PRIORITY_WINDOW_MANAGER 0

std::unique_ptr<spe::GameContext> CreateContex() {
    spe::GameContext::RuntimeConfig rtConfig{spe::TickRate{TICK_RATE}};
    auto                            ctx = std::make_unique<spe::GameContext>(rtConfig);

    // Add a WindowManager
    auto winMgr = std::make_unique<spe::DefaultWindowManager>(ctx->getQAORuntime().nonOwning(),
                                                              PRIORITY_WINDOW_MANAGER);
    // clang-format off
    spe::WindowManagerInterface::WindowConfig windowConfig{
        hg::win::VideoMode{800, 800},
        "FTT (SPeMPE)",
        hg::win::WindowStyle::Default
    };
    spe::WindowManagerInterface::MainRenderTextureConfig mrtConfig{
        /* SIZE */  {1024, 1024},
        /* SMOOTH*/ true
    };
    spe::WindowManagerInterface::TimingConfig timingConfig{
    #ifdef _MSC_VER
        spe::FrameRate{FRAME_RATE},
        spe::PREVENT_BUSY_WAIT_ON,
        spe::VSYNC_OFF
    #else
        FRAME_RATE,
        spe::PREVENT_BUSY_WAIT_OFF,
        spe::VSYNC_OFF
    #endif
    };
    // clang-format on
    winMgr->setToNormalMode(windowConfig, mrtConfig, timingConfig);
    winMgr->setMainRenderTextureDrawPosition(spe::WindowManagerInterface::DrawPosition::Fit);
    winMgr->setStopIfCloseClicked(true);
    winMgr->getView().setCenter({0.f, 0.f});

    ctx->attachAndOwnComponent(std::move(winMgr));

    return ctx;
}

class Driver : spe::NonstateObject {
public:
    Driver(hg::QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
        : spe::NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "Driver"} {}

    void _eventUpdate1() override {
        auto&       winMgr = ccomp<spe::WindowManagerInterface>();
        const auto& input = winMgr.getInput();

        const auto lr = (float)input.checkPressed(PK_D) - (float)input.checkPressed(PK_A);
        const auto ud = (float)input.checkPressed(PK_S) - (float)input.checkPressed(PK_W);
        const auto multiplier = 8.f;

        winMgr.getView().move({-lr * multiplier, -ud * multiplier});
    }

    void _eventDraw1() override {
        auto& winMgr = ccomp<spe::WindowManagerInterface>();

        gr::CircleShape circle{32.f};
        circle.setOrigin({32.f, 32.f});
        circle.setFillColor(gr::COLOR_RED);
        winMgr.getCanvas().draw(circle);
    }
};

void TestWithSPeMPE() {
    auto context = CreateContex();

    Driver driver{context->getQAORuntime().nonOwning(), PRIORITY_EDITOR_DRIVER};

    context->runFor(-1);
}

int main(int argc, char* argv[]) {
    SimpleTest();
    TestWithSPeMPE();
}
