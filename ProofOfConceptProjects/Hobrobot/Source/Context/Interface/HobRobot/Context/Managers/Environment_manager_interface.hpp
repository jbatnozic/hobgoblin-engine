#pragma once

#include "Engine.hpp"

#include <Hobgoblin/ChipmunkPhysics.hpp>

namespace hobrobot {

class EnvironmentManagerInterface : public spe::ContextComponent {
public:
    enum class Mode {
        Uninitialized,
        Host,
        Client,
    };

    virtual void setToHostMode() = 0;
    virtual void setToClientMode() = 0;
    virtual Mode getMode() const = 0;

    virtual cpSpace* getPhysicsSpace() const = 0;

private:
    SPEMPE_CTXCOMP_TAG("hobrobot::EnvironmentManagerInterface");
};

} // namespace hobrobot
