// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef SPEMPE_UTILITY_TIMING_HPP
#define SPEMPE_UTILITY_TIMING_HPP

#include <Hobgoblin/Common.hpp>

#include <chrono>

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

//! Represents a number of seconds (integral or fractional, using
//! a floating-point representation) in a type-safe manner.
using FloatSeconds = std::chrono::duration<double>;

//! Represents the desired number of ticks per second for your game.
//! 'Ticks' are logical steps in the game, where game world simulation happens, separated
//! from the number of literal frames the game produces per second.
//! One tick corresponds to one QAO Update step, usually (but not necessarily) followed by
//! a QAO Draw step.
class TickRate {
public:
    explicit TickRate(hg::PZInteger aTicksPerSecond = 60)
        : _value{aTicksPerSecond} {}

    //! Returns the number of ticks per second.
    hg::PZInteger getValue() const {
        return _value;
    }

    //! Returns the duration of one tick in seconds.
    FloatSeconds getDeltaTime() const {
        return FloatSeconds{1.0 / static_cast<double>(_value)};
    }

private:
    hg::PZInteger _value;
};

//! Represents a number of frames per second that a game can produce.
//! One frame corresponds to display() call to a window - which sends
//! a rendered frame buffer to your GPU/Monitor.
//! This is different from TickRate and does not affect the simulation
//! speed of the game.
class FrameRate {
public:
    explicit FrameRate(hg::PZInteger aFramesPerSecond = 120)
        : _value{aFramesPerSecond} {}

    //! Returns the number of display intervals per second.
    hg::PZInteger getValue() const {
        return _value;
    }

    //! Returns the duration of one display interval in seconds.
    FloatSeconds getDeltaTime() const {
        return FloatSeconds{1.0 / static_cast<double>(_value)};
    }

private:
    hg::PZInteger _value;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_UTILITY_TIMING_HPP
