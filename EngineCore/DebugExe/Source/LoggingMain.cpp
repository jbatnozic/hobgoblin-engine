// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Logging.hpp>

#include <string>

namespace hg = jbatnozic::hobgoblin;

constexpr const char* LOG_ID = "LoggingMain";

int main(int argc, char* argv[]) {
    hg::log::SetMinimalLogSeverity(hg::log::Severity::All);

    HG_LOG_INFO(LOG_ID, "{} is {} logging!", "This", std::string{"Hobgoblin"});
    HG_LOG_WARN(LOG_ID, "{} is {} logging again!", "This", std::string{ "Hobgoblin" });
}

// clang-format on
