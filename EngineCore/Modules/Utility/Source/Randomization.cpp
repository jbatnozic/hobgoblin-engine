
#include <Hobgoblin/Utility/Randomization.hpp>

#include <cstdint>
#include <chrono>
#include <limits>
#include <mutex>
#include <random>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

namespace {
std::mt19937 g_32bitMersenneTwisterEngine;
std::mutex   g_mutexFor32bitEngine;

std::mt19937_64 g_64bitMersenneTwisterEngine;
std::mutex      g_mutexFor64bitEngine;
} // namespace

namespace detail {
void Get32bitRNG(std::mt19937** aEngine, std::mutex** aMutex) {
    *aEngine = &g_32bitMersenneTwisterEngine;
    *aMutex  = &g_mutexFor32bitEngine;
}

void Get64bitRNG(std::mt19937_64** aEngine, std::mutex** aMutex) {
    *aEngine = &g_64bitMersenneTwisterEngine;
    *aMutex  = &g_mutexFor64bitEngine;
}
} // namespace detail

// functions - get real random, get simple uniform random (int/float), seedwith, seedautomatically...

std::uint64_t Generate64bitSeed() {
    const auto now = std::chrono::system_clock::now();
    const auto timeValue = static_cast<std::uint64_t>(now.time_since_epoch().count());

    std::random_device rd;
    std::uniform_int_distribution<std::uint64_t> dist{0, std::numeric_limits<std::uint64_t>::max()};

    const auto randomDeviceValue = dist(rd);

    return (timeValue ^ randomDeviceValue ^ 0x94642A771ED72DEF);
}

std::uint32_t Generate32bitSeed() {
    return static_cast<std::uint32_t>(Generate64bitSeed() & 0xFFFFFFFF);
}



}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>
