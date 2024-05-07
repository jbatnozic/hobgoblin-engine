// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef TYPHON_FRAMEWORK_EXECUTION_PRIORITIES_HPP
#define TYPHON_FRAMEWORK_EXECUTION_PRIORITIES_HPP

using ExecutionPriorityPtr = const int* const;

// Managers:
extern ExecutionPriorityPtr PEXEPR_ENVIRON_MGR;
extern ExecutionPriorityPtr PEXEPR_GAMEPLAY_MGR;
extern ExecutionPriorityPtr PEXEPR_NETWORK_MGR;
extern ExecutionPriorityPtr PEXEPR_WINDOW_MGR;

// Other:
extern ExecutionPriorityPtr PEXEPR_ENTITIES_ABOVE;
extern ExecutionPriorityPtr PEXEPR_ENTITIES_BELOW;

extern void ResolveExecutionPriorities();

#endif // !TYPHON_FRAMEWORK_EXECUTION_PRIORITIES_HPP

// clang-format on
