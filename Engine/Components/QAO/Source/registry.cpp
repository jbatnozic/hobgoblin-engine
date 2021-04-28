
#include <Hobgoblin/QAO/config.hpp>
#include <Hobgoblin/QAO/base.hpp>
#include <Hobgoblin/QAO/registry.hpp>
#include <Hobgoblin/Utility/Exceptions.hpp>

#include <stdexcept>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace qao {
namespace detail {

QAO_Registry::QAO_Registry(PZInteger capacity)
    : _indexer{capacity}
    , _elements{ToSz(capacity)}
    , _serial_counter{QAO_MIN_SERIAL}
{
}

QAO_SerialIndexPair QAO_Registry::insert(std::unique_ptr<QAO_Base> ptr) {
    const auto index = static_cast<PZInteger>(_indexer.acquire());
    const auto serial = nextSerial();

    adjustSize();

    _elements[ToSz(index)].ptr = std::move(ptr);
    _elements[ToSz(index)].serial = serial;
    _elements[ToSz(index)].no_own = false;

    return QAO_SerialIndexPair{serial, index};
}

QAO_SerialIndexPair QAO_Registry::insertNoOwn(QAO_Base* ptr) {
    const auto index = static_cast<int>(_indexer.acquire());
    const auto serial = nextSerial();

    adjustSize();

    _elements[index].ptr = std::unique_ptr<QAO_Base>{ptr};
    _elements[index].serial = serial;
    _elements[index].no_own = true;

    return QAO_SerialIndexPair{serial, index};
}

void QAO_Registry::insert(std::unique_ptr<QAO_Base> ptr, QAO_SerialIndexPair serialIndexPair) {
    if (!_indexer.tryAcquireSpecific(serialIndexPair.index)) {
        throw util::TracedLogicError("Cannot register object; Index already in use");
    }

    if (objectWithSerial(serialIndexPair.serial) != nullptr) {
        throw util::TracedLogicError("Cannot register object; Serial already in use");
    }

    adjustSize();

    _elements[serialIndexPair.index].ptr = std::move(ptr);
    _elements[serialIndexPair.index].serial = serialIndexPair.serial;
    _elements[serialIndexPair.index].no_own = false;
}

void QAO_Registry::insertNoOwn(QAO_Base* ptr, QAO_SerialIndexPair serialIndexPair) {
    if (!_indexer.tryAcquireSpecific(serialIndexPair.index)) {
        throw util::TracedLogicError("Cannot register object; Index already in use");
    }

    if (objectWithSerial(serialIndexPair.serial) != nullptr) {
        throw util::TracedLogicError("Cannot register object; Serial already in use");
    }

    adjustSize();

    _elements[serialIndexPair.index].ptr = std::unique_ptr<QAO_Base>{ptr};
    _elements[serialIndexPair.index].serial = serialIndexPair.serial;
    _elements[serialIndexPair.index].no_own = true;
}

std::unique_ptr<QAO_Base> QAO_Registry::release(PZInteger index) {
    Elem& elem = _elements[index];
    if (elem.no_own) {
        elem.ptr.release();
        _indexer.free(static_cast<std::size_t>(index));
        return std::unique_ptr<QAO_Base>{nullptr};
    }
    else {
        auto rv = std::move(elem.ptr);
        _indexer.free(static_cast<std::size_t>(index));
        return rv;
    }
}

void QAO_Registry::erase(PZInteger index) {
    Elem& elem = _elements[index];
    if (elem.no_own) {
        elem.ptr.release();
    } else {
        elem.ptr.reset();
    }
    _indexer.free(static_cast<std::size_t>(index));
}

int QAO_Registry::size() const {
    return _indexer.getSize();
}

QAO_Base* QAO_Registry::objectAt(PZInteger index) const {
    return _elements[index].ptr.get();
}

QAO_Base* QAO_Registry::objectWithSerial(std::int64_t serial) const {
    for (auto& elem : _elements) {
        if (elem.serial == serial && elem.ptr != nullptr) {
            return elem.ptr.get();
        }
    }
    return nullptr;
}

bool QAO_Registry::isObjectAtOwned(PZInteger index) const {
    return !_elements[index].no_own;
}

std::int64_t QAO_Registry::serialAt(PZInteger index) const {
    return _elements[index].serial;
}

PZInteger QAO_Registry::instanceCount() const {
    return _indexer.countFilled();
}

void QAO_Registry::adjustSize() {
    const auto indexer_size = _indexer.getSize();

    if (indexer_size != _elements.size()) {
        _elements.resize(indexer_size);
    }
}

bool QAO_Registry::isSlotEmpty(PZInteger index) const {
    return _indexer.isSlotEmpty(static_cast<std::size_t>(index));
}

std::int64_t QAO_Registry::nextSerial() {
    std::int64_t rv = _serial_counter;
    _serial_counter += 1;
    return rv;
}

} // namespace detail
} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
