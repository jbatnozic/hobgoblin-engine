#ifndef UHOBGOBLIN_RN_NODE_INTERFACE_HPP
#define UHOBGOBLIN_RN_NODE_INTERFACE_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

enum class RN_Protocol {
    None, //! No protocol implemented (only on dummy nodes).
    TCP,  //! Transmission control protocol (NOT YET IMPLEMENTED).
    UDP   //! User datagram protocol.
};

enum class RN_UpdateMode {
    Receive,
    Send
};

class RN_NodeInterface {
public:
    virtual bool pollEvent(RN_Event& ev) = 0;

    virtual bool isServer() const noexcept = 0;

    virtual RN_Protocol getProtocol() const noexcept = 0;

    virtual void update(RN_UpdateMode updateMode) = 0;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_NODE_INTERFACE_HPP