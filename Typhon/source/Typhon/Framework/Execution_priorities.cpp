
#include <Hobgoblin/QAO.hpp>

#include "Execution_priorities.hpp"

int EXEPR_MAIN_GAME_CONTROLLER;
int EXEPR_ENVIRON_MGR;
int EXEPR_NETWORK_MGR;
int EXEPR_WINDOW_MGR;
int EXEPR_CREATURES;

void ResolveExecutionPriorities() {
    const int initialPriority = 0;
    const int priorityStep = 5;
    hg::QAO_PriorityResolver2 res{initialPriority, priorityStep};

    res.category(&EXEPR_MAIN_GAME_CONTROLLER);

    // The environment must be drawn before the creatures
    res.category(&EXEPR_ENVIRON_MGR)
        .precedes(&EXEPR_CREATURES);

    // Creatures need to set their visible states in the image of their actual states
    // in postUpdate, but before syncing by the NetworkingManager
    res.category(&EXEPR_NETWORK_MGR)
        .dependsOn(&EXEPR_CREATURES);

    // Creatures precede MainGameController in update step
    // because MGC calculates their requested movement
    res.category(&EXEPR_CREATURES)
        .precedes(&EXEPR_MAIN_GAME_CONTROLLER);

    // WindowManager always goes last:
    res.category(&EXEPR_WINDOW_MGR)
        .dependsOn(&EXEPR_MAIN_GAME_CONTROLLER)
        .dependsOn(&EXEPR_ENVIRON_MGR)
        .dependsOn(&EXEPR_NETWORK_MGR)
        .dependsOn(&EXEPR_CREATURES);

    res.resolveAll();
}