#ifndef PLAYER_PHYSICS_BULLET_HPP
#define PLAYER_PHYSICS_BULLET_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_Macros.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/State_scheduler.hpp>

#include "GameObjects/Framework/Game_object_framework.hpp"
#include "GameObjects/Gameplay/Collisions.hpp"
#include "Experimental/Lighting.hpp"

class PhysicsBullet : public GOF_SynchronizedObject, private Collideables::IProjectile {
public:
    struct VisibleState { // Visible state object must be public
        // hg::PZInteger playerIndex = -1; // TODO Magic number
        float x = 0.f;
        float y = 0.f;
        // bool hidden = true; // TODO This probably doesn't work right

        HG_ENABLE_AUTOPACK(VisibleState, x, y);
    };

    static constexpr auto SERIALIZABLE_TAG = "PhysicsBullet";

    PhysicsBullet(QAO_RuntimeRef rtRef, GOF_SynchronizedObjectRegistry& syncObjReg, GOF_SyncId syncId,
                  const VisibleState& initialState = VisibleState{});

    ~PhysicsBullet();

    const VisibleState& getCurrentState() const {
        return _ssch.getCurrentState();
    }

    void initWithSpeed(const Collideables::ICreature* creator, double direction, double speed);

    void cannonicalSyncApplyUpdate(const VisibleState& state, int delay);

protected:
    void eventUpdate() override;
    void eventPostUpdate() override;
    void eventDraw1() override;

private:
    hg::util::StateScheduler<VisibleState> _ssch;

    hg::cpBodyUPtr _body;
    hg::cpShapeUPtr _shape;
    const Collideables::ICreature* _creator;

    cpBool collisionBegin(Collideables::ICreature* other, cpArbiter* arbiter) override;
    void collisionPostSolve(Collideables::ICreature* other, cpArbiter* arbiter) override;
    void collisionSeparate(Collideables::ICreature* other, cpArbiter* arbiter) override;

    GOF_GENERATE_CANNONICAL_SYNC_DECLARATIONS;
};

#endif // !PLAYER_PHYSICS_BULLET_HPP
