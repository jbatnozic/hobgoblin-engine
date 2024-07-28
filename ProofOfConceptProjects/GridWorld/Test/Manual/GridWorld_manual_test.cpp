#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>

#include <iostream>
#include <stdexcept>

#include "Lighting_test.hpp"
#include "Spooling_test.hpp"

namespace hg = jbatnozic::hobgoblin;

int main() try {
    hg::log::SetMinimalLogSeverity(hg::log::Severity::Debug);

    // RunLightingTest();
    RunSpoolingTest();

} catch (const hg::TracedException& ex) {
    std::cout << "Traced exception caught: " << ex.getFullFormattedDescription() << '\n';
    return 1;
} catch (const std::exception& ex) {
    std::cout << "Exception caught: " << ex.what() << '\n';
    return 1;
}
