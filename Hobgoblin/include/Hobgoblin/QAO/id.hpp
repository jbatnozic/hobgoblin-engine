#ifndef UHOBGOBLIN_QAO_ID_HPP
#define UHOBGOBLIN_QAO_ID_HPP

#include <Hobgoblin/common.hpp>

#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

class QAO_Base;
class QAO_Registry;
class QAO_Runtime;

template <class T>
class QAO_Id;

class QAO_GenericId {
public:
    QAO_GenericId();
    QAO_GenericId(std::nullptr_t p);

    bool operator==(const QAO_GenericId& other) const;
    bool operator!=(const QAO_GenericId& other) const;

    // Copy:
    QAO_GenericId(const QAO_GenericId &other) = default;
    QAO_GenericId& operator=(const QAO_GenericId &other) = default;

    // Move:
    QAO_GenericId(QAO_GenericId &&other) = default;
    QAO_GenericId& operator=(QAO_GenericId &&other) = default;

    // Utility:
    PZInteger getIndex() const noexcept;
    std::int64_t getSerial() const noexcept;
    bool isNull() const noexcept;

    template<class T>
    QAO_Id<T> cast() const noexcept;

protected:
    friend class QAO_Runtime;
    QAO_GenericId(std::int64_t serial, int index);

private:
    std::int64_t _serial;
    PZInteger _index;
};

template <class T>
class QAO_Id : public QAO_GenericId {
public:
    QAO_Id(): QAO_GenericId() { }
    QAO_Id(std::nullptr_t p): QAO_GenericId(p) { }

    // Copy:
    QAO_Id(const QAO_Id<T> &other) = default;
    QAO_Id& operator=(const QAO_Id<T> &other) = default;

    // Move:
    QAO_Id(QAO_Id &&other) = default;
    QAO_Id& operator=(QAO_Id &&other) = default;

    operator QAO_GenericId() {
        return QAO_GenericId{_serial, _index};
    }

protected:
    friend class QAO_GenericId;
    QAO_Id(std::int64_t serial, PZInteger index)
        : QAO_GenericId{serial, index} 
    {
    }

};

template<class T>
QAO_Id<T> QAO_GenericId::cast() const noexcept {
    return QAO_Id<T>{_serial, _index};
}

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_QAO_ID_HPP