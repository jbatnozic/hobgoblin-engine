
#include <Typhon/GameObjects/Control/Controls_manager.hpp>
#include <Typhon/GameObjects/Control/Environment_manager.hpp>

#include <cassert>

#include "Extensions.hpp"

TyphonGameContextExtensionData::~TyphonGameContextExtensionData() {
}

const std::type_info& TyphonGameContextExtensionData::getTypeInfo() const {
    return typeid(TyphonGameContextExtensionData);
}

void ExtendGameContext(spempe::GameContext& ctx) {
    auto extData = std::make_unique<TyphonGameContextExtensionData>();
    extData->controlsManager = QAO_UPCreate<ControlsManager>(ctx.getQaoRuntime().nonOwning(), 1, 2, 1); // TODO
    extData->environmentManager = QAO_UPCreate<EnvironmentManager>(ctx.getQaoRuntime().nonOwning(),
                                                                   ctx.getSyncObjReg(), SYNC_ID_NEW);
    extData->physicsSpace.reset(cpSpaceNew());
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