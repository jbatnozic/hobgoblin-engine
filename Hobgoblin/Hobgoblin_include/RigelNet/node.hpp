#ifndef UHOBGOBLIN_RN_NODE_HPP
#define UHOBGOBLIN_RN_NODE_HPP

#include <Hobgoblin_include/Common.hpp>
#include <Hobgoblin_include/RigelNet/handlers.hpp>
#include <Hobgoblin_include/RigelNet/packet.hpp>

#include <cstdint>
#include <deque>
#include <type_traits>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

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
    /*template <class ... Args>
    void compose(PZInteger receiver, Args&&... args) {

    }*/

    template <class T>
    T extractArgument();

    template <class ... Args>
    friend void UHOBGOBLIN_RN_ComposeImpl(RN_Node& node, int receiver, int handlerId, Args... args);

    template <class T>
    friend typename std::remove_reference<T>::type UHOBGOBLIN_RN_ExtractArg(RN_Node& node);
};

template <class T>
T RN_Node::extractArgument() {
    T rv;
    // ???
    // return rv;
    return T{};
}

template <class ... Args>
void UHOBGOBLIN_RN_ComposeImpl(RN_Node& node, int receiver, std::int32_t handlerId, Args... args) {

}

template <class T>
typename std::remove_reference<T>::type UHOBGOBLIN_RN_ExtractArg(RN_Node& node) {
    return node.extractArgument<std::remove_reference<T>::type>();
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_NODE_HPP