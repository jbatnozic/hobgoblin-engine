// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Unicode.hpp>
#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Graphics.hpp>

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

constexpr auto LOG_ID = "Antimony";

#define VIEW_GUI   0
#define VIEW_WORLD 1

class MasterLoader {
public:
    MasterLoader(const std::filesystem::path& aDirectory,
                 const std::string&           aBaseSpriteName,
                 hg::PZInteger                aSpriteCount) {
        for (hg::PZInteger i = 0; i < aSpriteCount; i += 1) {
            std::filesystem::path specificSpriteName = {aBaseSpriteName + std::to_string(i) + ".png"};
            const auto&           texture = _loader.startTexture(2048, 2048)
                                      ->addSprite(i, aDirectory / specificSpriteName)
                                      ->finalize(gr::TexturePackingHeuristic::BestAreaFit);
            _textures.push_back(&texture);
        }
    }

    gr::SpriteBlueprint getMasterBlueprint(hg::PZInteger aIndex) const {
        return _loader.getBlueprint(aIndex);
    }

private:
    static constexpr gr::SpriteIdNumerical SPRITE_ID = 0;

    gr::SpriteLoader _loader;
    std::vector<const gr::Texture*> _textures;
};

class Editor {
public:
    Editor(const MasterLoader& aMasterLoader, hg::PZInteger aMasterCount, hg::PZInteger aLerpCount)
        : _masterLoader{aMasterLoader}
    {
        _offsets.resize(static_cast<std::size_t>(aMasterCount * (aLerpCount + 1)));
    }

    void onLeftClick(hg::math::Vector2f aWorldPosition) {
        _origin = aWorldPosition;
    }

    void draw(spe::WindowManagerInterface& aWinMgr) {
        auto& canvas = aWinMgr.getCanvas();

        // Draw master
        const auto blueprint = _masterLoader.getMasterBlueprint(_stage);
        auto sprite = blueprint.spr();
        canvas.draw(sprite);

        // Draw origin

    }

    void drawGui(spe::WindowManagerInterface& aWinMgr) {
        auto& canvas = aWinMgr.getCanvas();

        const auto string = hg::UFormat(
            HG_UNIFMT("Stage: {} / {} \n"
                      "Origin: {}, {}"),
            _stage,
            _substage,
            (_origin) ? _origin->x : 0.f,
            (_origin) ? _origin->y : 0.f
        );

        const auto& font = gr::BuiltInFonts::getFont(gr::BuiltInFonts::TITILLIUM_REGULAR);
        gr::Text text{font, string};
        text.setPosition({32.f, 32.f});
        text.setFillColor(gr::COLOR_RED);
        text.setStyle(gr::Text::REGULAR);
        canvas.draw(text);

        if (_origin.has_value()) {
            _drawCrosshairs(aWinMgr, *_origin, gr::COLOR_RED);
        }
    }

private:
    const MasterLoader& _masterLoader;

    std::optional<hg::math::Vector2f> _origin;
    std::vector<std::optional<hg::math::Vector2f>> _offsets;

    enum class Substage {
        PLACE_GROIN,
        PLACE_LEFT_ANKLE,
        PLACE_RIGHT_ANKLE,

        SUBSTAGE_COUNT
    };

    int _stage = 0;
    int _substage = 0;

    void _previousStage() {

    }

    void _nextStage() {

    }

    void _drawCrosshairs(spe::WindowManagerInterface& aWinMgr,
                         hg::math::Vector2f           aCenter,
                         gr::Color                    aColor) {
        auto& canvas = aWinMgr.getCanvas();
        const auto screenPos = aWinMgr.mapCoordsToPixel({aCenter.x, aCenter.y});

        gr::RectangleShape rect;
        rect.setFillColor(aColor);

        // Horizontal
        rect.setSize({(float)aWinMgr.getWindowSize().x, 1.f});
        rect.setPosition({0.f, (float)screenPos.y});
        canvas.draw(rect);

        // Vertical
        rect.setSize({1.f, (float)aWinMgr.getWindowSize().y});
        rect.setPosition({(float)screenPos.x, 1.f});
        canvas.draw(rect);
    }
};

class EditorDriver : public spe::NonstateObject {
public:
    EditorDriver(hg::QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
        : spe::NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "EditorDriver"}
    {}

    void init(Editor& aEditor) {
        _editor = &aEditor;
    }

