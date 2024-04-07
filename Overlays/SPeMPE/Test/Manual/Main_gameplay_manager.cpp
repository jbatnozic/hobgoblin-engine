
#include "Main_gameplay_manager.hpp"

#include <Hobgoblin/Input.hpp>
#include <Hobgoblin/Logging.hpp>

#include <chrono>

#include "Actors.hpp"
#include "Config.hpp"

static constexpr auto LOG_ID = "SPeMPE.ManualTest";

void MainGameplayManagerBase::_eventPreUpdate() {
    if (_periodicStopwatch.getElapsedTime() >= std::chrono::seconds{5}) {
        const auto time = _periodicStopwatch.restart<std::chrono::milliseconds>();
        const auto cycles = ctx().getCurrentIterationOrdinal();

        const auto expectedCycles =
            _totalStopwatch.getElapsedTime<std::chrono::milliseconds>().count() / 1000.0 * DESIRED_FRAMERATE;

        HG_LOG_INFO(LOG_ID,
                    "Timing update: last_period={}ms, total_time={}ms, total_cycles={}, accurracy={}%",
                    time.count(),
                    _totalStopwatch.getElapsedTime<std::chrono::milliseconds>().count(),
                    cycles,
                    (1.0 - std::abs(expectedCycles - cycles) / expectedCycles) * 100.0
                    );
    }

    if (hg::in::CheckPressedPK(hg::in::PK_RSHIFT)) {
        ctx().stop();
    }
}

namespace singleplayer {

MainGameplayManager::MainGameplayManager(QAO_RuntimeRef aRuntimeRef)
    : MainGameplayManagerBase{aRuntimeRef}
{
    QAO_RuntimeRef rt = ctx().getQAORuntime();
    auto* p = QAO_PCreate<BasicActor>(
        rt,
        ccomp<spe::NetworkingManagerInterface>().getRegistryId(),
        spe::SYNC_ID_NEW
    );
    p->init(32.f, 32.f, hg::gr::COLOR_PURPLE, 0);
}

} // namespace singleplayer

namespace multiplayer {

MainGameplayManager::MainGameplayManager(QAO_RuntimeRef aRuntimeRef)
    : MainGameplayManagerBase{aRuntimeRef}
{
    if (ctx().isPrivileged()) {
        QAO_RuntimeRef rt = ctx().getQAORuntime();

#if 1
        auto* basicActor = QAO_PCreate<BasicActor>(
            rt,
            ccomp<spe::NetworkingManagerInterface>().getRegistryId(),
            spe::SYNC_ID_NEW
        );
        basicActor->init(32.f, 32.f, hg::gr::COLOR_PURPLE, 0);
#endif
#if 1
        auto* autodiffActor = QAO_PCreate<AutodiffActor>(
            rt,
            ccomp<spe::NetworkingManagerInterface>().getRegistryId(),
            spe::SYNC_ID_NEW
        );
        autodiffActor->init(32.f, 96.f, hg::gr::COLOR_ORANGE, 0);
#endif
#if 1
        auto* alternatingActor = QAO_PCreate<AlternatingActor>(
            rt,
            ccomp<spe::NetworkingManagerInterface>().getRegistryId(),
            spe::SYNC_ID_NEW
        );
        alternatingActor->init(32.f, 160.f, hg::gr::COLOR_ROYAL_BLUE, 0);
#endif
#if 1
        auto* alternatingAutodiffActor = QAO_PCreate<AlternatingAutodiffActor>(
            rt,
            ccomp<spe::NetworkingManagerInterface>().getRegistryId(),
            spe::SYNC_ID_NEW
        );
        alternatingAutodiffActor->init(32.f, 224.f, hg::gr::COLOR_FOREST_GREEN, 0);
#endif
    }
}

} // namespace multiplayer
