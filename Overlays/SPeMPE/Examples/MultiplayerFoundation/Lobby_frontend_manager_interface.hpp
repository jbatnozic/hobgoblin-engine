#pragma once

#include "Engine.h"

class LobbyFrontendManagerInterface
    : public spe::ContextComponent
{
public:
    enum class Mode {
        Uninitialized,
        Normal,
        Headless
    };

    ~LobbyFrontendManagerInterface() override = default;

    virtual void setToNormalMode() = 0;
    virtual void setToHeadlessMode() = 0;
    virtual Mode getMode() const = 0;

private:
    SPEMPE_CTXCOMP_TAG("LobbyFrontendManagerInterface");
};
