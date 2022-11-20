#ifndef UHOBGOBLIN_UTIL_RANDOMIZATION_HPP
#define UHOBGOBLIN_UTIL_RANDOMIZATION_HPP

#include <cstdint>
#include <mutex>
#include <random>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

namespace detail {
void Get32bitRNG(std::mt19937** aEngine, std::mutex** aMutex);
void Get64bitRNG(std::mt19937_64** aEngine, std::mutex** aMutex);
} // namespace detail

// functions - get real random, get simple uniform random (int/float), seedwith, seedautomatically...

//! Generates a truly random 32bit seed using the system's specialized hardware for generating
//! randomness (if available) and the system clock. The returned value should be used only to
//! seed a pseudorandom number generator, as this function can be very slow.
std::uint32_t Generate32bitSeed();

//! Generates a truly random 64bit seed using the system's specialized hardware for generating
//! randomness (if available) and the system clock. The returned value should be used only to
//! seed a pseudorandom number generator, as this function can be very slow.
std::uint64_t Generate64bitSeed();

//! Runs a function, functor or other object that's callable with a single parameter of type
//! std::mt19937&, in a thread-safe manner. The provided reference will be to a statically
//! allocated mersenne-twister engine. Returns the value of the callable (if any).
template <class taCallable>
auto DoWith32bitRNG(const taCallable& aCallable) {
    std::mt19937* engine;
    std::mutex*   mutex;
    detail::Get32bitRNG(&engine, &mutex);
    {
        std::lock_guard lock{*mutex};
        return aCallable(static_cast<std::mt19937&>(*engine));
    }
}

//! Runs a function, functor or other object that's callable with a single parameter of type
//! std::mt19937_64&, in a thread-safe manner. The provided reference will be to a statically
//! allocated mersenne-twister engine. Returns the value of the callable (if any).
template <class taCallable>
auto DoWith64bitRNG(const taCallable& aCallable) {
    std::mt19937_64* engine;
    std::mutex*      mutex;
    detail::Get64bitRNG(&engine, &mutex);
    {
        std::lock_guard lock{*mutex};
        return aCallable(static_cast<std::mt19937_64&>(*engine));
    }
}

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_RANDOMIZATION_HPP
