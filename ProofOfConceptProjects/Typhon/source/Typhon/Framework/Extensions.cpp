// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Typhon/GameObjects/Control/Controls_manager.hpp>
#include <Typhon/GameObjects/Control/Environment_manager.hpp>
#include <Typhon/GameObjects/Control/Gameplay_manager.hpp>

#include <cassert>

#include "Extensions.hpp"

TyphonGameContextExtensionData::~TyphonGameContextExtensionData() {
}

const std::type_info& TyphonGameContextExtensionData::getTypeInfo() const {
    return typeid(TyphonGameContextExtensionData);
}

void ExtendGameContext(spempe::GameContext& ctx) {
    ctx.getNetworkingManager().setExecutionPriority(*PEXEPR_NETWORK_MGR);
    ctx.getWindowManager().setExecutionPriority(*PEXEPR_WINDOW_MGR);

    auto extData = std::make_unique<TyphonGameContextExtensionData>();

    cpSpace* space = cpSpaceNew();
    cpSpaceSetUserData(space, &ctx);
    Collideables::installCollisionHandlers(space);
    extData->physicsSpace.reset(space);
    
    extData->mainGameController = QAO_UPCreate<GameplayManager>(ctx.getQaoRuntime().nonOwning());    
    extData->environmentManager = QAO_UPCreate<EnvironmentManager>(ctx.getQaoRuntime().nonOwning());   
    extData->controlsManager = QAO_UPCreate<ControlsManager>(ctx.getQaoRuntime().nonOwning(), 20, 2, 1); // TODO Hardcoded values

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

GameplayManager& GetGameplayManager(spempe::GameContext& ctx) {
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

// clang-format on
