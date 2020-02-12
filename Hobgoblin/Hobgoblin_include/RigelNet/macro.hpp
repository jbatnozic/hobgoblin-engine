#pragma once

#include "Rigel-config.hpp"

// Type for when you want to send only strings
#define RN_ANY RIGELNETW_CONFIG_NS_NAME::Int8

// "Immediate" token concatenation (Helper macro):
#define RIGELNETW_TOKEN_CONCAT_IM(x, y) x ## y

// "Indirection layer" for token concatenation (Helper macro):
#define RIGELNETW_TOKEN_CONCAT_IL(x, y) RIGELNETW_TOKEN_CONCAT_IM(x, y)

// Static initialization of handler function:
//
// Usage: RN_STATIC_HANDLER_INIT{index, handler};
//
// index:   unsigned integral constant (handler ID)
// handler: function pointer (function name) of the handler
// 
// Desc: Uses the global-object-constructor-trick to
// initialize a handler before entering main().
//
// Note: Must not be on the same line as another invocation
// of the same macro. Must be placed in the global scope.
//
#define RN_STATIC_HANDLER_INIT static RIGELNETW_CONFIG_NS_NAME::GlobalHandlerInitializer \
                               RIGELNETW_TOKEN_CONCAT_IL(Rigel_GlobHndInit_, __LINE__)

namespace RIGELNETW_CONFIG_NS_NAME {

    struct GlobalHandlerInitializer {

        GlobalHandlerInitializer() = delete;

        GlobalHandlerInitializer(size_t index, Handler f) {

            HandlerMgr::handler_set(index, f);

            }

        };

    }
