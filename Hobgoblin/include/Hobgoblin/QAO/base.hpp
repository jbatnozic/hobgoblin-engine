#ifndef UHOBGOBLIN_QAO_BASE_HPP
#define UHOBGOBLIN_QAO_BASE_HPP

#include <Hobgoblin/QAO/config.hpp>
#include <Hobgoblin/QAO/orderer.hpp>
#include <Hobgoblin/QAO/id.hpp>
#include <Hobgoblin/Utility/NoCopyNoMove.hpp>
#include <Hobgoblin/Utility/Passkey.hpp>

#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

class QAO_Base;
class QAO_Runtime;

class QAO_Base : NO_COPY, NO_MOVE {
public:
    QAO_Base() = delete;
    QAO_Base(int type_id, int execution_priority = 0, const std::string& name = "Undefined");
    virtual ~QAO_Base() = 0;
    
    QAO_Runtime* getRuntime() const noexcept;

    void setExecutionPriority(int priority);
    int getExecutionPriority() const noexcept;

    void setName(const std::string& name);
    std::string getName() const;

    QAO_GenericId getId() const noexcept;

    virtual QAO_Base* clone() const;

    // TODO Give common prefix to passkey-restricted methods
    // Internal methods are not part of API
    void _internal_callEvent(QAO_Event::Enum ev, QAO_Runtime& rt, util::Passkey<QAO_Runtime>);
    void _internal_setOrdererIterator(QAO_OrdererIterator orderer_iterator, util::Passkey<QAO_Runtime>);
    void _internal_setExecutionPriority(int priority, util::Passkey<QAO_Runtime>);
    std::int64_t _internal_getStepOrdinal(util::Passkey<QAO_Runtime>) const noexcept;
    void _internal_setStepOrdinal(std::int64_t step_oridinal, util::Passkey<QAO_Runtime>);
    void _internal_setRuntime(QAO_Runtime* runtime, util::Passkey<QAO_Runtime>);
    void _internal_setThisId(QAO_GenericId id, util::Passkey<QAO_Runtime>);

protected:
    virtual void eventFrameStart() { }
    virtual void eventPreUpdate()  { }
    virtual void eventUpdate()     { }
    virtual void eventPostUpdate() { }
    virtual void eventDraw1()      { }
    virtual void eventDraw2()      { }
    virtual void eventDrawGUI()    { }
    virtual void eventRender()     { }

private:
    std::string _instance_name;
    QAO_GenericId _this_id;
    QAO_OrdererIterator _orderer_iterator;
    QAO_Runtime* _runtime;
    std::int64_t _step_ordinal;
    int _type_id;
    int _execution_priority;
};

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_BASE_HPP