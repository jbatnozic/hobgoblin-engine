#ifndef UHOBGOBLIN_QAO_FUNC_HPP
#define UHOBGOBLIN_QAO_FUNC_HPP

#include <Hobgoblin/QAO/id.hpp>
#include <Hobgoblin/QAO/runtime.hpp>

#include <cassert>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

template <class T, class ... Args>
T* QAO_Create(QAO_Runtime& runtime, Args&&... args) {
	return runtime.addObject(std::make_unique<T>(std::forward<Args>(args)...));
    //return new 
}

inline void QAO_Destroy(QAO_Base* obj) {
    assert(obj);
    QAO_Runtime* runtime = obj->getRuntime();
    assert(runtime);
    runtime->eraseObject(obj->getId());
}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_FUNC_HPP