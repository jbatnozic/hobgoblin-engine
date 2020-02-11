
#include <Hobgoblin_include/QAO/config.hpp>
#include <Hobgoblin_include/QAO/base.hpp>
#include <Hobgoblin_include/QAO/id.hpp>
#include <Hobgoblin_include/QAO/registry.hpp>

#include <stdexcept>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {
namespace detail {

QAO_Registry::QAO_Registry(int capacity)
    : _indexer{static_cast<std::size_t>(capacity)}
    , _elements{capacity}  
    , _serial_counter{QAO_MIN_SERIAL}
{
}

QAO_SerialIndexPair QAO_Registry::insert(std::unique_ptr<QAO_Base> ptr) {
    const auto index = static_cast<int>(_indexer.acquire());
    const auto serial = nextSerial();

    adjustSize();

    _elements[index].ptr = std::move(ptr);
    _elements[index].serial = serial;
    _elements[index].no_own = false;
    
    return QAO_SerialIndexPair{serial, index};
}

QAO_SerialIndexPair QAO_Registry::insertNoOwn(QAO_Base* ptr) {
    // TODO Code duplication
    const auto index = static_cast<int>(_indexer.acquire());
    const auto serial = nextSerial();

    adjustSize();

    _elements[index].ptr = std::unique_ptr<QAO_Base>{ptr};
    _elements[index].serial = serial;
    _elements[index].no_own = true;
    
    return QAO_SerialIndexPair{serial, index};
}

std::unique_ptr<QAO_Base> QAO_Registry::release(int index) {
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

void QAO_Registry::erase(int index) {
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

QAO_Base* QAO_Registry::objectAt(int index) const {
    return _elements[index].ptr.get();
}

std::int64_t QAO_Registry::serialAt(int index) const {
    return _elements[index].serial;
}

int QAO_Registry::instanceCount() const {
    return _indexer.countFilled();
}

void QAO_Registry::adjustSize() {
    const auto indexer_size = _indexer.getSize();

    if (indexer_size != _elements.size()) {
        _elements.resize(indexer_size);
    }
}

bool QAO_Registry::isSlotEmpty(int index) const {
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

#include <Hobgoblin_include/Private/pmacro_undef.hpp>