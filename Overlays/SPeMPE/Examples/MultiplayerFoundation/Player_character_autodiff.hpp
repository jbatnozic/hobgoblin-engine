#pragma once

#include "Engine.h"

#include <cstdint>

SPEMPE_DEFINE_AUTODIFF_STATE(AutodiffPlayerCharacter_VisibleState,
    SPEMPE_MEMBER(float, x, 0.f),
    SPEMPE_MEMBER(float, y, 0.f),
    SPEMPE_MEMBER(std::int32_t, owningPlayerIndex, spe::PLAYER_INDEX_UNKNOWN)
) {};

/**
 * Implementation of a synchronized object with autodiff state optimization enabled.
 * (members that didn't change are automatically detected and are not sent to clients).
 */
class AutodiffPlayerCharacter
    : public spe::SynchronizedObject<AutodiffPlayerCharacter_VisibleState>
{
public:
    AutodiffPlayerCharacter(QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId);

    ~AutodiffPlayerCharacter() override;

    void init(int aOwningPlayerIndex, float aX, float aY);

private:
    void _eventUpdate1(spe::IfMaster) override;
    void _eventDraw1() override;
    void _eventFinalizeFrame(spe::IfMaster) override;

    void _syncCreateImpl(spe::SyncDetails& aSyncDetails) const override;
    void _syncUpdateImpl(spe::SyncDetails& aSyncDetails) const override;
    void _syncDestroyImpl(spe::SyncDetails& aSyncDetails) const override;
};
