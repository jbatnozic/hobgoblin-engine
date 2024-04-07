
#include <Hobgoblin/Logging.hpp>
#include <SPeMPE/SPeMPE.hpp>

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "Actors.hpp"
#include "Singleplayer.hpp"
#include "Multiplayer.hpp"

static constexpr auto LOG_ID = "SPeMPE.ManualTest";

int main(int argc, char* argv[]) {
  hg::log::SetMinimalLogSeverity(hg::log::Severity::Debug);
  RN_IndexHandlers();

  const char mode = (argc >= 2) ? (argv[1][0]) : 'n';
  switch (mode) {
  case 's': // singleplayer
      {
          auto context = multiplayer::CreateHostGameContext();
          auto result = context->runFor(-1);
          HG_LOG_INFO(LOG_ID, "Singleplayer context exited with status {}.", result);
      }
      break;

  case 'h': // host (multiplayer)
      {
          auto context = multiplayer::CreateHostGameContext();
          const auto serverPort =
              context->getComponent<spe::NetworkingManagerInterface>().getServer().getLocalPort();
          HG_LOG_INFO(LOG_ID, "Server started on port {}.", serverPort);
          auto result = context->runFor(-1);
          HG_LOG_INFO(LOG_ID, "Host exited with status {}.", result);
      }
      break;

  case 'c': // client (multiplayer)
      {
          auto context = multiplayer::CreateClientGameContext(std::stoi(argv[2]));
          auto result = context->runFor(-1);
          HG_LOG_INFO(LOG_ID, "Client 1 exited with status {}.", result);
      }
      break;

  case 'n': // none
  default:
      HG_LOG_INFO(LOG_ID, "Unknown mode '{}'; exiting.", mode);
      break;
  }

  return EXIT_SUCCESS;
}
