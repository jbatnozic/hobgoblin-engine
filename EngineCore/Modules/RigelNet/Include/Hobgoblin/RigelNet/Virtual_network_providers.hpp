#ifndef UHOBGOBLIN_RN_VIRTUAL_NETWORK_PROVIDERS_HPP
#define UHOBGOBLIN_RN_VIRTUAL_NETWORK_PROVIDERS_HPP

#if 0 // Virtual network providers not supported for now

#include <Hobgoblin/Private/Pmacro_define.hpp>

#include <memory>

#define HOBGOBLIN_RN_ZEROTIER_SUPPORT // TODO temp.
#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
#include <ZTCpp/Events.hpp>
#include <ZTCpp/Ip_address.hpp>
#endif

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

enum class RN_VirtualNetworkProviderType {
    ZeroTier = 0 //! https://github.com/zerotier/libzt
};

class RN_VirtualNetworkProviderInterface {
public:
    //! Calls shutDown() automatically on destruction if it wasn't called
    //! already before.
    virtual ~RN_VirtualNetworkProviderInterface() = 0;

    //! Returns the type of the virtual network provider.
    virtual RN_VirtualNetworkProviderType getType() = 0;

    //! TODO(add comment)
    virtual void shutDown() = 0;

    //! If this is set to true (it's always false by default), shutting
    //! down the virtual network provider will also free all resources
    //! it needs to function. If you do this, you won't be able to start
    //! a new virtual network provider of the same type without restarting 
    //! the whole application.
    //! For some providers this may not be applicable, in which case it
    //! is a no-op.
    virtual void setWipeDataOnShutDown(bool aWipeDataOnShutDown) = 0;

    // TODO(update config)
};

std::unique_ptr<RN_VirtualNetworkProviderInterface> RN_StartVirtualNetworkProvider();

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif

#endif // !UHOBGOBLIN_RN_VIRTUAL_NETWORK_PROVIDERS_HPP