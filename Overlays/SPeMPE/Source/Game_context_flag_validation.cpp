
#include <SPeMPE/GameContext/Game_context_flag_validation.hpp>

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
    std::ostringstream oss;
    oss << "The flags of spempe::GameContext at address " << std::hex << reinterpret_cast<std::uintptr_t>(&aCtx)
        << " are different than expected - ";

    if (!aPrivilegedMatches) {
        oss << fmt::format("privileged: {} (actual) != {} (expected); ", 
                           aCtx.isPrivileged() ? "YES" : "NO",
                           aCtx.isPrivileged() ? "NO" : "YES");
    }
    if (!aHeadlessMatches) {
        oss << fmt::format("headless: {} (actual) != {} (expected); ", 
                           aCtx.isHeadless() ? "YES" : "NO",
                           aCtx.isHeadless() ? "NO" : "YES");
    }

    if (!aNetworkingMatches) {
        oss << fmt::format("networking: {} (actual) != {} (expected); ", 
                           aCtx.hasNetworking() ? "YES" : "NO",
                           aCtx.hasNetworking() ? "NO" : "YES");
    }

    HG_THROW_TRACED(InconsistentGameContextError, 0, oss.str());
}

} // namespace detail
} // namespace spempe
} // namespace jbatnozic
