#ifndef UHOBGOBLIN_QAO_RUNTIME_HPP
#define UHOBGOBLIN_QAO_RUNTIME_HPP

#include <Hobgoblin_include/QAO/config.hpp>
#include <Hobgoblin_include/QAO/id.hpp>
#include <Hobgoblin_include/QAO/orderer.hpp>
#include <Hobgoblin_include/QAO/registry.hpp>
#include <Hobgoblin_include/Utility/NoCopyNoMove.hpp>
#include <Hobgoblin_include/Utility/Passkey.hpp>

#include <cstdint>
#include <memory>
#include <string>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

struct QAO_UserData {
};

class QAO_Base;

class QAO_Runtime : NO_COPY, NO_MOVE {
public:
    ~QAO_Runtime();

    // Object manipulation
    template <class T>
    QAO_Id<T> addObject(std::unique_ptr<T> object);

    template <class T>
    QAO_Id<T> addObjectNoOwn(T& object);

    std::unique_ptr<QAO_Base> releaseObject(const QAO_GenericId& id);
    void eraseObject(const QAO_GenericId& id);

    QAO_Base* findObjectWithName(const std::string& name) const;
    QAO_Base* findObjectWithId(const QAO_GenericId& id) const;

    // Execution
    void startStep();
    void advanceStep(bool& done);
    QAO_Event::Enum getCurrentEvent();

    // Other
    void setUserData(QAO_UserData* user_data);
    QAO_UserData* getUserData() const noexcept;
    // TODO Orderer iterations
    int getObjectCount() const noexcept;

    // Internal methods are not part of API
    void _internal_updateExecutionPriorityForObject(QAO_Base& object, int new_priority, util::Passkey<QAO_Base>);

private:
    detail::QAO_Registry _registry;
    detail::QAO_Orderer _orderer;
    std::int64_t _step_counter;
    QAO_Event::Enum _current_event;
    QAO_OrdererIterator _step_orderer_iterator;
    QAO_UserData* _user_data;

    detail::QAO_SerialIndexPair addObjectImpl(std::unique_ptr<QAO_Base> object);
    detail::QAO_SerialIndexPair addObjectNoOwnImpl(QAO_Base& object);
};

template <class T>
QAO_Id<T> QAO_Runtime::addObject(std::unique_ptr<T> object) {
    const auto pair = addObjectImpl(std::move(object));
    return QAO_GenericId{this, pair.serial, pair.index}.castPtr<T>();
}

template <class T>
QAO_Id<T> QAO_Runtime::addObjectNoOwn(T& object) {
    const auto pair = addObjectNoOwnImpl(object);
    return QAO_GenericId{this, pair.serial, pair.index}.castPtr<T>();
}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_QAO_RUNTIME_HPP