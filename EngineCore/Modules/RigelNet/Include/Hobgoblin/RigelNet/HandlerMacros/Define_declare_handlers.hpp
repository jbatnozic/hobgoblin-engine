#ifndef UHOBGOBLIN_RN_HANDLER_MACROS_DEFINE_DECLARE_HANDLERS_HPP
#define UHOBGOBLIN_RN_HANDLER_MACROS_DEFINE_DECLARE_HANDLERS_HPP

#include <Hobgoblin/RigelNet/HandlerMacros/Extract_args.hpp>
#include <Hobgoblin/RigelNet/HandlerMacros/Normalize_args.hpp>
#include <Hobgoblin/RigelNet/HandlerMacros/Pass_args.hpp>
#include <Hobgoblin/RigelNet/HandlerMacros/Pass_compose_args.hpp>

#define RN_NODE_IN_HANDLER() UHOBGOBLIN_RN_Reference_to_node

#define UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, ...) \
    void UHOBGOBLIN_RN_Handler_##_name_( \
        ::jbatnozic::hobgoblin::rn::RN_NodeInterface& RN_NODE_IN_HANDLER() /* , */ \
         UHOBGOBLIN_RN_NORMALIZE_ARGS(/*nonconst*/, __VA_ARGS__) \
    )

#define UHOBGOBLIN_RN_GENERATE_HANDLER_PROXY(_name_, ...) \
    void UHOBGOBLIN_RN_HandlerProxy_##_name_(::jbatnozic::hobgoblin::rn::RN_NodeInterface& node) { \
        UHOBGOBLIN_RN_EXTRACT_ARGS(__VA_ARGS__) \
        UHOBGOBLIN_RN_Handler_##_name_(node  UHOBGOBLIN_RN_PASS_ARGS(__VA_ARGS__)); \
    }

#define UHOBGOBLIN_RN_INSTALL_HANDLER_PROXY(_name_) \
    ::jbatnozic::hobgoblin::rn::detail::RN_StaticHandlerInitializer \
    UHOBGOBLIN_RN_HandlerInit_##_name_{#_name_, UHOBGOBLIN_RN_HandlerProxy_##_name_}

// Some preprocessors will paste Compose_* with the return type (void - see below) if _prefix_ is empty
template <class taType>
using UHOBGOBLIN_TypeIdentity = taType;

#define UHOBGOBLIN_RN_GENERATE_COMPOSE_FUNCTION(_name_, _prefix_, ...) \
    ::UHOBGOBLIN_TypeIdentity<void> \
    _prefix_##Compose_##_name_(::jbatnozic::hobgoblin::rn::RN_NodeInterface& node, \
                               std::initializer_list<::jbatnozic::hobgoblin::PZInteger> recepients /* , */ \
                               UHOBGOBLIN_RN_NORMALIZE_ARGS(const, __VA_ARGS__)) { \
        static ::jbatnozic::hobgoblin::rn::detail::RN_HandlerNameToIdCacher hntic{#_name_}; \
        ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ComposeImpl(node, recepients, \
                                                              hntic.getHandlerId() /* , */ \
                                                              UHOBGOBLIN_RN_PASS_COMPOSE_ARGS(__VA_ARGS__)); \
    } \
    template <class taRec> \
    UHOBGOBLIN_TypeIdentity<void> \
    _prefix_##Compose_##_name_(::jbatnozic::hobgoblin::rn::RN_NodeInterface& node, \
                                taRec&& recepients /* , */ \
                                UHOBGOBLIN_RN_NORMALIZE_ARGS(const, __VA_ARGS__)) { \
        static ::jbatnozic::hobgoblin::rn::detail::RN_HandlerNameToIdCacher hntic{#_name_}; \
        ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ComposeImpl(node, std::forward<taRec>(recepients), \
                                                              hntic.getHandlerId() /* , */ \
                                                              UHOBGOBLIN_RN_PASS_COMPOSE_ARGS(__VA_ARGS__)); \
    }

///////////////////////////////////////



#endif // !UHOBGOBLIN_RN_HANDLER_MACROS_DEFINE_DECLARE_HANDLERS_HPP
