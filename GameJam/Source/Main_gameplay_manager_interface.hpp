#pragma once

#include "Engine.hpp"

//! TODO(add description)
class MainGameplayManagerInterface : public spe::ContextComponent {
public:
    enum class Mode {
        UNINITIALIZED,
        HOST,
        CLIENT,
    };

    ~MainGameplayManagerInterface() override = default;

    //! \param aPlayerCount number of players in the game, including player 0 (the host)
    virtual void setToHostMode(hg::PZInteger aPlayerCount) = 0;
    virtual void setToClientMode() = 0;
    virtual Mode getMode() const = 0;

private:
    SPEMPE_CTXCOMP_TAG("MainGameplayManagerInterface");
};

using MMainGameplay = MainGameplayManagerInterface;
