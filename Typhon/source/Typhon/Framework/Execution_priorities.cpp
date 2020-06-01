
#include <Hobgoblin/QAO.hpp>

#include "Execution_priorities.hpp"

namespace {
int EXEPR_ENVIRON_MGR_;
int EXEPR_GAMEPLAY_MGR_;
int EXEPR_NETWORK_MGR_;
int EXEPR_WINDOW_MGR_;
int EXEPR_ENTITIES_ABOVE_;
int EXEPR_ENTITIES_BELOW_;
} // namespace

ExecutionPriorityPtr PEXEPR_ENVIRON_MGR    = &EXEPR_ENVIRON_MGR_;
ExecutionPriorityPtr PEXEPR_GAMEPLAY_MGR   = &EXEPR_GAMEPLAY_MGR_;
ExecutionPriorityPtr PEXEPR_NETWORK_MGR    = &EXEPR_NETWORK_MGR_;
ExecutionPriorityPtr PEXEPR_WINDOW_MGR     = &EXEPR_WINDOW_MGR_;
ExecutionPriorityPtr PEXEPR_ENTITIES_ABOVE = &EXEPR_ENTITIES_ABOVE_;
ExecutionPriorityPtr PEXEPR_ENTITIES_BELOW = &EXEPR_ENTITIES_BELOW_;

void ResolveExecutionPriorities() {
    const int initialPriority = 0;
    const int priorityStep = 5;
    hg::QAO_PriorityResolver2 res{initialPriority, priorityStep};

    res.category(&EXEPR_GAMEPLAY_MGR_);

    // The environment must be drawn before the entities
    res.category(&EXEPR_ENVIRON_MGR_)
        .precedes(&EXEPR_ENTITIES_ABOVE_)
        .precedes(&EXEPR_ENTITIES_BELOW_);

    // Entities need to set their visible states in the image of their actual states
    // in postUpdate, but before syncing by the NetworkingManager
    res.category(&EXEPR_NETWORK_MGR_)
        .dependsOn(&EXEPR_ENTITIES_ABOVE_)
        .dependsOn(&EXEPR_ENTITIES_BELOW_);

    // 'Below' entities precede 'above' entities
    // All entities precede GameplayController in update step
    // because GPC calculates their requested movement
    res.category(&EXEPR_ENTITIES_BELOW_)
        .precedes(&EXEPR_ENTITIES_ABOVE_);

    res.category(&EXEPR_ENTITIES_ABOVE_)
        .precedes(&EXEPR_GAMEPLAY_MGR_);

    // WindowManager always goes last:
    res.category(&EXEPR_WINDOW_MGR_)
        .dependsOn(&EXEPR_GAMEPLAY_MGR_)
        .dependsOn(&EXEPR_ENVIRON_MGR_)
        .dependsOn(&EXEPR_NETWORK_MGR_)
        .dependsOn(&EXEPR_ENTITIES_ABOVE_)
        .dependsOn(&EXEPR_ENTITIES_BELOW_);

    res.resolveAll();
}