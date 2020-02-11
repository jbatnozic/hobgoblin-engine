#ifndef UHOBGOBLIN_QAO_FUNC_HPP
#define UHOBGOBLIN_QAO_FUNC_HPP

#include <Hobgoblin_include/QAO/id.hpp>
#include <Hobgoblin_include/QAO/runtime.hpp>

#include <utility>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

template <class T, class ... Args>
inline QAO_Id<T> QAO_Create(QAO_Runtime& runtime, Args&&... args) {
	return runtime.addObject(std::make_unique<T>(std::forward<Args>(args)...));
}

inline void QAO_Destroy(const QAO_GenericId& id) {
    QAO_Runtime* runtime = id.getRuntime();
    if (runtime) {
        runtime->eraseObject(id);
    }
}

} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_QAO_FUNC_HPP