// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_COMPACT_ANGLE_HPP
#define UHOBGOBLIN_UTIL_COMPACT_ANGLE_HPP

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Math/Angle.hpp>

#include <cmath>
#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! Stores an angle between 0 and 360 degrees in a single byte
//! using 2.5 degree increments.
class CompactAngle {
public:
    CompactAngle() = default;

    explicit CompactAngle(math::AngleF aAngle)
        : _value{static_cast<std::uint8_t>(std::round(aAngle.normalize().asDeg() / INCREMENT))} {}

    bool hasValue() const {
        return (_value != VALUE_NONE);
    }

    math::AngleF getValue() const {
        HG_VALIDATE_PRECONDITION(hasValue());
        return math::AngleF::fromDeg(_value * INCREMENT);
    }

    math::AngleF operator*() const {
        return getValue();
    }

private:
    static constexpr float        INCREMENT  = 2.5f;
    static constexpr std::uint8_t VALUE_NONE = 255;

    std::uint8_t _value = VALUE_NONE;
};

static_assert(sizeof(CompactAngle) == 1, "An instance of CompactAngle must fit in one byte!");

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_define.hpp>

#endif // !UHOBGOBLIN_UTIL_COMPACT_ANGLE_HPP
