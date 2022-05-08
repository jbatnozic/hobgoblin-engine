
#include <Hobgoblin/Logging.hpp>

#include <string>

namespace hg = jbatnozic::hobgoblin;

constexpr const char* LOG_ID = "LoggingMain";

int main(int argc, char* argv[]) {
    HG_LOG_INFO(LOG_ID, "{} is {} logging!", "This", std::string{"Hobgoblin"});
    HG_LOG_WARN(LOG_ID, "{} is {} logging again!", "This", std::string{ "Hobgoblin" });
}