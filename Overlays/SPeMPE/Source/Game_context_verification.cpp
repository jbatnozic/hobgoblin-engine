
#include <SPeMPE/GameContext/Game_context_verification.hpp>

#include <Hobgoblin/Format.hpp>

#include <sstream>
#include <string>

namespace jbatnozic {
namespace spempe {
namespace detail {

void RaiseInconsistentGameContextError(
    const GameContext& aCtx,
    bool aPrivilegedMatches,
    bool aHeadlessMatches,
    bool aNetworkingMatches
) {
    std::stringstream ss;
    ss << "The flags of spempe::GameContext at address " << std::hex << reinterpret_cast<std::uintptr_t>(&aCtx)
       << " are different than expected - ";

    if (!aPrivilegedMatches) {
        ss << fmt::format("privileged: {} (actual) != {} (expected); ", 
                          aCtx.isPrivileged() ? "YES" : "NO",
                          aCtx.isPrivileged() ? "NO" : "YES");
    }
    if (!aHeadlessMatches) {
        ss << fmt::format("headless: {} (actual) != {} (expected); ", 
                          aCtx.isHeadless() ? "YES" : "NO",
                          aCtx.isHeadless() ? "NO" : "YES");
    }

    if (!aNetworkingMatches) {
        ss << fmt::format("networking: {} (actual) != {} (expected); ", 
                          aCtx.hasNetworking() ? "YES" : "NO",
                          aCtx.hasNetworking() ? "NO" : "YES");
    }

    throw InconsistentGameContextError{ss.str()};
}

} // namespace detail
} // namespace spempe
} // namespace jbatnozic
