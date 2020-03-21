#ifndef UHOBGOBLIN_QAO_FUNC_HPP
#define UHOBGOBLIN_QAO_FUNC_HPP

#include <Hobgoblin/Utility/Exceptions.hpp>
#include <Hobgoblin/QAO/id.hpp>
#include <Hobgoblin/QAO/runtime.hpp>

#include <cassert>
#include <utility>
#include <variant>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

template <class T, class ... Args>
T* QAO_PCreate(Args&&... args) {
    T* const object = new T(std::forward<Args>(args)...);

    try {
        if (object->getRuntime() != nullptr ||
            object->getRuntime()->ownsObject(object)) {
            throw util::TracedLogicError(""); // TODO
        }
    }
    catch (...) {
        delete object;
        throw;
    }

    return object;
}

template <class T, class ... Args>
std::unique_ptr<T> QAO_UPCreate(Args&&... args) {
    // TODO
}

template <class T, class ... Args>
QAO_Id<T> QAO_IPCreate(Args&&... args) {
    // TODO
}

inline void QAO_Destroy(QAO_Base* obj) {
    // TODO
}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_FUNC_HPP