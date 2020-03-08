
#include <Hobgoblin/QAO/base.hpp>
#include <Hobgoblin/QAO/runtime.hpp>
#include <Hobgoblin/Utility/Passkey.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

QAO_Base::QAO_Base(int type_id_, int execution_priority_, const std::string & name)
    : _instance_name{name}
    , _runtime{nullptr}
    , _step_ordinal{}
    , _type_id{type_id_}
    , _execution_priority{execution_priority_}
{
}

QAO_Base::~QAO_Base() {
    if (_runtime) {
        _runtime->releaseObject(_this_id).reset();
    }
}

QAO_Runtime* QAO_Base::getRuntime() const noexcept {
    return _runtime;
}

int QAO_Base::getExecutionPriority() const noexcept {
    return _execution_priority;
}

std::string QAO_Base::getName() const {
    return _instance_name;
}

QAO_GenericId QAO_Base::getId() const noexcept {
    return _this_id;
}

QAO_Base* QAO_Base::clone() const {
    return nullptr;
}

void QAO_Base::setExecutionPriority(int new_priority) {
    if (_execution_priority == new_priority) {
        return;
    }
    if (_runtime != nullptr) {
        _runtime->updateExecutionPriorityForObject(this, new_priority);
    } else {
        _execution_priority = new_priority;
    }
}

void QAO_Base::setName(const std::string& new_name) {
    _instance_name = new_name;
}

// Internal

void QAO_Base::_internal_callEvent(QAO_Event::Enum ev, QAO_Runtime& rt, util::Passkey<QAO_Runtime>) {
    using EventHandlerPointer = void(QAO_Base::*)();
    static constexpr EventHandlerPointer handlers[QAO_Event::Count] = {
        &QAO_Base::eventFrameStart,
        &QAO_Base::eventPreUpdate,
        &QAO_Base::eventUpdate,
        &QAO_Base::eventPostUpdate,
        &QAO_Base::eventDraw1,
        &QAO_Base::eventDraw2,
        &QAO_Base::eventDrawGUI,
        &QAO_Base::eventRender
    };
    assert(ev >= 0 && ev < QAO_Event::Count);
    (this->*handlers[ev])();
}

void QAO_Base::_internal_setOrdererIterator(QAO_OrdererIterator orderer_iterator, util::Passkey<QAO_Runtime>) {
    _orderer_iterator = orderer_iterator;
}

void QAO_Base::_internal_setExecutionPriority(int priority, util::Passkey<QAO_Runtime>) {
    _execution_priority = priority;
}

std::int64_t QAO_Base::_internal_getStepOrdinal(util::Passkey<QAO_Runtime>) const noexcept {
    return _step_ordinal;
}

void QAO_Base::_internal_setStepOrdinal(std::int64_t step_oridinal, util::Passkey<QAO_Runtime>) {
    _step_ordinal = step_oridinal;
}

void QAO_Base::_internal_setRuntime(QAO_Runtime* runtime, util::Passkey<QAO_Runtime>) {
    _runtime = runtime;
}

void QAO_Base::_internal_setThisId(QAO_GenericId id, util::Passkey<QAO_Runtime>) {
    _this_id = id;
}

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>