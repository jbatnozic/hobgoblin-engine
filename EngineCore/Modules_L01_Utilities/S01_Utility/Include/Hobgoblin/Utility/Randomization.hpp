// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_UTIL_RANDOMIZATION_HPP
#define UHOBGOBLIN_UTIL_RANDOMIZATION_HPP

#include <cstdint>
#include <mutex>
#include <random>
#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

namespace detail {
void Get32bitRNG(std::mt19937** aEngine, std::mutex** aMutex);
void Get64bitRNG(std::mt19937_64** aEngine, std::mutex** aMutex);
} // namespace detail

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
//!
//! NOTE: To seed the engine you can do:
//! DoWith32bitRNG([](std::mt19937& aRNG) { aRNG.seed(Generate32bitSeed()); });
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
//!
//! NOTE: To seed the engine you can do:
//! DoWith64bitRNG([](std::mt19937_64& aRNG) { aRNG.seed(Generate64bitSeed()); });
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

//! Returns a pseudorandom integral or floating-point number of type taNumber
//! in a thread-safe manner using DoWith32bitRNG or DoWith64bitRNG.
template <class taNumber>
taNumber GetRandomNumber(taNumber aMin, taNumber aMax) {
    taNumber result;
    if constexpr (sizeof(taNumber) <= sizeof(std::int32_t)) {
        DoWith32bitRNG(
            [aMin, aMax, &result](std::mt19937& aRNG) {
                if constexpr (std::is_integral<taNumber>::value) {
                    std::uniform_int_distribution dist{aMin, aMax};
                    result = dist(aRNG);
                } else {
                    std::uniform_real_distribution dist{aMin, aMax};
                    result = dist(aRNG);
                }
            }
        );
    } else {
        DoWith64bitRNG(
            [aMin, aMax, &result](std::mt19937_64& aRNG) {
                if constexpr (std::is_integral<taNumber>::value) {
                    std::uniform_int_distribution dist{aMin, aMax};
                    result = dist(aRNG);
                } else {
                    std::uniform_real_distribution dist{aMin, aMax};
                    result = dist(aRNG);
                }
            }
        );
    }
    return result;
}

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_RANDOMIZATION_HPP

// clang-format on
