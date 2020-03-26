
#include <Hobgoblin/Utility/Exceptions.hpp>
#include <Hobgoblin/QAO/Priority_resolver2.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

void QAO_PriorityResolver2::CategoryDefinition::reset() {
    // dependencyCounter = 0;
    dependencyFulfilledCounter = 0;
    priority.reset();
}

bool QAO_PriorityResolver2::CategoryDefinition::priorityAssigned() const {
    return priority.has_value();
}

bool QAO_PriorityResolver2::CategoryDefinition::dependenciesSatisfied() const {
    return (dependencyFulfilledCounter == dependencyCounter);
}

QAO_PriorityResolver2::DependencyInserter QAO_PriorityResolver2::category(int* category) {
    return DependencyInserter(Self, category);
}

void QAO_PriorityResolver2::resolveAll() {
    _priorityCounter = 0; // TODO Parametrize

    // Reset all definitions:
    for (auto& definition : _definitions) {
        definition.second.reset();
    }

    // Try to resolve all:
    while (true) {
        // Find definition with all dependencies satisfied:
        DefinitionMap::iterator curr;
        for (curr = _definitions.begin(); curr != _definitions.end(); curr = std::next(curr)) {
            if ((*curr).second.priorityAssigned() == false &&
                (*curr).second.dependenciesSatisfied()) {
                break;
            }
        }
        if (curr == _definitions.end()) {
            break;
        }

        for (auto& dependeeIter : (*curr).second.dependees) {
            (*dependeeIter).second.dependencyFulfilledCounter += 1;
        }

        (*curr).second.priority = _priorityCounter;
        _priorityCounter -= 1; // TODO Parametrize
    }

    // Verify & populate output:
    for (auto& definitionIter : _definitions) {
        if (!definitionIter.second.priorityAssigned()) {
            throw util::TracedLogicError("Cannot resolve priorities - impossible situation requested");
        }

        int* categoryPriorityPtr = definitionIter.first;
        *categoryPriorityPtr = *(definitionIter.second.priority);
    }
}

void QAO_PriorityResolver2::categoryDependsOn(int* category, int* dependency) {
    auto dependeeIter = _definitions.find(category);
    if (dependeeIter == _definitions.end()) {
        dependeeIter = _definitions.emplace(std::make_pair(category, CategoryDefinition{})).first;
    }
    (*dependeeIter).second.dependencyCounter += 1;

    _definitions[dependency].dependees.push_back(dependeeIter);
}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>