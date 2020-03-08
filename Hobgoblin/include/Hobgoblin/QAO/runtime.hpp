#ifndef UHOBGOBLIN_QAO_RUNTIME_HPP
#define UHOBGOBLIN_QAO_RUNTIME_HPP

#include <Hobgoblin/common.hpp>
#include <Hobgoblin/QAO/config.hpp>
#include <Hobgoblin/QAO/id.hpp>
#include <Hobgoblin/QAO/orderer.hpp>
#include <Hobgoblin/QAO/registry.hpp>
#include <Hobgoblin/Utility/NoCopyNoMove.hpp>
#include <Hobgoblin/Utility/Passkey.hpp>

#include <cstdint>
#include <memory>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

constexpr std::int32_t QAO_ALL_EVENT_FLAGS = 0xFFFFFFFF;

struct QAO_UserData {
};

class QAO_Base;

class QAO_Runtime : NO_COPY, NO_MOVE {
public:
    QAO_Runtime();
    ~QAO_Runtime();

    // Object manipulation
    template <class T>
    T* addObject(std::unique_ptr<T> object);

    template <class T>
    T* addObjectNoOwn(T& object);

    std::unique_ptr<QAO_Base> releaseObject(QAO_GenericId id);
    std::unique_ptr<QAO_Base> releaseObject(QAO_Base* obj);
    void eraseObject(QAO_GenericId id);
    void eraseObject(QAO_Base* obj);

    QAO_Base* find(const std::string& name) const;
    QAO_Base* find(QAO_GenericId id) const;

    template<class T>
    T* find(QAO_Id<T> id) const;

    void updateExecutionPriorityForObject(QAO_GenericId id, int new_priority);
    void updateExecutionPriorityForObject(QAO_Base* object, int new_priority);

    // Execution
    void startStep();
    void advanceStep(bool& done, std::int32_t eventFlags = QAO_ALL_EVENT_FLAGS);
    QAO_Event::Enum getCurrentEvent();

    // Other
    void setUserData(QAO_UserData* user_data);
    QAO_UserData* getUserData() const noexcept;  
    PZInteger getObjectCount() const noexcept;
    std::int64_t getIterationOrdinal() const noexcept {
        return _iteration_ordinal; // TODO
    }

    // TODO Orderer iterations

private:
    detail::QAO_Registry _registry;
    detail::QAO_Orderer _orderer;
    std::int64_t _step_counter;
    std::int64_t _iteration_ordinal;
    QAO_Event::Enum _current_event;
    QAO_OrdererIterator _step_orderer_iterator;
    QAO_UserData* _user_data;

    QAO_Base* addObjectImpl(std::unique_ptr<QAO_Base> object);
    QAO_Base* addObjectNoOwnImpl(QAO_Base& object);
};

template <class T>
T* QAO_Runtime::addObject(std::unique_ptr<T> object) {
    return static_cast<T*>(addObjectImpl(std::move(object)));
}

template <class T>
T* QAO_Runtime::addObjectNoOwn(T& object) {
    return static_cast<T*>(addObjectNoOwnImpl(object));
}

template<class T>
T* QAO_Runtime::find(QAO_Id<T> id) const {
    return static_cast<T>(find(QAO_GenericId{id}));
}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_QAO_RUNTIME_HPP