#pragma once

#include "Engine.h"

#include <Hobgoblin/Utility/Autopack.hpp>

#include <cstdint>

SPEMPE_DEFINE_AUTODIFF_STATE(PlayerCharacter_VisibleState,
    SPEMPE_MEMBER(float, x, 0.f),
    SPEMPE_MEMBER(float, y, 0.f),
    SPEMPE_MEMBER(std::int32_t, owningPlayerIndex, spe::PLAYER_INDEX_UNKNOWN)
) {};

class PlayerCharacter
    : public spe::SynchronizedObject<PlayerCharacter_VisibleState>
{
public:
    PlayerCharacter(QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId);

    ~PlayerCharacter() override;

    void init(int aOwningPlayerIndex, float aX, float aY);

private:
    void _eventUpdate(spe::IfMaster) override;

    void _eventDraw1() override;

    void _syncCreateImpl(spe::SyncDetails& aSyncDetails) const override;
    void _syncUpdateImpl(spe::SyncDetails& aSyncDetails) const override;
    void _syncDestroyImpl(spe::SyncDetails& aSyncDetails) const override;
};
