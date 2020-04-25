
#include <Hobgoblin/QAO/base.hpp>
#include <Hobgoblin/QAO/runtime.hpp>

#include <cassert>
#include <limits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

constexpr std::int64_t MIN_STEP_ORDINAL = std::numeric_limits<std::int64_t>::min(); // TODO to config.hpp

QAO_Runtime::QAO_Runtime()
    : QAO_Runtime{nullptr}
{
}

QAO_Runtime::QAO_Runtime(util::AnyPtr userData)
    : _step_counter{MIN_STEP_ORDINAL + 1}
    , _iteration_ordinal{-1}
    , _current_event{QAO_Event::NoEvent}
    , _step_orderer_iterator{_orderer.end()}
    , _user_data{userData}
{
}

QAO_Runtime::~QAO_Runtime() {
    for (PZInteger i = 0; i < _registry.size(); i += 1) {
        if (!_registry.isSlotEmpty(i)) {
            QAO_Base* const object = _registry.objectAt(i);
            delete object;
        }
    }
}

QAO_RuntimeRef QAO_Runtime::nonOwning() {
    QAO_RuntimeRef rv{this};
    FRIEND_ACCESS rv._isOwning = false;
    return rv;
}

void QAO_Runtime::addObject(std::unique_ptr<QAO_Base> object) {
    QAO_Base* const objRaw = object.get();
    const auto reg_pair = _registry.insert(std::move(object));

    auto ordPair = _orderer.insert(objRaw); // first = iterator, second = added_new
    assert(ordPair.second);

    FRIEND_ACCESS objRaw->_context = QAO_Base::Context{
        MIN_STEP_ORDINAL,
        QAO_GenericId{reg_pair.serial, reg_pair.index},
        ordPair.first,
        this
    };
}

void QAO_Runtime::addObjectNoOwn(QAO_Base& object) {
    const auto reg_pair = _registry.insertNoOwn(&object);

    auto ordPair = _orderer.insert(&object); // first = iterator, second = added_new
    assert(ordPair.second);

    FRIEND_ACCESS object._context = QAO_Base::Context{
        MIN_STEP_ORDINAL,
        QAO_GenericId{reg_pair.serial, reg_pair.index},
        ordPair.first,
        this
    };
}

void QAO_Runtime::addObject(std::unique_ptr<QAO_Base> object, QAO_GenericId specififcId) {
    QAO_Base* const objRaw = object.get();
    _registry.insert(std::move(object), detail::QAO_SerialIndexPair{specififcId.getSerial(), specififcId.getIndex()});

    auto ordPair = _orderer.insert(objRaw); // first = iterator, second = added_new
    assert(ordPair.second);

    FRIEND_ACCESS objRaw->_context = QAO_Base::Context{
        MIN_STEP_ORDINAL,
        FRIEND_ACCESS objRaw->_context.id,
        ordPair.first,
        this
    };
}

void QAO_Runtime::addObjectNoOwn(QAO_Base& object, QAO_GenericId specififcId) {
    _registry.insertNoOwn(&object, detail::QAO_SerialIndexPair{specififcId.getSerial(), specififcId.getIndex()});

    auto ordPair = _orderer.insert(&object); // first = iterator, second = added_new
    assert(ordPair.second);

    FRIEND_ACCESS object._context = QAO_Base::Context{
        MIN_STEP_ORDINAL,
        FRIEND_ACCESS object._context.id,
        ordPair.first,
        this
    };
}

std::unique_ptr<QAO_Base> QAO_Runtime::releaseObject(QAO_Base* object) {
    assert(object);
    assert(object->getRuntime() == this);

    const auto id = object->getId();
    const auto index = id.getIndex();
    const auto serial = id.getSerial();

    std::unique_ptr<QAO_Base> rv = _registry.release(index); // nullptr if object wasn't owned

    // If current _step_orderer_iterator points to released object, advance it first
    if (_step_orderer_iterator != _orderer.end() && *_step_orderer_iterator == object) {
        _step_orderer_iterator = std::next(_step_orderer_iterator);
    }
    _orderer.erase(object);

    FRIEND_ACCESS object->_context = QAO_Base::Context{};

    return rv;
}

void QAO_Runtime::eraseObject(QAO_Base* object) {
    releaseObject(object).reset();
}

