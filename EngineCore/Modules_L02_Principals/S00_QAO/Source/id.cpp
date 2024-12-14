// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/QAO/base.hpp>
#include <Hobgoblin/QAO/config.hpp>
#include <Hobgoblin/QAO/id.hpp>
#include <Hobgoblin/QAO/runtime.hpp>
#include <Hobgoblin/Utility/Passkey.hpp>

#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace qao {

QAO_GenericId::QAO_GenericId()
    : QAO_GenericId(QAO_NULL_SERIAL, QAO_NULL_INDEX)
{
}

QAO_GenericId::QAO_GenericId(std::nullptr_t)
    : QAO_GenericId(QAO_NULL_SERIAL, QAO_NULL_INDEX)
{
}

QAO_GenericId::QAO_GenericId(const QAO_Base* object)
    : QAO_GenericId{object ? object->_context.id : nullptr}
{
}

QAO_GenericId::QAO_GenericId(const QAO_Base& object)
    : QAO_GenericId{object._context.id}
{
}

QAO_GenericId::QAO_GenericId(std::int64_t serial, PZInteger index)
    : _serial{serial}
    , _index{index}
{
}

bool QAO_GenericId::operator==(const QAO_GenericId& other) const {
    return (_serial == other._serial && _index == other._index);
}

bool QAO_GenericId::operator!=(const QAO_GenericId& other) const {
    return !(*this == other);
}

int QAO_GenericId::getIndex() const noexcept {
    return _index;
}

std::int64_t QAO_GenericId::getSerial() const noexcept {
    return _serial;
}

bool QAO_GenericId::isNull() const noexcept {
    return (_serial == QAO_NULL_SERIAL || _index == QAO_NULL_INDEX);
}

util::OutputStream& operator<<(util::OutputStreamExtender& ostream, const QAO_GenericId& self) {
    ostream << self._serial << self._index;
    return *ostream;
}

util::InputStream& operator>>(util::InputStreamExtender& istream, QAO_GenericId& self) {
    istream->noThrow() >> self._serial >> self._index;
    return *istream;
}

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
