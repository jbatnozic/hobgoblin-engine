#ifndef TYPHON_FRAMEWORK_EXECUTION_PRIORITIES_HPP
#define TYPHON_FRAMEWORK_EXECUTION_PRIORITIES_HPP

using ExecutionPriorityPtr = const int* const;

// Managers:
extern ExecutionPriorityPtr PEXEPR_ENVIRON_MGR;
extern ExecutionPriorityPtr PEXEPR_GAMEPLAY_MGR;
extern ExecutionPriorityPtr PEXEPR_NETWORK_MGR;
extern ExecutionPriorityPtr PEXEPR_WINDOW_MGR;

// Other:
extern ExecutionPriorityPtr PEXEPR_ENTITIES;

extern void ResolveExecutionPriorities();

#endif // !TYPHON_FRAMEWORK_EXECUTION_PRIORITIES_HPP

