// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_HPP
#define UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_HPP

#include "Flow_field_spooler_impl.hpp"

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Utility/Flow_field_calculator.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

/*
class WorldCostProvider {
public:
    std::uint8_t getCostAt(math::Vector2pz aPosition) const; // pos.x, pos.y could be very large

private:
    // ...
};
*/

struct OffsetFlowField {
    FlowField flowField;
    math::Vector2pz offset;
};

//! This is a utility class and a wrapper over `FlowFieldCalculator` that facilitates
//! calculating flow fields in limited parts of a (potentially) very large grid-based
//! game world.
//! The flow fields are calculated asynchronously by a pool of threads that can each
//! work on their own flow field in parallel, or they can collaborate on a single 
//! flow field to finalize it more quickly.
//!
//! TODO: write what condition taWorldCostProvider must fulfill
template <class taWorldCostProvider>
class FlowFieldSpooler {
public:
    //! \brief TODO
    using CostProviderMap = std::unordered_map<std::int32_t, NeverNull<const taWorldCostProvider*>>;

    //! \brief TODO
    FlowFieldSpooler(CostProviderMap aCostProviderMap,
                     PZInteger aConcurrencyLimit = 8);

    //! \brief Pauses all of the spooler's operations.
    //!
    //! Calling this method will suspend all of the spooler's worker threads until
    //! `unpause()` is called. It is necessary to do this if you want to change any
    //! of the world data that any of the cost providers (passed to the constructor)
    //! will or could read.
    //!
    //! \warning if the spooler is paused while a flow field is being calculated,
    //!          and a part of the world that intersects with the flow field is changed
    //!          before the spooler is unpaused, then the resulting flow field could
    //!          end up being suboptimal or even invalid. However, the spooler still
    //!          guarantees not to crash or produce any breaking errors in this situation.
    //!
    //! \see unpause
    void pause();

    //! \brief Resumes all of the spooler's operations.
    //!
    //! Calling this method basically undoes the effects of `pause()`. Call it once
    //! you're done changing the world data.
    //!
    //! \see pause
    void unpause();

    //! \brief TODO
    void tick();

    //! Identifies a flow field request.
    //! \see addRequest
    using RequestId = std::uint64_t;

    //! \brief Adds to the spooler a new request to calculate a flow field with the
    //!        given parameters.
    //!
    //! \param aFieldTopLeft coordinates of the top-left corner of the flow field in
    //!                      the game world. Note: the spooler will NOT check if this
    //!                      is inside of the bounds of the game world.
    //! \param aFieldDimensions width and height of the flow field. Note: the spooler
    //!                         will NOT check if this is inside the bounds of the
    //!                         game world when added to `aFieldTopLeft`.
    //! \param aTarget target coordinates that the flow field will try to reach. This is
    //!                given in absolute world coordinates and NOT relative to `aFieldTopLeft`.
    //!                This value must be within `aFieldDimensions`.
    //! \param aCostProviderId TODO
    //! \param aMaxIterations maximum number of iterations given to calculate the flow field.
    //!                       Must be > 0. Note: setting too low a value can make subsequent
    //!                       calls to `tick()` block:
    //!                       - If `aMaxIterations` is 1, the first `tick()` call after it
    //!                         will block until the requested flow field is calculated;
    //!                       - If `aMaxIterations` is 2, the second `tick()` call after it
    //!                         will block until the requested flow field is calculated;
    //!                       - And so on...
    //!                       (Of course the time it takes to calculate a flow field depends
    //!                        mostly on its dimensions.)
    //!
    //! \returns ID of the request that can be used to refer to it later (to cancel it or
    //!          poll for the result - see `cancelRequest` & `collectResult`).
    //!
    //! \see tick, cancelRequest, collectResult
    RequestId addRequest(math::Vector2pz aFieldTopLeft,
                         math::Vector2pz aFieldDimensions,
                         math::Vector2pz aTarget,
                         std::int32_t aCostProviderId,
                         PZInteger aMaxIterations);

    //! \brief Cancels an ongoing flow field request.
    //!
    //! \param aRequestId ID of the request to cancel. If there is no request by this ID
    //!                   or if it has already been cancelled, nothing happens.
    //!
    //! \see addRequest
    void cancelRequest(RequestId aRequestId);

    //! \brief TODO
    //! hint: returns an emoty optional (std::nullopt) if result isn't ready yet
    std::optional<OffsetFlowField> collectResult(RequestId aRequestId);

private:
    std::unique_ptr<detail::FlowFieldSpoolerImplInterface> _impl;

    static std::uint8_t _worldCostFunction(math::Vector2pz aWorldPosition, const void* aData) {
        // Assume not null for performance reasons
        // "If he dies, he dies"
        const auto* provider = static_cast<const taWorldCostProvider*>(aData);
        return provider->getCostAt(aWorldPosition);
    }

    static detail::WCFMap _convertCostMap(const CostProviderMap& aCostProviderMap) {
        detail::WCFMap result;

        for (const auto& pair : aCostProviderMap) {
            const std::int32_t id = pair.first;
            const taWorldCostProvider* provider = pair.second;

            result[id] = {&_worldCostFunction, provider};
        }

        return result;
    }
};

///////////////////////////////////////////////////////////////////////////
// IMPLEMENTATIONS                                                       //
///////////////////////////////////////////////////////////////////////////

template <class taWorldCostProvider>
FlowFieldSpooler<taWorldCostProvider>::FlowFieldSpooler(CostProviderMap aCostProviderMap,
                                                        PZInteger aConcurrencyLimit)
    : _impl{detail::CreateDefaultFlowFieldSpoolerImpl(_convertCostMap(aCostProviderMap), aConcurrencyLimit)}
{
}

template <class taWorldCostProvider>
void FlowFieldSpooler<taWorldCostProvider>::tick() {
    _impl->tick();
}

template <class taWorldCostProvider>
void FlowFieldSpooler<taWorldCostProvider>::pause() {
    _impl->pause();
}

template <class taWorldCostProvider>
void FlowFieldSpooler<taWorldCostProvider>::unpause() {
    _impl->unpause();
}

template <class taWorldCostProvider>
FlowFieldSpooler<taWorldCostProvider>::RequestId FlowFieldSpooler<taWorldCostProvider>::addRequest(
    math::Vector2pz aFieldTopLeft,
    math::Vector2pz aFieldDimensions,
    math::Vector2pz aTarget,
    std::int32_t aCostProviderId,
    PZInteger aMaxIterations) {
    return _impl->addRequest(aFieldTopLeft, aFieldDimensions, aTarget, aCostProviderId, aMaxIterations);
}

template <class taWorldCostProvider>
void FlowFieldSpooler<taWorldCostProvider>::cancelRequest(RequestId aRequestId) {
    _impl->cancelRequest(aRequestId);
}

template <class taWorldCostProvider>
std::optional<OffsetFlowField> FlowFieldSpooler<taWorldCostProvider>::collectResult(
    RequestId aRequestId) {
    auto implResult = _impl->collectResult(aRequestId);
    if (!implResult.has_value()) {
        return {};
    }

    OffsetFlowField result;
    result.flowField = std::move(implResult->flowField);
    result.offset = implResult->offset;
    return {std::move(result)};
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_define.hpp>

#endif // !UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_HPP
