#pragma once

#include "Engine.h"

#include <Hobgoblin/Utility/Autopack.hpp>

#include <cstdint>

#if 1
SPEMPE_DEFINE_AUTODIFF_STATE(PlayerCharacterAlt_VisibleState,
    SPEMPE_MEMBER(float, x, 0.f),
    SPEMPE_MEMBER(float, y, 0.f),
    SPEMPE_MEMBER(std::int32_t, owningPlayerIndex, spe::PLAYER_INDEX_UNKNOWN)
) {
    // void setPackMode(spe::AutodiffPackMode) override {}
};
#else
struct PlayerCharacterAlt_VisibleState {
    float x = 0.f, y = 0.f;
    std::int32_t owningPlayerIndex = spe::PLAYER_INDEX_UNKNOWN;

    HG_ENABLE_AUTOPACK(PlayerCharacterAlt_VisibleState, x, y, owningPlayerIndex);

    void initMirror() {}
    void commit() {}
};
#endif

class PlayerCharacterAlt
    : public spe::SynchronizedObject<PlayerCharacterAlt_VisibleState>
{
public:
    PlayerCharacterAlt(QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId);

    ~PlayerCharacterAlt() override;

    void init(int aOwningPlayerIndex, float aX, float aY);

private:
    void _eventUpdate(spe::IfMaster) override;

    void _eventDraw1() override;
    void _eventFinalizeFrame(spe::IfMaster) override;

    void _syncCreateImpl(spe::SyncDetails& aSyncDetails) const override;
    void _syncUpdateImpl(spe::SyncDetails& aSyncDetails) const override;
    void _syncDestroyImpl(spe::SyncDetails& aSyncDetails) const override;
};
