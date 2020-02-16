#ifndef UHOBGOBLIN_RN_NODE_HPP
#define UHOBGOBLIN_RN_NODE_HPP

#include <Hobgoblin_include/common.hpp>
#include <Hobgoblin_include/RigelNet/packet.hpp>

#include <deque>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

enum Safety {
    RN_Unsfe,
    RN_Safe
};

class RN_Node {
public:
    virtual ~RN_Node() = 0 {}

    /*RN_Node(const RN_Node& rhs) = default;
    RN_Node& operator=(const RN_Node& rhs) = default;

    RN_Node(RN_Node&& rhs) = default;
    RN_Node& operator=(RN_Node&& rhs) = default;*/

    // Compose template

    bool pollEvent(int& ev);

    // virtual RN_NodeType getType() const;

protected:
    std::deque<int> _eventQueue;

    void  clearEvents();
    void  queueEvent(int ev);

    // virtual std::vector<RN_Packet*> getPacketsToComposeInto() const;

    template <class ... Args>
    void compose(bool safe, PZInteger receiver, Args&&... args) {

    }
    
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_NODE_HPP