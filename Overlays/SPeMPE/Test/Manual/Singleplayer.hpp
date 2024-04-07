#pragma once

#include "Config.hpp"
#include "Engine.hpp"
#include "Main_gameplay_manager.hpp"

#include <SPeMPE/Managers/Networking_manager_default.hpp>

#include <memory>

namespace singleplayer {

inline
std::unique_ptr<spe::GameContext> CreateGameContext() {
    auto context = std::make_unique<spe::GameContext>(
        spe::GameContext::RuntimeConfig{std::chrono::duration<double>(1.0 / DESIRED_FRAMERATE)});
    context->setToMode(spe::GameContext::Mode::GameMaster);

    // Create and attach a Networking manager
    auto netMgr = std::make_unique<spe::DefaultNetworkingManager>(
        context->getQAORuntime().nonOwning(), PRIORITY_NETWORKMGR, 0);
    netMgr->setToServerMode(
        hg::RN_Protocol::UDP, "pass", 1, 1024, hg::RN_NetworkingStack::Default);

    context->attachAndOwnComponent(std::move(netMgr));

    // Create and attach a Window manager
    auto winMgr = std::make_unique<spe::DefaultWindowManager>(context->getQAORuntime().nonOwning(),
                                                              PRIORITY_WINDOWMGR);
    winMgr->setToNormalMode(
        spe::WindowManagerInterface::WindowConfig{
            hg::win::VideoMode{WINDOW_WIDTH, WINDOW_HEIGHT},
            "SPeMPE Manual Test (Singleplayer)",
            hg::win::WindowStyle::Default
        },
        spe::WindowManagerInterface::MainRenderTextureConfig{{WINDOW_WIDTH, WINDOW_HEIGHT}},
        spe::WindowManagerInterface::TimingConfig{
            DESIRED_FRAMERATE,
            false, /* Framerate limiter */
            false, /* V-Sync */
            true   /* Precise timing */
        }
    );

    context->attachAndOwnComponent(std::move(winMgr));

    // Create and attach a Main gameplay manager
    auto mainGameplayMgr = std::make_unique<MainGameplayManager>(
        context->getQAORuntime().nonOwning());

    context->attachAndOwnComponent(std::move(mainGameplayMgr));

    return context;
}

} // namespace singleplayer
