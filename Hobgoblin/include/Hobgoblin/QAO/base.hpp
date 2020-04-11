#ifndef UHOBGOBLIN_QAO_BASE_HPP
#define UHOBGOBLIN_QAO_BASE_HPP

#include <Hobgoblin/QAO/config.hpp>
#include <Hobgoblin/QAO/id.hpp>
#include <Hobgoblin/QAO/orderer.hpp>
#include <Hobgoblin/QAO/runtime.hpp>
#include <Hobgoblin/Utility/NoCopyNoMove.hpp>
#include <Hobgoblin/Utility/Passkey.hpp>

#include <string>
#include <typeinfo>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

class QAO_Runtime;

class QAO_Base : NO_COPY, NO_MOVE {
public:
    QAO_Base() = delete;
    QAO_Base(QAO_RuntimeRef runtimeRef, const std::type_info& typeInfo, int executionPriority, std::string name);
    virtual ~QAO_Base() = 0;
    
    QAO_Runtime* getRuntime() const noexcept;

    void setExecutionPriority(int priority);
    int getExecutionPriority() const noexcept;

    void setName(std::string newName);
    std::string getName() const;

    QAO_GenericId getId() const noexcept;

    const std::type_info& getTypeInfo() const {
        return _typeInfo; // TODO
    }

    virtual QAO_Base* clone() const;

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
    struct Context {
        std::int64_t stepOrdinal = 0;
        QAO_GenericId id;
        QAO_OrdererIterator ordererIterator;
        QAO_Runtime* runtime = nullptr;

        Context() = default;

        Context(std::int64_t stepOrdinal, QAO_GenericId id,
            QAO_OrdererIterator ordererIterator, QAO_Runtime* runtime)
            : stepOrdinal{stepOrdinal}
            , id{id}
            , ordererIterator{ordererIterator}
            , runtime{runtime}
        {
        }
    };

    std::string _instanceName;
    Context _context;
    const std::type_info& _typeInfo;
    int _execution_priority;

    void _callEvent(QAO_Event::Enum ev);

    friend class QAO_Runtime;
    friend class QAO_GenericId;
};

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_BASE_HPP