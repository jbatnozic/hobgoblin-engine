#ifndef SPEMPE_UTILITY_NETWORKING_TELEMETRY_REPORTER_HPP
#define SPEMPE_UTILITY_NETWORKING_TELEMETRY_REPORTER_HPP

#include <Hobgoblin/Common.hpp>

#include <SPeMPE/GameContext/Game_context.hpp>
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>

namespace jbatnozic {
namespace spempe {

//! Periodically logs the bandwidth usage of a NetworkingManager.
class NetworkingTelemetryReporter : public NonstateObject {
public:
    struct Config {
        //! The value has to be greater than 0.
        //! - If the value is 1, a report will be logged every iteration;
        //! - If the value is 2, a report will be logged every other iteration;
        //! And so on...
        hobgoblin::PZInteger cycleLength = 60;
    };

    NetworkingTelemetryReporter(
        hobgoblin::QAO_RuntimeRef aRuntimeRef,
        int aExecutionPriority,
        const Config& aConfig
    );

private:
    hobgoblin::PZInteger _cycleLength;
    hobgoblin::PZInteger _counter;

    void _eventPostUpdate() override;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_UTILITY_NETWORKING_TELEMETRY_REPORTER_HPP
