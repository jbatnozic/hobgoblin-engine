// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_HPP
#define UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_HPP

#include "Flow_field_spooler_impl.hpp"

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Math/Angle.hpp>
#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Utility/Flow_field_calculator.hpp>

#include <cstdint>
#include <memory>
#include <optional>
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

template <class taWorldCostProvider>
class FlowFieldSpooler {
public:
    FlowFieldSpooler(const taWorldCostProvider& aWorldCostProvider);

    void tick();

    void pause();

    void unpause();

    using RequestId = std::uint64_t;

    RequestId addRequest(math::Vector2pz aFieldTopLeft,
                         math::Vector2pz aFieldDimensions,
                         math::Vector2pz aTarget,
                         PZInteger aMaxIterations);

    std::optional<OffsetFlowField> collectResult(std::uint64_t aRequestId);

private:
    const taWorldCostProvider& _costProvider;

    std::unique_ptr<detail::FlowFieldSpoolerImplInterface> _impl;

    static std::uint8_t _worldCostFunction(math::Vector2pz aWorldPosition, void* aData) {
        // Assume not null
        const auto& self = *static_cast<FlowFieldSpooler*>(aData);
        return self._costProvider.getCostAt(aWorldPosition);
    }
};

///////////////////////////////////////////////////////////////////////////
// IMPLEMENTATIONS                                                       //
///////////////////////////////////////////////////////////////////////////

template <class taWorldCostProvider>
FlowFieldSpooler<taWorldCostProvider>::FlowFieldSpooler(const taWorldCostProvider& aWorldCostProvider)
    : _costProvider{aWorldCostProvider}
    , _impl{detail::CreateDefaultFlowFieldSpoolerImpl(8, &_worldCostFunction, this)}
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
    PZInteger aMaxIterations) {
    return _impl->addRequest(aFieldTopLeft, aFieldDimensions, aTarget, aMaxIterations);
}

template <class taWorldCostProvider>
std::optional<OffsetFlowField> FlowFieldSpooler<taWorldCostProvider>::collectResult(
    std::uint64_t aRequestId) {
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
