#pragma once

#include "Engine.h"

<<<<<<< HEAD
#include <string>

=======
>>>>>>> e2774ff (idk what)
class LobbyFrontendManagerInterface
    : public spe::ContextComponent
{
public:
    enum class Mode {
        Uninitialized,
<<<<<<< HEAD
        HeadlessHost,
        Client,
=======
        Normal,
        Headless
>>>>>>> e2774ff (idk what)
    };

    ~LobbyFrontendManagerInterface() override = default;

<<<<<<< HEAD
    virtual void setToHeadlessHostMode() = 0;
    //virtual void setToHostMode() = 0;
    virtual void setToClientMode(const std::string& aName, const std::string& aUniqueId) = 0;
    
=======
    virtual void setToNormalMode() = 0;
    virtual void setToHeadlessMode() = 0;
>>>>>>> e2774ff (idk what)
    virtual Mode getMode() const = 0;

private:
    SPEMPE_CTXCOMP_TAG("LobbyFrontendManagerInterface");
};
