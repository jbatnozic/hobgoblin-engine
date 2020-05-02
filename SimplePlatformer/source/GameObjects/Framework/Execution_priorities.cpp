
#include <Hobgoblin/QAO.hpp>

#include "GameObjects/Framework/Execution_priorities.hpp"

int EXEPR_MAIN_GAME_CONTROLLER;
int EXEPR_TERRAIN_MGR;
int EXEPR_NETWORK_MGR;
int EXEPR_CREATURES;

void ResolveExecutionPriorities() {
    const int initialPriority = 0;
    const int priorityStep = 5;
    hg::QAO_PriorityResolver2 res{initialPriority, priorityStep};

    res.category(&EXEPR_MAIN_GAME_CONTROLLER);

    res.category(&EXEPR_TERRAIN_MGR)
        .precedes(&EXEPR_CREATURES);

    res.category(&EXEPR_NETWORK_MGR)
        .dependsOn(&EXEPR_CREATURES);

    res.category(&EXEPR_CREATURES)
        .precedes(&EXEPR_MAIN_GAME_CONTROLLER);

    res.resolveAll();
}