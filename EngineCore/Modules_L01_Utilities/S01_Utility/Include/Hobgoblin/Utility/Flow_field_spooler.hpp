// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_HPP
#define UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Utility/Private/Flow_field_spooler_impl.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

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
//! \tparam taCostProvider object which will be used to get the costs of traversing each cell in
//!                        the game world (the higher the cost of the cell, the more likely that
//!                        the cell will be avoided if possible. Return COST_IMPASSABLE (255) to
//!                        mark impassable cells). The only requirement is that it has a public
//!                        method with the following signature:
//!                                `std::uint8_t getCostAt(math::Vector2pz aPosition) const`
//!                        - Note: this method will be on the hot path of the flow field calculation,
//!                                so make sure it doesn't do any unnecessary work.
//!                        - Note: DO NOT, EVER have this method return zero! Only the destination
//!                                point of the flow field gets to have a cost of zero. However, the
//!                                calculator will not check this due to the previous point.
//! 
//! Example cost provider:
//!     class DataProvider {
//!     public:
//!         std::uint8_t getCostAt(math::Vector2pz aPosition) const {
//!             return costs.at(aPosition.y, aPosition.x);
//!         }
//!     
//!         util::RowMajorGrid<std::uint8_t> costs;
//!     };
//!
template <class taWorldCostProvider>
class FlowFieldSpooler {
public:
    //! \brief Identifies a single cost provider.
    using CostProviderId = std::uint32_t;

    //! \brief TODO
    using CostProviderMap = std::unordered_map<CostProviderId, NeverNull<const taWorldCostProvider*>>;

    //! \brief Constructor.
    //! 
    //! \param aCostProviderMap a collection of cost providers mapped to their IDs (the IDs can be
    //!                         chosen arbitrarily). Whenever you later request a new flow field,
    //!                         you will need to provide the ID of a cost provider to use. By this
    //!                         mechanism a single spooler can be used to calculate flow fields in
    //!                         multiple distinct game worlds; or to model the fact that different
    //!                         entities in the game traverse the game world in different ways and
    //!                         thus use different cost models.
    //!                         Note: all providers must be kept alive until the spooler is destroyed.
    //! \param aConcurrencyLimit the number of worker threads that the spooler is allowed to use.
    //!                          Note that at 1, the spooler will perform slower than a bare
    //!                          `FlowFieldCalculator`. At 2, they will be about on-par with each
    //!                          other. Thus at least 3 (and preferrably more) is recommended. But
    //!                          1 is the minimal required value.
    //! 
    //! \throws InvalidArgumentError if `aCostProviderMap` is empty or if `aConcurrencyLimit` is 0.
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

    //! \brief Helps the Spooler keep track of time.
    //! 
    //! The Spooler has no internal time-keeping mechanism, and when making a new flow field request,
    //! the maximum time to complete the request is given in iterations rather than milliseconds.
    //! The idea is that the program will call `tick()` at a steady pace, and the time between two
    //! of such calls is one iteration. The recommended way to order various method calls is as follows:
    //!     <GAME ITERATION BEGINS>
    //!     ...
    //!     spooler.tick();
    //!     <collect finished flow fields>
    //!     spooler.pause();
    //!     <quickly apply changes to the game world if needed>
    //!     spooler.unpause();
    //!     <add new flow field requests if needed>
    //!     ...
    //!     <GAME ITERATION ENDS>
    //! 
    //! \note if there are any requests that are due until the end of the current iteration when
    //!       `tick()` is called, the call will block until they are all finished (see the description
    //!       of `addRequest()` for more on this). This is why it's important to never call `tick()`
    //!       while the spooler is paused - because the requests would never get finished and it
    //!       would block forever.
    //! 
    //! \throws PreconditionNotMetError if the spooler is not unpaused when `tick()` is called.
    //! 
    //! \see addRequest
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
    //! \param aCostProviderId ID of the cost provider to use for the calculation (one of the
    //!                        providers passed to the constructor).
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
                         CostProviderId aCostProviderId,
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
    CostProviderId aCostProviderId,
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
