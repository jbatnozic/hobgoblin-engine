
#include <Hobgoblin_include/QAO/base.hpp>
#include <Hobgoblin_include/QAO/config.hpp>
#include <Hobgoblin_include/QAO/id.hpp>
#include <Hobgoblin_include/QAO/runtime.hpp>
#include <Hobgoblin_include/Utility/Passkey.hpp>

#include <cstdint>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

QAO_GenericId::QAO_GenericId(QAO_Runtime* runtime, std::int64_t serial, int index)
    : _runtime{runtime}
    , _serial{serial}
    , _index{index}
{
}

QAO_GenericId::QAO_GenericId()
    : QAO_GenericId(nullptr, QAO_NULL_SERIAL, QAO_NULL_INDEX)
{
}

QAO_GenericId::QAO_GenericId(const QAO_Base* obj)
    : QAO_GenericId(obj->_internal_getThisId(util::Passkey<QAO_GenericId>{}))
{
}

QAO_GenericId::QAO_GenericId(const QAO_Base& obj)
    : QAO_GenericId(obj._internal_getThisId(util::Passkey<QAO_GenericId>{}))
{
}

QAO_GenericId::QAO_GenericId(std::nullptr_t p)
    : QAO_GenericId(nullptr, QAO_NULL_SERIAL, QAO_NULL_INDEX)
{
}

bool QAO_GenericId::operator==(const QAO_GenericId& other) const {
    return (_runtime == other._runtime && _serial == other._serial && _index == other._index);
}

bool QAO_GenericId::operator!=(const QAO_GenericId& other) const {
    return !(*this == other);
}

QAO_Base* QAO_GenericId::operator->() const {
    return ptr();
}

QAO_Base& QAO_GenericId::operator*() const {
    return *ptr();
}

QAO_Base* QAO_GenericId::ptr() const {
    if (_runtime && _serial != QAO_NULL_SERIAL && _index != QAO_NULL_INDEX) {
        return _runtime->findObjectWithId(*this);
    }
    return nullptr;
}

int QAO_GenericId::getIndex() const noexcept {
    return _index;
}

std::int64_t QAO_GenericId::getSerial() const noexcept {
    return _serial;
}

QAO_Runtime* QAO_GenericId::getRuntime() const noexcept {
    return _runtime;
}

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>