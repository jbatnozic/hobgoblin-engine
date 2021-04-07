
#include "RandomGen.hpp"

#include <random>
#include <stdexcept>
#include <Windows.h>

namespace {

bool WinApiAcquireCryptContext(HCRYPTPROV *ctx) {
    if (!CryptAcquireContext(ctx, nullptr, nullptr, PROV_RSA_FULL, 0)) {
        return CryptAcquireContext(ctx, nullptr, nullptr, PROV_RSA_FULL, CRYPT_NEWKEYSET);
    }
    return true;
}

std::mt19937 general_purpose_random_engine;
std::minstd_rand fast_random_engine;

} // anonymous namespace

void WinApiSystemRandomSeed(void* dst, std::size_t dstlen) {
    HCRYPTPROV ctx;
    if (!WinApiAcquireCryptContext(&ctx)) {
        throw std::runtime_error("Unable to initialize Win32 crypt library.");
    }

    BYTE* buffer = reinterpret_cast<BYTE*>(dst);
    if (!CryptGenRandom(ctx, dstlen, buffer)) {
        throw std::runtime_error("Unable to generate random bytes.");
    }

    if (!CryptReleaseContext(ctx, 0)) {
        throw std::runtime_error("Unable to release Win32 crypt library.");
    }
}

std::mt19937& GeneralPurposeRandomEngine() {
    return general_purpose_random_engine;
}

std::minstd_rand& FastRandomEngine() {
    return fast_random_engine;
}

