
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>
#include <SPeMPE/SPeMPE.hpp>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "Actors.hpp"
#include "Singleplayer.hpp"
#include "Multiplayer.hpp"

#include "Simple_zerotier.hpp"

static constexpr auto LOG_ID = "SPeMPE.ManualTest";

int main(int argc, char* argv[]) try {
  hg::log::SetMinimalLogSeverity(hg::log::Severity::Debug);
  RN_IndexHandlers();

  //const auto localIpAddr = SimpleZeroTierInit(
  //    "ZeroTierNodeIdentity",
  //    9998,
  //    0xd3ecf5726d81ccb3,
  //    std::chrono::seconds{20}
  //);
  //if (!localIpAddr) {
  //    HG_THROW_TRACED(hg::TracedRuntimeError, 0, "Failed to initialize ZeroTier.");
  //}

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
          auto context = multiplayer::CreateClientGameContext(argv[2], std::stoi(argv[3]));
          auto result = context->runFor(-1);
          HG_LOG_INFO(LOG_ID, "Client 1 exited with status {}.", result);
      }
      break;

  case 'n': // none
  default:
      HG_LOG_INFO(LOG_ID, "Unknown mode '{}'; exiting.", mode);
      break;
  }

  // SimpleZeroTierStop();

  return EXIT_SUCCESS;
} catch (const hg::TracedException& ex) {
    HG_LOG_FATAL(LOG_ID, "Traced exception caught: {}", ex.getFullFormattedDescription());
    return EXIT_FAILURE;
} catch (const std::exception& ex) {
    HG_LOG_FATAL(LOG_ID, "Exception caught: {}", ex.what());
    return EXIT_FAILURE;
}
catch (...) {
    HG_LOG_FATAL(LOG_ID, "Exception caught: UNKNOWN");
    return EXIT_FAILURE;
}
