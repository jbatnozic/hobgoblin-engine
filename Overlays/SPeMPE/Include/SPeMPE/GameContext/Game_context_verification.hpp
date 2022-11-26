#ifndef SPEMPE_GAME_CONTEXT_GAME_CONTEXT_VERIFICATION_HPP
#define SPEMPE_GAME_CONTEXT_GAME_CONTEXT_VERIFICATION_HPP

#include <SPeMPE/GameContext/Game_context.hpp>

#include <Hobgoblin/Common.hpp>

namespace jbatnozic {
namespace spempe {

//! Exception raised when GameContext flag consistency check
//! (via SPEMPE_VERIFY_GAME_CONTEXT_FLAGS, see below) fails.
class InconsistentGameContextError : hobgoblin::TracedLogicError {
public:
    using TracedLogicError::TracedLogicError;
};

//! Use this macro to verify that the flags (privileged/headless/networking) of a spempe::GameContext
//! object match with the expectations of your other objects. If the flags don't match, an instance
//! of InconsistentGameContextError will be thrown.
//!
//! Example of usage:
//!     SPEMPE_VERIFY_GAME_CONTEXT_FLAGS(ctx, privileged=true, headless=true, networking=true);    
//!     SPEMPE_VERIFY_GAME_CONTEXT_FLAGS(ctx, headless=false, privileged=false);
//!     SPEMPE_VERIFY_GAME_CONTEXT_FLAGS(ctx, headless==true, privileged==true);
//!     // Notice: flags you don't care about can be left out and you can use either = or == to
//!     //         specify expectations.
#define SPEMPE_VERIFY_GAME_CONTEXT_FLAGS(_ctx_, ...) \
    do { \
        ::jbatnozic::spempe::detail::Tribool USPEMPE_privileged; \
        ::jbatnozic::spempe::detail::Tribool USPEMPE_headless;   \
        ::jbatnozic::spempe::detail::Tribool USPEMPE_networking; \
        USPEMPE_MACRO_EXPAND_VA(USPEMPE_VOID_EXPRESSIONS_5(__VA_ARGS__, 0, 0, 0, 0, 0)); \
        \
        bool USPEMPE_privileged_matches = true, USPEMPE_headless_matches = true, USPEMPE_networking_matches = true; \
        const ::jbatnozic::spempe::GameContext& USPEMPE_ctx = (_ctx_);\
        if (USPEMPE_privileged.z != -1 && USPEMPE_privileged.z != static_cast<int>(USPEMPE_ctx.isPrivileged())) { \
            USPEMPE_privileged_matches = false; \
        } \
        if (USPEMPE_headless.z != -1 && USPEMPE_headless.z != static_cast<int>(USPEMPE_ctx.isHeadless())) { \
            USPEMPE_headless_matches = false; \
        } \
        if (USPEMPE_networking.z != -1 && USPEMPE_networking.z != static_cast<int>(USPEMPE_ctx.hasNetworking())) { \
            USPEMPE_networking_matches = false; \
        } \
        if (!USPEMPE_privileged_matches || \
            !USPEMPE_headless_matches || \
            !USPEMPE_networking_matches) { \
            ::jbatnozic::spempe::detail::RaiseInconsistentGameContextError( \
                USPEMPE_ctx, \
                USPEMPE_privileged_matches, \
                USPEMPE_headless_matches, \
                USPEMPE_networking_matches \
            ); \
        } \
    }  while (false)

///////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION BELOW                                                  //
///////////////////////////////////////////////////////////////////////////

#ifndef USPEMPE_MACRO_EXPAND_VA
#define USPEMPE_MACRO_EXPAND_VA(...) __VA_ARGS__
#endif // !USPEMPE_MACRO_EXPAND_VA

#define USPEMPE_0 0

#define USPEMPE_VOID_EXPRESSIONS_5(_e1_, _e2_, _e3_, _e4_, _e5_, ...) \
    static_cast<void>(USPEMPE_##_e1_); \
    static_cast<void>(USPEMPE_##_e2_); \
    static_cast<void>(USPEMPE_##_e3_); \
    static_cast<void>(USPEMPE_##_e4_); \
    static_cast<void>(USPEMPE_##_e5_)

namespace detail {
struct Tribool {
    int z = -1;
    Tribool& operator=(const bool aRhs) { z = static_cast<int>(aRhs); return *this; }
    // Yes, I know this is weird!
    bool operator==(const bool aRhs) { z = static_cast<int>(aRhs); return false; }
};

void RaiseInconsistentGameContextError(
    const GameContext& aCtx,
    bool aPrivilegedMatches,
    bool aHeadlessMatches,
    bool aNetworkingMatches
);
} // namespace detail

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_CONTEXT_GAME_CONTEXT_VERIFICATION_HPP
