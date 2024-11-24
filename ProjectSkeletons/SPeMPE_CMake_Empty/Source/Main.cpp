#include <Hobgoblin/Logging.hpp>
#include <SPeMPE/SPeMPE.hpp>

#include <cstdlib>

using namespace jbatnozic;

int main(int argc, char* argv[]) {
    spempe::GameContext ctx{{}, {}};
    hobgoblin::log::SetMinimalLogSeverity(hobgoblin::log::Severity::All);
    HG_LOG_INFO("SPeMPE_CMake_Game", "It's working!");
    return EXIT_SUCCESS;
}
