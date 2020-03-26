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

HOBGOBLIN_NAMESPACE_START
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
        void dependsOn(int* argsHead, ArgsRest&&... argsRest);

        template <class ...NoArgs>
        std::enable_if_t<sizeof...(NoArgs) == 0, void> precedes();

        template <class ...ArgsRest>
        void precedes(int* argsHead, ArgsRest&&... argsRest);

    private:
        QAO_PriorityResolver2& _resolver;
        int* _category;
    };

    DependencyInserter category(int* category);

    void resolveAll();

    template <class T>
    int getPriorityOf(T&& categoryId) const;

private:
    friend class DependencyInserter;
    struct CategoryDefinition;
    using DefinitionMap = std::unordered_map<int*, CategoryDefinition>;

    struct CategoryDefinition {
        std::vector<DefinitionMap::iterator> dependees;
        int dependencyCounter = 0;
        int dependencyFulfilledCounter = 0;
        std::optional<int> priority;

        void reset();
        bool priorityAssigned() const;
        bool dependenciesSatisfied() const;
    };

    DefinitionMap _definitions;
    int _priorityCounter;

    void categoryDependsOn(int* category, int* dependency);
    std::vector<CategoryDefinition>::iterator findDefinition(int categoryId);
};

template <class ...NoArgs>
std::enable_if_t<sizeof...(NoArgs) == 0, void> QAO_PriorityResolver2::DependencyInserter::dependsOn() {
    // Do nothing
}

template <class ...ArgsRest>
void QAO_PriorityResolver2::DependencyInserter::dependsOn(int* argsHead, ArgsRest&&... argsRest) {
    FRIEND_ACCESS _resolver.categoryDependsOn(_category, argsHead);
    dependsOn(std::forward<ArgsRest>(argsRest)...);
}

//template <class T>
//int QAO_PriorityResolver2::getPriorityOf(T&& categoryId) const {
//    const int idAsInt = static_cast<int>(std::forward<T>(categoryId));
//    return _priorityMapping.at(idAsInt);
//}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_PRIORITY_RESOLVER2_HPP