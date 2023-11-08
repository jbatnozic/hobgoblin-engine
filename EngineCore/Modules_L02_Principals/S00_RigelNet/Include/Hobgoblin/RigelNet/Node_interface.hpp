#ifndef UHOBGOBLIN_RN_NODE_INTERFACE_HPP
#define UHOBGOBLIN_RN_NODE_INTERFACE_HPP

#include <Hobgoblin/RigelNet/Configuration.hpp>
#include <Hobgoblin/RigelNet/Events.hpp>
#include <Hobgoblin/RigelNet/Handlermgmt.hpp>
#include <Hobgoblin/RigelNet/Telemetry.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include <cassert>
#include <functional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

enum class RN_UpdateMode {
    Receive,
    Send
};

class RN_ClientInterface;
class RN_ServerInterface;

class RN_NodeInterface {
public:
    virtual ~RN_NodeInterface();

    virtual bool pollEvent(RN_Event& ev) = 0;

    virtual bool isServer() const noexcept = 0;

    virtual RN_Protocol getProtocol() const noexcept = 0;

    virtual RN_NetworkingStack getNetworkingStack() const noexcept = 0;

    virtual RN_Telemetry update(RN_UpdateMode updateMode) = 0;

    //! Call the provided function if this node is a Client.
    void callIfClient(std::function<void(RN_ClientInterface& client)> func);

    //! Call the provided funtion if this node is a Server.
    void callIfServer(std::function<void(RN_ServerInterface& client)> func);

    void setUserData(std::nullptr_t);

    template <class T>
    void setUserData(T* value);

    template <class T>
    T* getUserData() const;

    template <class T>
    T* getUserDataOrThrow() const;

private:
    virtual void _compose(RN_ComposeForAllType receiver, const void* data, std::size_t sizeInBytes) = 0;
    virtual void _compose(PZInteger receiver, const void* data, std::size_t sizeInBytes) = 0;
    virtual util::Packet* _getCurrentPacket() = 0;
    virtual void _setUserData(util::AnyPtr userData) = 0;
    virtual util::AnyPtr _getUserData() const = 0;

    template <class taRecepients, class ...taArgs>
    friend void UHOBGOBLIN_RN_ComposeImpl(RN_NodeInterface& node, 
                                          taRecepients&& recepients, 
                                          detail::RN_HandlerId handlerId, 
                                          taArgs... args);

    template <class T>
    friend typename std::remove_reference<T>::type UHOBGOBLIN_RN_ExtractArg(RN_NodeInterface& node);
};

template <class T>
void RN_NodeInterface::setUserData(T* value) {
    _setUserData(value);
}

template <class T>
T* RN_NodeInterface::getUserData() const {
    util::AnyPtr p = _getUserData();
    return p.get<T>();
}

template <class T>
T* RN_NodeInterface::getUserDataOrThrow() const {
    util::AnyPtr p = _getUserData();
    return p.getOrThrow<T>();
}

//! Function for internal use.
template <class taRecepients,  class ...taArgs>
void UHOBGOBLIN_RN_ComposeImpl(RN_NodeInterface& node,
                               taRecepients&& recepients,
                               detail::RN_HandlerId handlerId,
                               taArgs... args) {
    util::Packet packet;
    packet.insert(handlerId);
    util::PackArgs(packet, std::forward<taArgs>(args)...);

    if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<taRecepients>>, RN_ComposeForAllType>) {
        node._compose(RN_ComposeForAllType{}, packet.getData(), packet.getDataSize());
    }
    else if constexpr (std::is_convertible_v<taRecepients, PZInteger>) {    
        node._compose(std::forward<taRecepients>(recepients),
                      packet.getData(), 
                      packet.getDataSize());
    } 
    else {
        for (PZInteger i : std::forward<taRecepients>(recepients)) {
            node._compose(i, packet.getData(), packet.getDataSize());
        }
    }
}

//! Function for internal use.
template <class taArgType>
typename std::remove_reference<taArgType>::type UHOBGOBLIN_RN_ExtractArg(RN_NodeInterface& node) {
    auto* pack = node._getCurrentPacket();
    assert(pack);
    return pack->extractOrThrow<typename std::remove_reference<taArgType>::type>();
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_NODE_INTERFACE_HPP