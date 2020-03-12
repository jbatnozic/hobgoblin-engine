#ifndef UHOBGOBLIN_RN_MACRO_HANDLERS_HPP
#define UHOBGOBLIN_RN_MACRO_HANDLERS_HPP

#include <Hobgoblin/RigelNet/Macro_extract_args.hpp>
#include <Hobgoblin/RigelNet/Macro_normalize_args.hpp>
#include <Hobgoblin/RigelNet/Macro_pass_args.hpp>
#include <Hobgoblin/RigelNet/Macro_pass_compose_args.hpp>

#define RN_NODE_IN_HANDLER() UHOBGOBLIN_RN_Reference_to_node

#define UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, ...) \
    void UHOBGOBLIN_RN_Handler_##_name_( \
        ::jbatnozic::hobgoblin::rn::RN_Node& RN_NODE_IN_HANDLER()  UHOBGOBLIN_RN_NORMALIZE_ARGS(/*nonconst*/, __VA_ARGS__) \
    )

#define UHOBGOBLIN_RN_GENERATE_HANDLER_PROXY(_name_, ...) \
    void UHOBGOBLIN_RN_HandlerProxy_##_name_(::jbatnozic::hobgoblin::rn::RN_Node& node) { \
        UHOBGOBLIN_RN_EXTRACT_ARGS(__VA_ARGS__) \
        UHOBGOBLIN_RN_Handler_##_name_(node  UHOBGOBLIN_RN_PASS_ARGS(__VA_ARGS__)); \
    }

#define UHOBGOBLIN_RN_INSTALL_HANDLER_PROXY(_name_) \
    ::jbatnozic::hobgoblin::rn::detail::RN_StaticHandlerInitializer \
    UHOBGOBLIN_RN_HandlerInit_##_name_{#_name_, UHOBGOBLIN_RN_HandlerProxy_##_name_}

#define UHOBGOBLIN_RN_GENERATE_COMPOSE_FUNCTION(_name_, ...) \
    void RN_Compose_##_name_(::jbatnozic::hobgoblin::rn::RN_Node& node, \
                             std::initializer_list<::jbatnozic::hobgoblin::PZInteger> receivers UHOBGOBLIN_RN_NORMALIZE_ARGS(const, __VA_ARGS__)) { \
        static ::jbatnozic::hobgoblin::rn::detail::RN_HandlerNameToIdCacher hntic{#_name_};  \
        ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ComposeImpl(node, receivers, \
                                                              hntic.getHandlerId()  UHOBGOBLIN_RN_PASS_COMPOSE_ARGS(__VA_ARGS__)); \
    } \
    template <class R> \
    void RN_Compose_##_name_(::jbatnozic::hobgoblin::rn::RN_Node& node, R&& receivers UHOBGOBLIN_RN_NORMALIZE_ARGS(const, __VA_ARGS__)) { \
        static ::jbatnozic::hobgoblin::rn::detail::RN_HandlerNameToIdCacher hntic{#_name_};  \
        ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ComposeImpl(node, std::forward<R>(receivers), \
                                                              hntic.getHandlerId()  UHOBGOBLIN_RN_PASS_COMPOSE_ARGS(__VA_ARGS__)); \
    }

///////////////////////////////////////

#define RN_DEFINE_HANDLER(_name_, _args_) \
    UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, _args_); \
    UHOBGOBLIN_RN_GENERATE_HANDLER_PROXY(_name_, _args_) \
    UHOBGOBLIN_RN_INSTALL_HANDLER_PROXY(_name_); \
    UHOBGOBLIN_RN_GENERATE_COMPOSE_FUNCTION(_name_, _args_) \
    UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, _args_)

#define RN_DECLARE_HANDLER(_name_, _args_) \
    UHOBGOBLIN_RN_GENERATE_COMPOSE_FUNCTION(_name_, _args)

#define RN_HANDLER_SIGNATURE(_name_, _args_) \
    UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, _args_)

#define RN_ARGS(...) __VA_ARGS__

#endif // !UHOBGOBLIN_RN_MACRO_HANDLERS_HPP
