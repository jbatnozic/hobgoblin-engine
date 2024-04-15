// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_QAO_PRIORITY_RESOLVER2_HPP
#define UHOBGOBLIN_QAO_PRIORITY_RESOLVER2_HPP

#include <cassert>
#include <list>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace qao {

class QAO_PriorityResolver2 {
public:
    class DependencyInserter {
    public:
        DependencyInserter(QAO_PriorityResolver2& resolver, int* category)
            : _resolver{resolver}
            , _category{category}
        {
        }

        template <class ...NoArgs>
        std::enable_if_t<sizeof...(NoArgs) == 0, void> dependsOn();

        template <class ...ArgsRest>
        DependencyInserter& dependsOn(int* argsHead, ArgsRest&&... argsRest);

        template <class ...NoArgs>
        std::enable_if_t<sizeof...(NoArgs) == 0, void> precedes();

        template <class ...ArgsRest>
        DependencyInserter& precedes(int* argsHead, ArgsRest&&... argsRest);

    private:
        QAO_PriorityResolver2& _resolver;
        int* _category;
    };

    QAO_PriorityResolver2();
    QAO_PriorityResolver2(int initialPriority, int priorityStep);

    DependencyInserter category(int* category);

    void resolveAll();

private:
    friend class DependencyInserter;

    class DefinitionMapIterator;

    struct CategoryDefinition {
        std::vector<DefinitionMapIterator> dependees;
        int dependencyCounter = 0;
        int dependencyFulfilledCounter = 0;
        std::optional<int> priority;

        void reset();
        bool priorityAssigned() const;
        bool dependenciesSatisfied() const;
    };

    using DefinitionMap = std::unordered_map<int*, CategoryDefinition>;
    class DefinitionMapIterator : public DefinitionMap::iterator {
        // This doesn't seem like a very well thought out class but whatever, it works.
    public:
        DefinitionMapIterator(const DefinitionMap::iterator& aOther) 
            : DefinitionMap::iterator(aOther) {}
    };

    DefinitionMap _definitions;
    int _initialPriority;
    int _priorityStep;
    int _priorityCounter;

    void categoryDependsOn(int* category, int* dependency);
    void categoryPrecedes(int* category, int* dependee);
    std::vector<CategoryDefinition>::iterator findDefinition(int categoryId);
};

template <class ...NoArgs>
std::enable_if_t<sizeof...(NoArgs) == 0, void> QAO_PriorityResolver2::DependencyInserter::dependsOn() {
    // Do nothing
}

template <class ...ArgsRest>
QAO_PriorityResolver2::DependencyInserter& 
QAO_PriorityResolver2::DependencyInserter::dependsOn(int* argsHead, ArgsRest&&... argsRest) {
    _resolver.categoryDependsOn(_category, argsHead);
    dependsOn(std::forward<ArgsRest>(argsRest)...);
    return SELF;
}

template <class ...NoArgs>
std::enable_if_t<sizeof...(NoArgs) == 0, void> QAO_PriorityResolver2::DependencyInserter::precedes() {
    // Do nothing
}

template <class ...ArgsRest>
QAO_PriorityResolver2::DependencyInserter&
QAO_PriorityResolver2::DependencyInserter::precedes(int* argsHead, ArgsRest&&... argsRest) {
    _resolver.categoryPrecedes(_category, argsHead);
    precedes(std::forward<ArgsRest>(argsRest)...);
    return SELF;
}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_PRIORITY_RESOLVER2_HPP

// clang-format on
