#pragma once

#include <Hobgoblin/HGExcept.hpp>

#include <ZTCpp.hpp>

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>

class SimpleZeroTierError : public jbatnozic::hobgoblin::TracedRuntimeError {
public:
    using TracedRuntimeError::TracedRuntimeError;
};

std::optional<jbatnozic::ztcpp::IpAddress> SimpleZeroTierInit(
    const std::string&        aNodeIdentityPath,
    std::uint16_t             aServicePort,
    std::uint64_t             aNetworkId,
    std::chrono::milliseconds aMaxTimeToWait
);

void SimpleZeroTierStop();
