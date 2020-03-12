#ifndef UHOBGOBLIN_RN_NODE_HPP
#define UHOBGOBLIN_RN_NODE_HPP

#include <Hobgoblin/Common.hpp>

#include <initializer_list>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

class RN_ReceiverSet {
public:


private:
    std::initializer_list<PZInteger> _receivers;
};

constexpr int XYZ = sizeof(RN_ReceiverSet);

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_NODE_HPP