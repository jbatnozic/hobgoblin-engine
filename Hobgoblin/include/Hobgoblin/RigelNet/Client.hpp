#ifndef UHOBGOBLIN_RN_CLIENT_HPP
#define UHOBGOBLIN_RN_CLIENT_HPP

#include <Hobgoblin/RigelNet/Node.hpp>
#include <Hobgoblin/RigelNet/Connector.hpp>

#include <chrono>
#include <cstdint>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

struct RN_RemoteInfo;

template <class T, class U>
class RN_Client : public RN_Node {
public:
    RN_Client(RN_NodeType type);

};

template <class T, class U>
RN_Client<T, U>::RN_Client(RN_NodeType type)
    : RN_Node{type}
{
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_CLIENT_HPP
