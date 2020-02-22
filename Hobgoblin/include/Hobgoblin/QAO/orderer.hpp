#ifndef UHOBGOBLIN_QAO_ORDERER_HPP
#define UHOBGOBLIN_QAO_ORDERER_HPP

#include <set>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

class QAO_Base;

namespace detail {

struct QAO_OrdererComparator {
    bool operator()(const QAO_Base* a, const QAO_Base* b) const;
};

using QAO_Orderer = std::set<QAO_Base*, QAO_OrdererComparator>;

} // namespace detail

using QAO_OrdererIterator             = detail::QAO_Orderer::iterator;
using QAO_OrdererReverseIterator      = detail::QAO_Orderer::reverse_iterator;
using QAO_OrdererConstIterator        = detail::QAO_Orderer::const_iterator;
using QAO_OrdererConstReverseIterator = detail::QAO_Orderer::const_reverse_iterator;

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_QAO_ORDERER_HPP