#pragma once

#include "Engine.hpp"

class HostMenuManagerInterface : public spe::ContextComponent {
public:
    virtual void setVisible(bool aVisible) = 0;
    virtual void setZeroTierEnabled(bool aEnabled) = 0;

private:
    SPEMPE_CTXCOMP_TAG("HostMenuManagerInterface");
};
