// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_QAO_ORDERER_HPP
#define UHOBGOBLIN_QAO_ORDERER_HPP

#include <set>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace qao {

class QAO_Base;

namespace qao_detail {

struct QAO_OrdererComparator {
    bool operator()(const QAO_Base* a, const QAO_Base* b) const;
};

using QAO_Orderer = std::set<QAO_Base*, QAO_OrdererComparator>;

} // namespace qao_detail

using QAO_OrdererIterator             = qao_detail::QAO_Orderer::iterator;
using QAO_OrdererReverseIterator      = qao_detail::QAO_Orderer::reverse_iterator;
using QAO_OrdererConstIterator        = qao_detail::QAO_Orderer::const_iterator;
using QAO_OrdererConstReverseIterator = qao_detail::QAO_Orderer::const_reverse_iterator;

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_ORDERER_HPP

// clang-format on
