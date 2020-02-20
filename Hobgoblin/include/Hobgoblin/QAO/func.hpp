#ifndef UHOBGOBLIN_QAO_FUNC_HPP
#define UHOBGOBLIN_QAO_FUNC_HPP

#include <Hobgoblin/QAO/id.hpp>
#include <Hobgoblin/QAO/runtime.hpp>

#include <cassert>
#include <utility>

#include <Hobgoblin/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

template <class T, class ... Args>
T* QAO_Create(QAO_Runtime& runtime, Args&&... args) {
	return runtime.addObject(std::make_unique<T>(std::forward<Args>(args)...));
}

inline void QAO_Destroy(QAO_Base* obj) {
    assert(obj);
    QAO_Runtime* runtime = obj->getRuntime();
    assert(runtime);
    runtime->eraseObject(obj->getId());
}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_QAO_FUNC_HPP