#ifndef TYPHON_FRAMEWORK_EXTENSIONS_HPP
#define TYPHON_FRAMEWORK_EXTENSIONS_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <SPeMPE/SPeMPE.hpp>

#include <memory>
#include <type_traits>
#include <utility>

class ControlsManager;
class EnvironmentManager;
class GameplayManager;

struct TyphonGameContextExtensionData : public spempe::GameContextExtensionData {
    hg::cpSpaceUPtr physicsSpace;
    std::unique_ptr<GameplayManager> mainGameController;
    std::unique_ptr<EnvironmentManager> environmentManager;
    std::unique_ptr<ControlsManager> controlsManager;

    ~TyphonGameContextExtensionData() override;
    const std::type_info& getTypeInfo() const override;
};

void ExtendGameContext(spempe::GameContext& ctx);

ControlsManager& GetControlsManager(spempe::GameContext& ctx);
EnvironmentManager& GetEnvironmentManager(spempe::GameContext& ctx);
spempe::KbInputTracker& GetKeyboardInput(spempe::GameContext& ctx);
GameplayManager& GetGameplayManager(spempe::GameContext& ctx);
spempe::NetworkingManager& GetNetworkingManager(spempe::GameContext& ctx);
cpSpace* GetPhysicsSpace(spempe::GameContext& ctx);
template <class taArithmetic>
const hg::gr::Multisprite& GetSprite(spempe::GameContext& ctx, taArithmetic&& val) {
    if constexpr (std::is_integral_v<std::remove_cv_t<std::remove_reference_t<taArithmetic>>>) {
        return ctx.getResourceConfig().spriteLoader->getSprite(static_cast<hg::PZInteger>(std::forward<taArithmetic>(val)));
    }
    else if constexpr (std::is_enum_v<std::remove_cv_t<std::remove_reference_t<taArithmetic>>>) {
        return ctx.getResourceConfig().spriteLoader->getSprite(static_cast<hg::PZInteger>(std::forward<taArithmetic>(val)));
    }
    else {
        return ctx.getResourceConfig().spriteLoader->getSprite(std::forward<taArithmetic>(val));
    }
}
spempe::WindowManager& GetWindowManager(spempe::GameContext& ctx);

// Prefixes:
// M = [M]anager object
// D = Other [D]ata
#define PASTE_CONTEXT_TAG_DEFINITIONS \
    enum ContextTagControlsManager    { MControls }; \
    enum ContextTagEnvironmentManager { MEnvironment }; \
    enum ContextTagKeyboardInput      { DKeyboard }; \
    enum ContextTagGameplayManager    { MGameplay }; \
    enum ContextTagNetworkingManager  { MNetworking }; \
    enum ContextTagPhysicsSpace       { DPhysicsSpace }; \
    enum ContextTagSprite             { DSprite }; \
    enum ContextTagWindowManager      { MWindow }

#define PASTE_CTX_METHOD_OVERLOADS \
    ControlsManager& ctx(ContextTagControlsManager) const { return GetControlsManager(ctx()); } \
    EnvironmentManager& ctx(ContextTagEnvironmentManager) const { return GetEnvironmentManager(ctx()); } \
    spempe::KbInputTracker& ctx(ContextTagKeyboardInput) const { return GetKeyboardInput(ctx()); } \
    GameplayManager& ctx(ContextTagGameplayManager) const { return GetGameplayManager(ctx()); } \
    spempe::NetworkingManager& ctx(ContextTagNetworkingManager) const { return GetNetworkingManager(ctx()); } \
    cpSpace* ctx(ContextTagPhysicsSpace) const { return GetPhysicsSpace(ctx()); } \
    template <class taArithmetic> \
    const hg::gr::Multisprite& ctx(ContextTagSprite, taArithmetic&& val) const { return GetSprite(ctx(), std::forward<taArithmetic>(val)); } \
    spempe::WindowManager& ctx(ContextTagWindowManager) const { return GetWindowManager(ctx()); } \

#endif // !TYPHON_FRAMEWORK_EXTENSIONS_HPP
