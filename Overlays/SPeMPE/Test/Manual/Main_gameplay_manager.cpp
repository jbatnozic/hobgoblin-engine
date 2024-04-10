
#include "Main_gameplay_manager.hpp"

#include <Hobgoblin/Input.hpp>
#include <Hobgoblin/Logging.hpp>

#include <chrono>

#include "Actors.hpp"
#include "Config.hpp"

static constexpr auto LOG_ID = "SPeMPE.ManualTest";

MainGameplayManagerBase::MainGameplayManagerBase(QAO_RuntimeRef aRuntimeRef)
    : spe::NonstateObject{aRuntimeRef,
                          SPEMPE_TYPEID_SELF, 
                          PRIORITY_GAMEPLAYMGR,
                          "MainGameplayManager"}
{
    const int execPriority = 10; // not really important for these objects
    const int cycleLength  = 600; // 10 seconds @ 60fps
    {
        const auto config = spe::EventLoopTimingReporter::Config{ cycleLength };
        QAO_PCreate<spe::EventLoopTimingReporter>(ctx().getQAORuntime(), execPriority, config);
    }
        {
        const auto config = spe::NetworkingTelemetryReporter::Config{ cycleLength };
        QAO_PCreate<spe::NetworkingTelemetryReporter>(ctx().getQAORuntime(), execPriority, config);

        ccomp<spe::NetworkingManagerInterface>().setTelemetryCycleLimit(cycleLength);
    }
}

void MainGameplayManagerBase::_eventPreUpdate() {
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
