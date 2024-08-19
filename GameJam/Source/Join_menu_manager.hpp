#pragma once

#include "Engine.hpp"

#include "Context_factory.hpp"
#include "Join_menu_manager_interface.hpp"

#include <memory>

class JoinMenuManager
    : public JoinMenuManagerInterface
    , public spe::NonstateObject {
public:
    JoinMenuManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);
    ~JoinMenuManager() override;

    void setVisible(bool aVisible) override;

private:
    class Impl;
    friend class Impl;
    std::unique_ptr<Impl> _impl;

    std::unique_ptr<ClientGameParams> _clientGameParams;
    bool _timeToDie = false;

    void _eventPreUpdate() override;
    void _eventDrawGUI() override;
};
