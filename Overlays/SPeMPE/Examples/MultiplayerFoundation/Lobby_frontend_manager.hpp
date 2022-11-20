#pragma once

#include "Engine.h"
#include "Lobby_frontend_manager_interface.hpp"

#include <memory>

class LobbyFrontendManager
    : public LobbyFrontendManagerInterface
    , public spe::NonstateObject
{
public:
    LobbyFrontendManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);
    ~LobbyFrontendManager() override;

    void setToNormalMode() override;
    void setToHeadlessMode() override;
    Mode getMode() const override;

private:
    class Impl;
    friend Impl;
    std::unique_ptr<Impl> _impl;

    void _eventDrawGUI() override;

    friend void ActivateCommand(LobbyFrontendManager& aMgr, int aCommand, void* aArgs);
};
