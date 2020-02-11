
#include <Hobgoblin_include/QAO/base.hpp>
#include <Hobgoblin_include/QAO/runtime.hpp>

#include <cassert>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

QAO_Runtime::~QAO_Runtime() {
    for (int i = 0; i < _registry.size(); i += 1) {
        if (!_registry.isSlotEmpty(i)) {
            QAO_Base* obj = _registry.objectAt(i);
            eraseObject(obj);
        }
    }
}

detail::QAO_SerialIndexPair QAO_Runtime::addObjectImpl(std::unique_ptr<QAO_Base> object) {
    QAO_Base* object_raw = object.get();
    const auto reg_pair = _registry.insert(std::move(object));

    const auto ord_pair = _orderer.insert(object_raw); // first = iterator, second = added_new
    assert(ord_pair.second);

    object_raw->_internal_setRuntime(this, PASSKEY);
    object_raw->_internal_setThisId(QAO_GenericId{this, reg_pair.serial, reg_pair.index}, PASSKEY);
    object_raw->_internal_setOrdererIterator(ord_pair.first, PASSKEY);
    object_raw->_internal_setStepOrdinal(-1, PASSKEY);

    return reg_pair;
}

detail::QAO_SerialIndexPair QAO_Runtime::addObjectNoOwnImpl(QAO_Base& object) {
    // TODO Resolve code duplication
    QAO_Base* object_raw = &object;
    const auto reg_pair = _registry.insertNoOwn(object_raw);

    const auto ord_pair = _orderer.insert(object_raw); // first = iterator, second = added_new
    assert(ord_pair.second);

    object_raw->_internal_setRuntime(this, PASSKEY);
    object_raw->_internal_setThisId(QAO_GenericId{this, reg_pair.serial, reg_pair.index}, PASSKEY);
    object_raw->_internal_setOrdererIterator(ord_pair.first, PASSKEY);
    object_raw->_internal_setStepOrdinal(-1, PASSKEY);

    return reg_pair;
}

std::unique_ptr<QAO_Base> QAO_Runtime::releaseObject(const QAO_GenericId& id) {
    const auto index = id.getIndex();
    const auto serial = id.getSerial();

    QAO_Base* obj_raw = id.ptr();
    assert(obj_raw);

    std::unique_ptr<QAO_Base> rv = _registry.release(index); // nullptr if object wasn't owned

    // If current _step_orderer_iterator points to released object, advance it first
    if (_step_orderer_iterator != _orderer.end() && *_step_orderer_iterator == obj_raw) {
        _step_orderer_iterator = std::next(_step_orderer_iterator);
    }
    _orderer.erase(obj_raw);

    obj_raw->_internal_setRuntime(nullptr, PASSKEY);
    obj_raw->_internal_setThisId(nullptr, PASSKEY);

    return rv;
}

void QAO_Runtime::eraseObject(const QAO_GenericId& id) {
    releaseObject(id).reset();
}

QAO_Base* QAO_Runtime::findObjectWithName(const std::string& name) const {
    return nullptr; // TODO
}

QAO_Base* QAO_Runtime::findObjectWithId(const QAO_GenericId& id) const {
    const auto index = id.getIndex();
    const auto serial = id.getSerial();
    if (_registry.serialAt(index) != serial) {
        return nullptr;
    }
    return _registry.objectAt(index);
}

// Execution

void QAO_Runtime::startStep() {
    _current_event = QAO_Event::FrameStart;
    _step_orderer_iterator = _orderer.begin();
}

void QAO_Runtime::advanceStep(bool& done) {
    done = false;
    QAO_OrdererIterator& curr = _step_orderer_iterator;

    //-----------------------------------------//
    for (int i = _current_event; i < QAO_Event::Count; i += 1) {          
        auto ev = static_cast<QAO_Event::Enum>(i);
        _current_event = ev;

        while (curr != _orderer.end()) {
            QAO_Base* instance = *curr;
            curr = std::next(curr);
            if (instance->_internal_getStepOrdinal(PASSKEY) < _step_counter) {
                instance->_internal_callEvent(ev, PASSKEY);
                instance->_internal_setStepOrdinal(_step_counter, PASSKEY);
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

void QAO_Runtime::setUserData(QAO_UserData* user_data) {
    _user_data = user_data;
}

QAO_UserData* QAO_Runtime::getUserData() const noexcept {
    return _user_data;
}

// Internal

void QAO_Runtime::_internal_updateExecutionPriorityForObject(QAO_Base& object, int new_priority, util::Passkey<QAO_Base>) {
    _orderer.erase(&object);
    object._internal_setExecutionPriority(new_priority, PASSKEY);

    const auto ord_pair = _orderer.insert(&object); // first = iterator, second = added_new
    object._internal_setOrdererIterator(ord_pair.first, PASSKEY);
}

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>