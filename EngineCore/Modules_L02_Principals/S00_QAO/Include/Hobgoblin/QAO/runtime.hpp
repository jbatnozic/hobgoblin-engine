#ifndef UHOBGOBLIN_QAO_RUNTIME_HPP
#define UHOBGOBLIN_QAO_RUNTIME_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/QAO/config.hpp>
#include <Hobgoblin/QAO/id.hpp>
#include <Hobgoblin/QAO/orderer.hpp>
#include <Hobgoblin/QAO/registry.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/No_copy_no_move.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <typeinfo>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace qao {

constexpr std::int32_t QAO_ALL_EVENT_FLAGS = 0xFFFFFFFF;

class QAO_Base;
class QAO_Runtime;

class QAO_RuntimeRef {
public:
    QAO_RuntimeRef() noexcept = default;

    QAO_RuntimeRef(std::nullptr_t) noexcept
        : QAO_RuntimeRef{}
    {
    }

    QAO_RuntimeRef(QAO_Runtime& runtime) noexcept
        : _runtime{&runtime}
        , _isOwning{true}
    {
    }

    QAO_RuntimeRef(QAO_Runtime* runtime) noexcept
        : _runtime{runtime}
        , _isOwning{true}
    {
    }

    QAO_Runtime* ptr() const noexcept {
        return _runtime;
    }

    bool isOwning() const noexcept {
        return _isOwning;
    }

private:
    QAO_Runtime* _runtime = nullptr;
    bool _isOwning = false;

    friend class QAO_Runtime;
};

class QAO_Runtime : NO_COPY, NO_MOVE {
public:
    QAO_Runtime();
    QAO_Runtime(util::AnyPtr userData);
    ~QAO_Runtime();

    QAO_RuntimeRef nonOwning();

    // Object manipulation
    void addObject(std::unique_ptr<QAO_Base> obj);
    void addObjectNoOwn(QAO_Base& object);

    void addObject(std::unique_ptr<QAO_Base> obj, QAO_GenericId specififcId);
    void addObjectNoOwn(QAO_Base& object, QAO_GenericId specififcId);

    std::unique_ptr<QAO_Base> releaseObject(QAO_Base* obj);
    // TODO: Name should be destroyObject
    void eraseObject(QAO_Base* obj);

    void destroyAllOwnedObjects();

    QAO_Base* find(const std::string& name) const;
    QAO_Base* find(QAO_GenericId id) const;

    template <class T>
    T* find(QAO_Id<T> id) const;

    void updateExecutionPriorityForObject(QAO_Base* object, int new_priority);

    // Execution
    void startStep();
    void advanceStep(bool& done, std::int32_t eventFlags = QAO_ALL_EVENT_FLAGS);
    QAO_Event::Enum getCurrentEvent() const;

    // Other
    PZInteger getObjectCount() const noexcept;
    bool ownsObject(const QAO_Base* object) const;

    // User data
    void setUserData(std::nullptr_t);

    template <class T>
    void setUserData(T* value);

    template <class T>
    T* getUserData() const;

    template <class T>
    T* getUserDataOrThrow() const;

    // Orderer/instance iterations:
    QAO_OrdererIterator begin();
    QAO_OrdererIterator end();

    QAO_OrdererReverseIterator rbegin();
    QAO_OrdererReverseIterator rend();

    QAO_OrdererConstIterator cbegin() const;
    QAO_OrdererConstIterator cend() const;

    QAO_OrdererConstReverseIterator crbegin() const;
    QAO_OrdererConstReverseIterator crend() const;

    // Pack/Unpack state:
    friend util::Packet& operator<<(util::PacketExtender& packet, const QAO_Runtime& self);
    friend util::Packet& operator>>(util::PacketExtender& packet, QAO_Runtime& self);

private:
    qao_detail::QAO_Registry _registry;
    qao_detail::QAO_Orderer _orderer;
    std::int64_t _step_counter;
    QAO_Event::Enum _current_event;
    QAO_OrdererIterator _step_orderer_iterator;
    util::AnyPtr _user_data;
};

template<class T>
T* QAO_Runtime::find(QAO_Id<T> id) const {
    return static_cast<T*>(find(QAO_GenericId{id}));
}

template <class T>
void QAO_Runtime::setUserData(T* value) {
    _user_data.reset(value);
}

template <class T>
T* QAO_Runtime::getUserData() const {
    return _user_data.get<T>();
}

template <class T>
T* QAO_Runtime::getUserDataOrThrow() const {
    return _user_data.getOrThrow<T>();
}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_RUNTIME_HPP