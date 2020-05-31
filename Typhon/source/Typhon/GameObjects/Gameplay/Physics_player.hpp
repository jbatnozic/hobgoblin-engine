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
    struct VisibleState { // Visible state object must be public
        hg::PZInteger playerIndex = -1; // TODO Magic number
        float x = 0.f;
        float y = 0.f;
        float angle;
        bool hidden = true; // TODO This probably doesn't work right

        HG_ENABLE_AUTOPACK(VisibleState, playerIndex, x, y, angle);
    };

    static constexpr auto SERIALIZABLE_TAG = "PhysicsPlayer";

    static constexpr float MAX_SPEED = 5.f;
    static constexpr float GRAVITY = 1.f;
    static constexpr float JUMP_POWER = 16.f;

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
    void eventUpdate() override;
    void eventPostUpdate() override;
    void eventDraw1() override;

private:
    hg::util::StateScheduler<VisibleState> _ssch;

    hg::cpBodyUPtr _body;
    hg::cpShapeUPtr _shape;
    LightingController::LightHandle _lightHandle;
    int _fireCounter;

    void collisionPostSolve(Collideables::IProjectile* proj, cpArbiter* arb) override {
        std::cout << "hit POSTSOLVE\n";
    }

    SPEMPE_GENERATE_CANNONICAL_SYNC_DECLARATIONS;
};

#endif // !PLAYER_PHYSICS_PLAYER_HPP
