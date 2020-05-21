
#include <Typhon/GameObjects/Control/Controls_manager.hpp>
#include <Typhon/GameObjects/Control/Environment_manager.hpp>
#include <Typhon/GameObjects/Control/Main_game_controller.hpp>

#include <cassert>

#include "Extensions.hpp"

TyphonGameContextExtensionData::~TyphonGameContextExtensionData() {
}

const std::type_info& TyphonGameContextExtensionData::getTypeInfo() const {
    return typeid(TyphonGameContextExtensionData);
}

void ExtendGameContext(spempe::GameContext& ctx) {
    auto extData = std::make_unique<TyphonGameContextExtensionData>();

    extData->physicsSpace.reset(cpSpaceNew());
    // TODO Temporarily here; should be in EnvironmentManager
    cpSpaceSetUserData(extData->physicsSpace.get(), &ctx);
    cpSpaceSetDamping(extData->physicsSpace.get(), 0.1);
    Collideables::installCollisionHandlers(extData->physicsSpace.get());

    extData->mainGameController = QAO_UPCreate<MainGameController>(ctx.getQaoRuntime().nonOwning());
    
    extData->environmentManager = QAO_UPCreate<EnvironmentManager>(ctx.getQaoRuntime().nonOwning(),
                                                                   ctx.getSyncObjReg(), SYNC_ID_NEW); // TODO Environment manager shouldn't be a SynchronizedObject...
    
    extData->controlsManager = QAO_UPCreate<ControlsManager>(ctx.getQaoRuntime().nonOwning(), 10, 2, 1); // TODO

    ctx.setExtensionData(std::move(extData));
}

ControlsManager& GetControlsManager(spempe::GameContext& ctx) {
    auto* extData = ctx.getExtensionData();
    assert(extData);
    assert(extData->getTypeInfo() == typeid(TyphonGameContextExtensionData));

    auto* p = static_cast<TyphonGameContextExtensionData*>(extData)->controlsManager.get();
    assert(p != nullptr);
    return *p;
}

EnvironmentManager& GetEnvironmentManager(spempe::GameContext& ctx) {
    auto* extData = ctx.getExtensionData();
    assert(extData);
    assert(extData->getTypeInfo() == typeid(TyphonGameContextExtensionData));

    auto* p = static_cast<TyphonGameContextExtensionData*>(extData)->environmentManager.get();
    assert(p != nullptr);
    return *p;
}

spempe::KbInputTracker& GetKeyboardInput(spempe::GameContext& ctx) {
    return ctx.getWindowManager().getKeyboardInput();
}

MainGameController& GetMainGameController(spempe::GameContext& ctx) {
    auto* extData = ctx.getExtensionData();
    assert(extData);
    assert(extData->getTypeInfo() == typeid(TyphonGameContextExtensionData));

    auto* p = static_cast<TyphonGameContextExtensionData*>(extData)->mainGameController.get();
    assert(p != nullptr);
    return *p;
}

spempe::NetworkingManager& GetNetworkingManager(spempe::GameContext& ctx) {
    return ctx.getNetworkingManager();
}

cpSpace* GetPhysicsSpace(spempe::GameContext& ctx) {
    auto* extData = ctx.getExtensionData();
    assert(extData);
    assert(extData->getTypeInfo() == typeid(TyphonGameContextExtensionData));

    auto* p = static_cast<TyphonGameContextExtensionData*>(extData)->physicsSpace.get();
    assert(p != nullptr);
    return p;
}

spempe::WindowManager& GetWindowManager(spempe::GameContext& ctx) {
    return ctx.getWindowManager();
}