#ifndef RANDOM_GEN_HPP
#define RANDOM_GEN_HPP

#include <random>

void WinApiSystemRandomSeed(void*, std::size_t);

template <class T>
T SystemRandomSeed() {
    T rv;
    WinApiSystemRandomSeed(&rv, sizeof(rv));
    return rv;
}

// TODO CRTP for General RNG engine interface

std::mt19937& GeneralPurposeRandomEngine();
std::minstd_rand& FastRandomEngine();

#endif // !RANDOM_GEN_HPP
