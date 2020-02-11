#ifndef UHOBGOBLIN_QAO_ID_HPP
#define UHOBGOBLIN_QAO_ID_HPP

#include <cstdint>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

class QAO_Base;
class QAO_Registry;
class QAO_Runtime;

class QAO_GenericId {
public:
    QAO_GenericId();
    QAO_GenericId(const QAO_Base* obj);
    QAO_GenericId(const QAO_Base& obj);
    QAO_GenericId(std::nullptr_t p);

    bool operator==(const QAO_GenericId& other) const;
    bool operator!=(const QAO_GenericId& other) const;

    QAO_Base* operator->() const;
    QAO_Base& operator*() const;

    // Copy:
    QAO_GenericId(const QAO_GenericId &other) = default;
    QAO_GenericId& operator=(const QAO_GenericId &other) = default;

    // Move:
    QAO_GenericId(QAO_GenericId &&other) = default;
    QAO_GenericId& operator=(QAO_GenericId &&other) = default;

    // Utility:
    QAO_Base* ptr() const;
    int getIndex() const noexcept;
    std::int64_t getSerial() const noexcept;
    QAO_Runtime* getRuntime() const noexcept;

    template <class T>
    T* castPtr() const {
        return static_cast<T*>(ptr());
    }

protected:
    friend class QAO_Runtime;
    QAO_GenericId(QAO_Runtime* runtime, std::int64_t serial, int index);

private:
    QAO_Runtime* _runtime;
    std::int64_t _serial;
    int _index;
};

template <class T>
class QAO_Id : public QAO_GenericId {
public:
    QAO_Id(): QAO_GenericId() { }
    QAO_Id(const T& obj): QAO_GenericId(obj) { }
    QAO_Id(const T* obj): QAO_GenericId(obj) { }
    QAO_Id(std::nullptr_t p): QAO_GenericId(p) { }

    // Copy:
    QAO_Id(const QAO_Id<T> &other) = default;
    QAO_Id& operator=(const QAO_Id<T> &other) = default;

    // Move:
    QAO_Id(QAO_Id &&other) = default;
    QAO_Id& operator=(QAO_Id &&other) = default;

    operator QAO_GenericId() {
        return QAO_GenericId{_runtime, _serial, _index};
    }

    T* ptr() const {
        return static_cast<T*>(QAO_GenericId::ptr());
    }

    T& operator*() const {
        return (*ptr());
    }

    T* operator->() const {     
        return ptr();
    }
};

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_QAO_ID_HPP