    void _eventUpdate1() override {
        HG_HARD_ASSERT(_editor != nullptr);

        auto& winMgr = ccomp<spe::WindowManagerInterface>();
        const auto& input = winMgr.getInput();

        _editorZoom -= input.getVerticalMouseWheelScroll() * 0.1f;
        if (_editorZoom < 0.1f) {
            _editorZoom = 0.1f;
        }

        if (input.checkPressed(hg::in::MB_LEFT)) {
            _editor->onLeftClick(input.getViewRelativeMousePos());
        }

        winMgr.getView().setSize({1024.f, 1024.f});
        winMgr.getView().zoom(_editorZoom);

        const auto xOff = ((float)input.checkPressed(hg::in::PK_D) - (float)input.checkPressed(hg::in::PK_A)) * 8.f;
        const auto yOff = ((float)input.checkPressed(hg::in::PK_S) - (float)input.checkPressed(hg::in::PK_W)) * 8.f;
        winMgr.getView().move({xOff, yOff});
    }

    void _eventDraw1() override {
        HG_HARD_ASSERT(_editor != nullptr);

        auto& winMgr = ccomp<spe::WindowManagerInterface>();
        _editor->draw(winMgr);
    }

    void _eventDrawGUI() override {
        HG_HARD_ASSERT(_editor != nullptr);

        auto& winMgr = ccomp<spe::WindowManagerInterface>();
        _editor->drawGui(winMgr);
    }

private:
    Editor* _editor = nullptr;

    float _editorZoom = 1.f;
};

#define PRIORITY_EDITOR_DRIVER  1
#define PRIORITY_WINDOW_MANAGER 0

std::unique_ptr<spe::GameContext> CreateContex() {
    spe::GameContext::RuntimeConfig rtConfig;
    rtConfig.deltaTime = std::chrono::duration<double>{1.0 / 60.0};
    rtConfig.maxFramesBetweenDisplays = 2;
    auto ctx = std::make_unique<spe::GameContext>(rtConfig);

    // Add a WindowManager
    auto winMgr = std::make_unique<spe::DefaultWindowManager>(ctx->getQAORuntime().nonOwning(), PRIORITY_WINDOW_MANAGER);
    // clang-format off
    spe::WindowManagerInterface::WindowConfig windowConfig{
        hg::win::VideoMode{900, 900},
        "Antimony Animator",
        hg::win::WindowStyle::Default
    };
    spe::WindowManagerInterface::MainRenderTextureConfig mrtConfig{
        /* SIZE */  {1024, 1024},
        /* SMOOTH*/ true
    };
    spe::WindowManagerInterface::TimingConfig timingConfig{
        60, false, true, true
    };
    // clang-format on
    winMgr->setToNormalMode(windowConfig, mrtConfig, timingConfig);
    winMgr->setMainRenderTextureDrawPosition(spe::WindowManagerInterface::DrawPosition::Fit);

    ctx->attachAndOwnComponent(std::move(winMgr));

    return ctx;
}

/*
 * antimony.exe dir/to/sprites sprite_base_name master_cnt lerp_cnt
 */
int main(int argc, char* argv[]) try {
    hg::log::SetMinimalLogSeverity(hg::log::Severity::Info);

    if (argc != 5) {
        HG_LOG_INFO(LOG_ID, "Usage:\n    antimony.exe dir/to/sprites sprite_base_name master_cnt lerp_cnt");
        return EXIT_SUCCESS;
    }
    const std::filesystem::path spriteDir = argv[1];
    const std::string           spriteBaseName = argv[2];
    const hg::PZInteger         masterCount = std::stoi(argv[3]);
    const hg::PZInteger         lerpCount = std::stoi(argv[3]);

    auto context = CreateContex();

    MasterLoader loader{spriteDir, spriteBaseName, masterCount};
    Editor editor{loader, masterCount, lerpCount};

    EditorDriver editorDriver{context->getQAORuntime().nonOwning(), PRIORITY_EDITOR_DRIVER};
    editorDriver.init(editor);

    const auto status = context->runFor(-1);

    return status;
} catch (const hg::TracedException& ex) {
    HG_LOG_FATAL(LOG_ID, "Traced exception caught: {}", ex.getFormattedDescription());
    return EXIT_FAILURE;
} catch (const std::exception& ex) {
    HG_LOG_FATAL(LOG_ID, "Exception caught: {}", ex.what());
    return EXIT_FAILURE;
}
