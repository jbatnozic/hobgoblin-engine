#pragma once

#include "Engine.hpp"

class JoinMenuManagerInterface : public spe::ContextComponent {
public:
    virtual void setVisible(bool aVisible) = 0;
    
private:
    SPEMPE_CTXCOMP_TAG("JoinMenuManagerInterface");
};

