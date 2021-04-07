#ifndef UHOBGOBLIN_QAO_PRIORITY_RESOLVER_HPP
#define UHOBGOBLIN_QAO_PRIORITY_RESOLVER_HPP

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

class QAO_PriorityResolver {
public:
    class CategoryDefinition {
    public:
        CategoryDefinition(int categoryId)
            : _categoryId{categoryId}
            , _dependencyFulfilledCounter{0}
        {
        }

        template <class ...NoArgs>
        std::enable_if_t<sizeof...(NoArgs) == 0, void> dependsOn();

        template <class ArgsHead, class ...ArgsRest>
        void dependsOn(ArgsHead&& argsHead, ArgsRest&&... argsRest);

    private:
        std::vector<int> _dependencies;
        std::vector<std::vector<CategoryDefinition>::iterator> _dependees;
        std::optional<int> _priority;
        int _categoryId;
        int _dependencyFulfilledCounter;

        void reset();
        bool priorityAssigned() const;
        bool dependenciesSatisfied() const;

        friend class QAO_PriorityResolver;
    };

    QAO_PriorityResolver();
    QAO_PriorityResolver(int initialPriority, int priorityStep);

    template <class T>
    int getPriorityOf(T&& categoryId) const;

    template <class T>
    CategoryDefinition& category(T&& categoryId);

    void resolveAll();

private:
    std::vector<CategoryDefinition> _definitions;
    std::unordered_map<int, int> _priorityMapping;
    int _initialPriority;
    int _priorityStep;
    int _priorityCounter;

    std::vector<CategoryDefinition>::iterator findDefinition(int categoryId);
};

template <class ...NoArgs>
std::enable_if_t<sizeof...(NoArgs) == 0, void> QAO_PriorityResolver::CategoryDefinition::dependsOn() {
    // Do nothing
}

template <class ArgsHead, class ...ArgsRest>
void QAO_PriorityResolver::CategoryDefinition::dependsOn(ArgsHead&& argsHead, ArgsRest&&... argsRest) {
    _dependencies.push_back(static_cast<int>(std::forward<ArgsHead>(argsHead)));
    dependsOn(std::forward<ArgsRest>(argsRest)...);
}

template <class T>
int QAO_PriorityResolver::getPriorityOf(T&& categoryId) const {
    const int idAsInt = static_cast<int>(std::forward<T>(categoryId));
    return _priorityMapping.at(idAsInt);
}

template <class T>
QAO_PriorityResolver::CategoryDefinition& QAO_PriorityResolver::category(T&& categoryId) {
    const int idAsInt = static_cast<int>(std::forward<T>(categoryId));
    for (auto& definition : _definitions) {
        if (FRIEND_ACCESS definition._categoryId == idAsInt) {
            assert(false && "Definition with the same category ID already exists");
        }
    }

    _definitions.emplace_back(idAsInt);
    return _definitions.back();
}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_PRIORITY_RESOLVER_HPP