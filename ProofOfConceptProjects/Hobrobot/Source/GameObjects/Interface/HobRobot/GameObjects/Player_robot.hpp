#pragma once

#include "Engine.hpp"

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>

#include <memory>

namespace hobrobot {

struct BodyPart {
    float x, y;
    float width, height;
    float angle;
    HG_ENABLE_AUTOPACK(BodyPart, x, y, width, height, angle);
};

struct PlayerRobot_VisibleState {
    // Torso
    BodyPart torso;
    // Legs
    BodyPart backThigh;
    BodyPart backShin;
    BodyPart backFoot;
    BodyPart frontThigh;
    BodyPart frontShin;
    BodyPart frontFoot;
    // Arms
    BodyPart backUpperArm;
    BodyPart backForeArm;
    BodyPart backHand;
    BodyPart frontUpperArm;
    BodyPart frontForeArm;
    BodyPart frontHand;

    HG_ENABLE_AUTOPACK(PlayerRobot_VisibleState,
                       torso, 
                       backThigh, backShin, backFoot, frontThigh, frontShin, frontFoot,
                       backUpperArm, backForeArm, backHand, frontUpperArm, frontForeArm, frontHand);
};

class PlayerRobot
    : public spe::SynchronizedObject<PlayerRobot_VisibleState>
{
public:
    PlayerRobot(QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId);

    ~PlayerRobot() override;

    void init(cpFloat aX, cpFloat aY);

private:
    class MasterData;
    std::unique_ptr<MasterData> _masterData;

    void _eventUpdate1(spe::IfMaster) override;
    void _eventUpdate1(spe::IfDummy) override;
    void _eventDraw1() override;

    void _syncCreateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncUpdateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncDestroyImpl(spe::SyncControlDelegate& aSyncCtrl) const override;

    void _adjustCamera();
};

} // namespace hobrobot