void QAO_Runtime::eraseAllNonOwnedObjects() {
    std::vector<QAO_Base*> objectsToErase;
    for (auto& object : SELF) {
        if (ownsObject(object)) {
            objectsToErase.push_back(object);
        }
    }
    for (auto& object : objectsToErase) {
        eraseObject(object);
    }
}

QAO_Base* QAO_Runtime::find(const std::string& name) const {
    return nullptr; // TODO
}

QAO_Base* QAO_Runtime::find(QAO_GenericId id) const {
    const auto index = id.getIndex();
    const auto serial = id.getSerial();

    if (_registry.size() <= index) {
        return nullptr;
    }
    if (_registry.serialAt(index) != serial) {
        return nullptr;
    }
    return _registry.objectAt(index);
}

void QAO_Runtime::updateExecutionPriorityForObject(QAO_Base* object, int newPriority) {
    assert(object);
    assert(find(object->getId()) == object);

    _orderer.erase(object);
    FRIEND_ACCESS object->_execution_priority = newPriority;

    const auto ord_pair = _orderer.insert(object); // first = iterator, second = added_new
    FRIEND_ACCESS object->_context.ordererIterator = ord_pair.first;
}

// Execution

void QAO_Runtime::startStep() {
    _current_event = QAO_Event::FrameStart;
    _step_orderer_iterator = _orderer.begin();
    _iteration_ordinal += 1;
}

void QAO_Runtime::advanceStep(bool& done, std::int32_t eventFlags) {
    done = false;
    QAO_OrdererIterator& curr = _step_orderer_iterator;

    //-----------------------------------------//
    for (std::int32_t i = _current_event; i < QAO_Event::Count; i += 1) {
        if ((eventFlags & (1 << i)) == 0) {
            continue;
        }

        auto ev = static_cast<QAO_Event::Enum>(i);
        _current_event = ev;

        while (curr != _orderer.end()) {
            QAO_Base* const instance = *curr;
            curr = std::next(curr);
            if (FRIEND_ACCESS instance->_context.stepOrdinal < _step_counter) {
                FRIEND_ACCESS instance->_callEvent(ev);
                FRIEND_ACCESS instance->_context.stepOrdinal = _step_counter;
            }
        }

        curr = _orderer.begin();
        _step_counter += 1;
    }
    //-----------------------------------------//

    _current_event = QAO_Event::NoEvent;
    done = true;
}

QAO_Event::Enum QAO_Runtime::getCurrentEvent() {
    return _current_event;
}

// Other

PZInteger QAO_Runtime::getObjectCount() const noexcept {
    return _registry.instanceCount();
}

bool QAO_Runtime::ownsObject(const QAO_Base* object) const {
    assert(object);
    assert(object->getRuntime() == this);

    const auto id = object->getId();
    const auto index = id.getIndex();

    return _registry.isObjectAtOwned(index);
}

// User data

void QAO_Runtime::setUserData(std::nullptr_t) {
    _user_data.reset(nullptr);
}

// Orderer/instance iterations

QAO_OrdererIterator QAO_Runtime::begin() {
    return _orderer.begin();
}

QAO_OrdererIterator QAO_Runtime::end() {
    return _orderer.end();
}

QAO_OrdererReverseIterator QAO_Runtime::rbegin() {
    return _orderer.rbegin();
}

QAO_OrdererReverseIterator QAO_Runtime::rend() {
    return _orderer.rend();
}

QAO_OrdererConstIterator QAO_Runtime::cbegin() const {
    return _orderer.cbegin();
}

QAO_OrdererConstIterator QAO_Runtime::cend() const {
    return _orderer.cend();
}

QAO_OrdererConstReverseIterator QAO_Runtime::crbegin() const {
    return _orderer.crbegin();
}

QAO_OrdererConstReverseIterator QAO_Runtime::crend() const {
    return _orderer.crend();
}

// Pack/Unpack state:
util::PacketBase& operator<<(util::PacketBase& packet, const QAO_Runtime& self) {
    packet << self._step_counter << self._iteration_ordinal << std::int32_t{self._current_event};
    return packet;
}

util::PacketBase& operator>>(util::PacketBase& packet, QAO_Runtime& self) {
    std::int32_t currentEvent;
    packet >> self._step_counter >> self._iteration_ordinal >> currentEvent;
    self._current_event = static_cast<decltype(self._current_event)>(currentEvent);
    return packet;
}

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>