#ifndef TYPHON_GAME_OBJECTS_GAMEPLAY_PHYSICS_BULLET_HPP
#define TYPHON_GAME_OBJECTS_GAMEPLAY_PHYSICS_BULLET_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/State_scheduler.hpp>

#include <Typhon/Framework.hpp>

#include "__Experimental/Lighting.hpp"

class PhysicsBullet : public SynchronizedObject, private Collideables::IProjectile {
public:
    struct VisibleState { // Visible state object must be public
        float x = 0.f;
        float y = 0.f;
        // bool hidden = true; // TODO This probably doesn't work right

        HG_ENABLE_AUTOPACK(VisibleState, x, y);
    };

    static constexpr auto SERIALIZABLE_TAG = "PhysicsBullet";

    PhysicsBullet(QAO_RuntimeRef rtRef, SynchronizedObjectRegistry& syncObjReg, SyncId syncId,
                  const VisibleState& initialState);

    ~PhysicsBullet();

    const VisibleState& getCurrentState() const {
        return _ssch.getCurrentState();
    }

    void initWithSpeed(const Collideables::ICreature* creator, double direction, double speed);

    void cannonicalSyncApplyUpdate(const VisibleState& state, int delay);

    void destroySelfIn(int steps);

    cpBody* getPhysicsBody() const;

protected:
    void eventUpdate() override;
    void eventPostUpdate() override;
    void eventDraw1() override;

private:
    hg::util::StateScheduler<VisibleState> _ssch;

    hg::cpBodyUPtr _body;
    hg::cpShapeUPtr _shape;
    const Collideables::ICreature* _creator;
    int _destroyCountdown = -1;
    bool _hitSomething = false;

    bool collisionBegin(Collideables::ICreature* other, cpArbiter* arbiter) const override;
    void collisionPostSolve(Collideables::ICreature* other, cpArbiter* arbiter) override;
    void collisionSeparate(Collideables::ICreature* other, cpArbiter* arbiter) override;

    void collisionPostSolve(Collideables::ITerrain* other, cpArbiter* arbiter) override;

    SPEMPE_GENERATE_CANNONICAL_SYNC_DECLARATIONS;
};

#endif // !TYPHON_GAME_OBJECTS_GAMEPLAY_PHYSICS_BULLET_HPP
