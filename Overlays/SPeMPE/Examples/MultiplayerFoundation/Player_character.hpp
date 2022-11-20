#pragma once

#include "Engine.h"

#include <Hobgoblin/Utility/Autopack.hpp>

struct PlayerCharacter_VisibleState {
    float x, y;
    int owningPlayerIndex = spe::PLAYER_INDEX_UNKNOWN;
    HG_ENABLE_AUTOPACK(PlayerCharacter_VisibleState, x, y, owningPlayerIndex);
};

class PlayerCharacter
    : public spe::SynchronizedObject<PlayerCharacter_VisibleState>
{
public:
    PlayerCharacter(QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId);

    ~PlayerCharacter() override;

    void init(int aOwningPlayerIndex);

private:
    void _eventUpdate(spe::IfMaster) override;

    void _eventDraw1() override;

    void _syncCreateImpl(spe::SyncDetails& aSyncDetails) const override;
    void _syncUpdateImpl(spe::SyncDetails& aSyncDetails) const override;
    void _syncDestroyImpl(spe::SyncDetails& aSyncDetails) const override;
};
