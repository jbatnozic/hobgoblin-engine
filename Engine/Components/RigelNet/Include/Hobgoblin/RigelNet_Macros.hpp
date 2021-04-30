#ifndef UHOBGOBLIN_RIGELNET_MACROS_HPP
#define UHOBGOBLIN_RIGELNET_MACROS_HPP

#include <Hobgoblin/RigelNet/HandlerMacros/Define_declare_handlers.hpp>

/* What these macros call a "handler proxy" is what's just a handler as seen ny the
 * rest of RN. It is a function that's registered in the global mapper and called
 * when a DATA-type message is received. It then extracts the relevant arguments from
 * the current packet and calls the function defined by the user (what user percieves
 * as the handler).
 */

//! Argument list in the format: type0, name0, type1, name1, ...
//! Up to 10 arguments supported. Can be empty.
#define RN_ARGS(...) __VA_ARGS__

//! Used to define a handler function and its proxy.
#define RN_DEFINE_HANDLER(_name_, _args_) \
    UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, _args_); \
    UHOBGOBLIN_RN_GENERATE_HANDLER_PROXY(_name_, _args_) \
    UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, _args_)

//! Define a Compose_* function for a handler, allowing to prepare the
//! RPC in a type-safe manner.
#define RN_DEFINE_COMPOSEFUNC(_name_, _args_) \
    UHOBGOBLIN_RN_GENERATE_COMPOSE_FUNCTION(_name_, /* No Compose prefix */, _args_) 

//! Same as RN_DEFINE_COMPOSEFUNC(), except that the name of the generated 
//! Compose function will be prefixed with _compose_prefix_.
#define RN_DEFINE_COMPOSEFUNC_P(_name_, _compose_prefix_, _args_) \
    UHOBGOBLIN_RN_GENERATE_COMPOSE_FUNCTION(_name_, _compose_prefix_, _args_)

//! Registers the handler with the global mapper before entering main()
//! using the static object constructor trick.
#define RN_REGISTER_HANDLER_BEFORE_MAIN(_name_) \
    UHOBGOBLIN_RN_INSTALL_HANDLER_PROXY(_name_)

//! Pastes the signature of a handler in case you need to befriend it to a class.
#define RN_HANDLER_SIGNATURE(_name_, _args_) \
    UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, _args_)

//! Declare and define a handler function and its proxy function, and register them
//! with the global mapper. Also generates the appropriate Compose_* function.
//! Equivalent to:
//!     RN_DEFINE_HANDLER(_name_, _args_);
//!     RN_DEFINE_COMPOSEFUNC(_name_, _args_);
//!     RN_REGISTER_HANDLER_BEFORE_MAIN(_name_);
#define RN_DEFINE_RPC(_name_, _args_) \
    UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, _args_); \
    UHOBGOBLIN_RN_GENERATE_HANDLER_PROXY(_name_, _args_) \
    UHOBGOBLIN_RN_INSTALL_HANDLER_PROXY(_name_); \
    UHOBGOBLIN_RN_GENERATE_COMPOSE_FUNCTION(_name_, /* No Compose prefix */, _args_) \
    UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, _args_)

//! Same as RN_DEFINE_RPC(), except that the name of the Compose function
//! will be prefixed with _compose_prefix_.
#define RN_DEFINE_RPC_P(_name_, _compose_prefix_, _args_) \
    UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, _args_); \
    UHOBGOBLIN_RN_GENERATE_HANDLER_PROXY(_name_, _args_) \
    UHOBGOBLIN_RN_INSTALL_HANDLER_PROXY(_name_); \
    UHOBGOBLIN_RN_GENERATE_COMPOSE_FUNCTION(_name_, _compose_prefix_, _args_) \
    UHOBGOBLIN_RN_DECLARE_HANDLER(_name_, _args_)

#endif // !UHOBGOBLIN_RIGELNET_MACROS_HPP