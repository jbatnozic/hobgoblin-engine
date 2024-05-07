// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/HGExcept.hpp>

#include <iostream>

namespace {
int Add(int x, int y) {
    return x + y;
}
} // namespace

using hg::TracedException;

int main(int argc, char* argv[]) {
    try {
        HG_THROW_TRACED(TracedException, 0);
    }
    catch (const hg::TracedException& ex) {
        std::cerr << ex.getFullFormattedDescription() << "\n\n";
    }

    try {
        HG_THROW_TRACED(TracedException, 0, "simple message");
    }
    catch (const hg::TracedException& ex) {
        std::cerr << ex.getFullFormattedDescription() << "\n\n";
    }

    try {
        HG_THROW_TRACED(TracedException, 0, "message format with detail: {}", "detail_string");
    }
    catch (const hg::TracedException& ex) {
        std::cerr << ex.getFullFormattedDescription() << "\n\n";
    }

    try {
        HG_THROW_TRACED(TracedException, 0, "message format with detail: {}", Add(2, 4));
    }
    catch (const hg::TracedException& ex) {
        std::cerr << ex.getFullFormattedDescription() << "\n\n";
    }

    try {
        HG_ASSERT(Add(5, 4) == 10);
        HG_HARD_ASSERT(5 == 6);
    }
    catch (const hg::TracedException& ex) {
        std::cerr << ex.getFullFormattedDescription() << "\n\n";
    }

    // Test compilation for up to 16 args:
    try {
        HG_THROW_TRACED(TracedException, 0, "{}", 0);
        HG_THROW_TRACED(TracedException, 0, "{} {}", 0, 1);
        HG_THROW_TRACED(TracedException, 0, "{} {} {}", 0, 1, 2);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {}", 0, 1, 2, 3);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {}", 0, 1, 2, 3, 4);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {} {}", 0, 1, 2, 3, 4, 5);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {} {} {}", 0, 1, 2, 3, 4, 5, 6);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {} {} {} {}", 0, 1, 2, 3, 4, 5, 6, 7);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {} {} {} {} {}", 0, 1, 2, 3, 4, 5, 6, 7, 8);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {} {} {} {} {} {}", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {} {} {} {} {} {} {}", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {} {} {} {} {} {} {} {}", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {} {} {} {} {} {} {} {} {}", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {} {} {} {} {} {} {} {} {} {}", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {} {} {} {} {} {} {} {} {} {} {}", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
        HG_THROW_TRACED(TracedException, 0, "{} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    }
    catch (const hg::TracedException& ex) {
        // Do nothing
    }

    return 0;
}

// clang-format on
