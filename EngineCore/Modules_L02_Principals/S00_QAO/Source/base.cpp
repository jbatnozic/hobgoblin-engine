// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/QAO/base.hpp>
#include <Hobgoblin/Utility/Passkey.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace qao {

QAO_Base::QAO_Base(QAO_RuntimeRef runtimeRef, const std::type_info& typeInfo, int executionPriority, std::string name)
    : _instanceName{std::move(name)}
    , _typeInfo{typeInfo}
    , _execution_priority{executionPriority}
{
    QAO_Runtime* runtime = runtimeRef.ptr();
    if (runtime) {
        if (runtimeRef.isOwning()) {
            runtime->addObject(std::unique_ptr<QAO_Base>{this});
        }
        else {
            runtime->addObjectNoOwn(SELF);
        }
    }
}

QAO_Base::QAO_Base(QAO_RuntimeRef runtimeRef, const std::type_info& typeInfo, util::Packet& packet)
    : _typeInfo{typeInfo}
{
    packet >> _instanceName >> _context.id >> _execution_priority;
    if (!packet) {
        HG_THROW_TRACED(TracedRuntimeError, 0, "Deserialization failed.");
    }

    QAO_Runtime* runtime = runtimeRef.ptr();
    if (runtime) {
        if (runtimeRef.isOwning()) {
            runtime->addObject(std::unique_ptr<QAO_Base>{this}, _context.id);
        }
        else {
            runtime->addObjectNoOwn(SELF, _context.id);
        }
    }
}

QAO_Base::~QAO_Base() {
    if (_context.runtime) {
        _context.runtime->releaseObject(this).release(); // TODO PEP
    }
}

QAO_Runtime* QAO_Base::getRuntime() const noexcept {
    return _context.runtime;
}

int QAO_Base::getExecutionPriority() const noexcept {
    return _execution_priority;
}

std::string QAO_Base::getName() const {
    return _instanceName;
}

QAO_GenericId QAO_Base::getId() const noexcept {
    return _context.id;
}

const std::type_info& QAO_Base::getTypeInfo() const {
    return _typeInfo;
}

bool QAO_Base::message(int tag, util::AnyPtr context) {
    return false;
}

util::OutputStream& operator<<(util::OutputStreamExtender& ostream, const QAO_Base& self) {
    *ostream << self._instanceName << self._context.id << self._execution_priority;
    return *ostream;
}

void QAO_Base::setExecutionPriority(int new_priority) {
    if (_execution_priority == new_priority) {
        return;
    }
    if (_context.runtime != nullptr) {
        _context.runtime->updateExecutionPriorityForObject(this, new_priority);
    } else {
        _execution_priority = new_priority;
    }
}

void QAO_Base::setName(std::string newName) {
    _instanceName = std::move(newName);
}

// Private

void QAO_Base::_callEvent(QAO_Event::Enum ev) {
    using EventHandlerPointer = void(QAO_Base::*)();
    const EventHandlerPointer handlers[QAO_Event::EVENT_COUNT] = {
        &QAO_Base::_eventPreUpdate,
        &QAO_Base::_eventBeginUpdate,
        &QAO_Base::_eventUpdate1,
        &QAO_Base::_eventUpdate2,
        &QAO_Base::_eventEndUpdate,
        &QAO_Base::_eventPostUpdate,

        &QAO_Base::_eventPreDraw,
        &QAO_Base::_eventDraw1,
        &QAO_Base::_eventDraw2,
        &QAO_Base::_eventDrawGUI,
        &QAO_Base::_eventPostDraw,

        &QAO_Base::_eventDisplay
    };
    assert(ev >= 0 && ev < QAO_Event::EVENT_COUNT);
    (this->*handlers[ev])();
}

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
