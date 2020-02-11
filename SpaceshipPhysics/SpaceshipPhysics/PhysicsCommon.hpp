#ifndef PHYSICS_COMMON_HPP
#define PHYSICS_COMMON_HPP

using Real = double; // DOUBLE seems to be faster than FLOAT

constexpr double PHYS_PI = Real{3.14159265358979323846};

template <typename T>
T Sqr(const T arg) {
    return arg * arg;
}

struct Radians {
    Real rad;
    Radians() = default;
    constexpr explicit Radians(Real rad) : rad(rad) {}
};

struct Degrees {
    Real deg;
    Degrees() = default;
    constexpr explicit Degrees(Real deg) : deg(deg) {}
};

inline
Radians DegToRad(Degrees deg) {
    return Radians{deg.deg * PHYS_PI / 180.0};
}

inline
Degrees RadToDeg(Radians rad) {
    return Degrees{rad.rad * 180 / PHYS_PI};
}

#endif // !PHYSICS_COMMON_HPP