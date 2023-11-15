
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/QAO/Priority_resolver.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace qao {

QAO_PriorityResolver::QAO_PriorityResolver()
    : _initialPriority{0}
    , _priorityStep{1}
{
}

QAO_PriorityResolver::QAO_PriorityResolver(int initialPriority, int priorityStep) 
    : _initialPriority{initialPriority}
    , _priorityStep{priorityStep}
{
}

void QAO_PriorityResolver::CategoryDefinition::reset() {
    _dependees.clear();
    _priority.reset();
    _dependencyFulfilledCounter = 0;
}

bool QAO_PriorityResolver::CategoryDefinition::priorityAssigned() const {
    return _priority.has_value();
}

bool QAO_PriorityResolver::CategoryDefinition::dependenciesSatisfied() const {
    return (static_cast<int>(_dependencies.size()) == _dependencyFulfilledCounter);
}

void QAO_PriorityResolver::resolveAll() {
    _priorityCounter = _initialPriority;

    // Reset all definitions:
    for (auto& definition : _definitions) {
        definition.reset();
    }

    // For all definitions, list them as dependees in their dependencies:
    for (auto definitionIter = _definitions.begin(); definitionIter != _definitions.end(); definitionIter++) {
        for (auto& dependencyId : (*definitionIter)._dependencies) {
            auto dependencyIter = findDefinition(dependencyId);
            (*dependencyIter)._dependees.push_back(definitionIter);
        }
    }

    // Try to resolve all:
    while (true) {
        // Find definition with all dependencies satisfied:
        std::vector<CategoryDefinition>::iterator curr;
        for (curr = _definitions.begin(); curr != _definitions.end(); curr = std::next(curr)) {
            if ((*curr).priorityAssigned() == false &&
                (*curr).dependenciesSatisfied()) {
                break;
            }
        }
        if (curr == _definitions.end()) {
            break;
        }

        for (auto& dependeeIter : (*curr)._dependees) {
            (*dependeeIter)._dependencyFulfilledCounter += 1;
        }

        (*curr)._priority = _priorityCounter;
        _priorityCounter -= _priorityStep;
    }

    // Verify & populate mapping:
    _priorityMapping.clear();
    for (auto& definition : _definitions) {
        if (!definition.priorityAssigned()) {
            HG_THROW_TRACED(TracedLogicError, 0, "Cannot resolve priorities - impossible situation requested.");
        }
        _priorityMapping[definition._categoryId] = *definition._priority;
        definition.reset();
    }
}

std::vector<QAO_PriorityResolver::CategoryDefinition>::iterator QAO_PriorityResolver::findDefinition(int categoryId) {
    for (auto curr = _definitions.begin(); curr != _definitions.end(); curr = std::next(curr)) {
        if ((*curr)._categoryId == categoryId) {
            return curr;
        }
    }
    HG_THROW_TRACED(TracedLogicError, 0, "Definition with ID {} not found.", categoryId);
}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>