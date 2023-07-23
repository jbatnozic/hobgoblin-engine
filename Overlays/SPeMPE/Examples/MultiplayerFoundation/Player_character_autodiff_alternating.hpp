#pragma once

#include "Engine.h"

#include <cstdint>

SPEMPE_DEFINE_AUTODIFF_STATE(AutodiffAlternatingPlayerCharacter_VisibleState,
    SPEMPE_MEMBER(float, x, 0.f),
    SPEMPE_MEMBER(float, y, 0.f),
    SPEMPE_MEMBER(std::int32_t, owningPlayerIndex, spe::PLAYER_INDEX_UNKNOWN)
) {};

class AutodiffAlternatingPlayerCharacter
    : public spe::SynchronizedObject<AutodiffAlternatingPlayerCharacter_VisibleState>
{
public:
    AutodiffAlternatingPlayerCharacter(QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId);

    ~AutodiffAlternatingPlayerCharacter() override;

    void init(int aOwningPlayerIndex, float aX, float aY);

private:
    void _eventUpdate(spe::IfMaster) override;
    void _eventDraw1() override;
    void _eventFinalizeFrame(spe::IfMaster) override;

    void _syncCreateImpl(spe::SyncDetails& aSyncDetails) const override;
    void _syncUpdateImpl(spe::SyncDetails& aSyncDetails) const override;
    void _syncDestroyImpl(spe::SyncDetails& aSyncDetails) const override;
};
