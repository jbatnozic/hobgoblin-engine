#ifndef UHOBGOBLIN_QAO_PROPERTY_HPP
#define UHOBGOBLIN_QAO_PROPERTY_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

#include <utility>

HOBGOBLIN_NAMESPACE_START
namespace qao {

class QAO_Base;

template <class T>
class QAO_Property {
public:
    T v;

    template <class ... Args>
    QAO_Property(QAO_Base* base, Args&&... args)
        : v{std::forward<Args>(args)...}
    {
        // base->add(this, Read, Write...)
    }

private:
    static void Read() {
    }

    static void Write() {
    }

};

#define QAO_PROPDECL(type, name) QAO_Property<type> name; type& _##name
#define QAO_PROPINIT(name, ...) name{this, __VA_ARGS__}, _##name{name.v}
// TODO Fix propinit for no arguments to VA_ARGS

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_PROPERTY_HPP
