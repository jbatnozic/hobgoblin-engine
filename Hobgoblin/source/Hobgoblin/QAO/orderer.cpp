
#include <Hobgoblin/QAO/base.hpp>
#include <Hobgoblin/QAO/orderer.hpp>

#include <Hobgoblin/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {
namespace detail {

bool QAO_OrdererComparator::operator()(const QAO_Base* a, const QAO_Base* b) const {
    // If A.priority > B.priority, A precedes B
    // For same priorities, we compare addresses
    const int pri_a = a->getExecutionPriority();
    const int pri_b = b->getExecutionPriority();
    
    return (pri_a > pri_b) ||
           ((pri_a == pri_b) && (reinterpret_cast<std::uintptr_t>(a) < reinterpret_cast<std::uintptr_t>(b)));
}

} // namespace detail
} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/pmacro_undef.hpp>
