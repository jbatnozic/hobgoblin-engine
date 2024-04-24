// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_IMPL_HPP
#define UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_IMPL_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Math/Angle.hpp>
#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Utility/Flow_field_calculator.hpp>

#include <cstdint>
#include <memory>
#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {
namespace detail {

using WorldCostFunction = std::uint8_t (*)(math::Vector2pz aWorldPosition, void* aData);

struct OffsetFlowField {
    FlowField flowField;
    math::Vector2pz offset;
};

class FlowFieldSpoolerImplInterface {
public:
    virtual ~FlowFieldSpoolerImplInterface() = default;

    virtual void tick() = 0;

    virtual void pause() = 0;

    virtual void unpause() = 0;

    virtual std::uint64_t addRequest(math::Vector2pz aFieldTopLeft,
                             math::Vector2pz aFieldDimensions,
                             math::Vector2pz aTarget,
                             PZInteger aMaxIterations) = 0;

    virtual std::optional<OffsetFlowField> collectResult(std::uint64_t aRequestId) = 0;
};

extern std::unique_ptr<FlowFieldSpoolerImplInterface> CreateDefaultFlowFieldSpoolerImpl(
    PZInteger aConcurrencyLimit,
    WorldCostFunction aWorldCostFunc,
    void* aWcfData);

} // namespace detail
} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_define.hpp>

#endif // !UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_IMPL_HPP
