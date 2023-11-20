#ifndef PLAYER_PHYSICS_PLAYER_HPP
#define PLAYER_PHYSICS_PLAYER_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/State_scheduler.hpp>

#include <Typhon/Framework.hpp>

#include "__Experimental/Lighting.hpp"
#include <Typhon/Utility/Periodic_counter.hpp>

class PhysicsPlayer : public SynchronizedObject, private Collideables::ICreature {
public:
    static constexpr double MAX_HEALTH = 100.0;
    static constexpr double MAX_SHIELD = 100.0;
    static constexpr double SHIELD_REGEN_RATE = 10.0 / 60.0;
    static constexpr hg::PZInteger INVUL_STEPS_AFTER_SPAWN = 180; // 3 sec
    static constexpr hg::PZInteger STUN_STEPS_AFTER_COLLISION = 120; // 2 sec

    struct VisibleState { // Visible state object must be public
        hg::PZInteger playerIndex = -1; // TODO Magic number
        float x = 0.f;
        float y = 0.f;
        float angle;
        float health = static_cast<float>(MAX_HEALTH);
        float shield = static_cast<float>(MAX_SHIELD);
        bool hidden = true; // TODO This probably doesn't work right

        HG_ENABLE_AUTOPACK(VisibleState, playerIndex, x, y, angle, health, shield);
    };

    static constexpr auto SERIALIZABLE_TAG = "PhysicsPlayer";



    PhysicsPlayer(QAO_RuntimeRef rtRef, SynchronizedObjectRegistry& syncObjReg, SyncId syncId, 
                  const VisibleState& initialState);

    ~PhysicsPlayer();

    const VisibleState& getCurrentState() const {
        return _ssch.getCurrentState();
    }

    void cannonicalSyncApplyUpdate(const VisibleState& state, int delay) {
        _ssch.putNewState(state, delay);
    }

    void destroySelfIn(int steps) {
        QAO_PDestroy(this);
    }

protected:
    void _eventUpdate1() override;
    void _eventPostUpdate() override;
    void _eventDraw1() override;
    void _eventDrawGUI() override;

private:
    hg::util::StateScheduler<VisibleState> _ssch;

    double _health = MAX_HEALTH;
    double _shield = MAX_SHIELD;
    hg::cpBodyUPtr _body;
    hg::cpShapeUPtr _shape;
    LightingController::LightHandle _lightHandle;
    hg::PZInteger _invulCounter = INVUL_STEPS_AFTER_SPAWN;
    hg::PZInteger _stunCounter = 0;
    int _fireCounter;

    void collisionPostSolve(Collideables::ICreature* terr, cpArbiter* arb) override;
    void collisionPostSolve(Collideables::IProjectile* proj, cpArbiter* arb) override;
    void collisionPostSolve(Collideables::ITerrain* terr, cpArbiter* arb) override;

    void _takeDamage(double damage);

    SPEMPE_GENERATE_CANNONICAL_SYNC_DECLARATIONS;
};

#endif // !PLAYER_PHYSICS_PLAYER_HPP
