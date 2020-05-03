#ifndef PLAYER_PHYSICS_PLAYER_HPP
#define PLAYER_PHYSICS_PLAYER_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/State_scheduler.hpp>

#include "GameObjects/Framework/Game_object_framework.hpp"
#include "Experimental/Lighting.hpp"

class PhysicsPlayer : public GOF_SynchronizedObject {
public:
    struct ViState { // Visible state object must be public
        hg::PZInteger playerIndex = -1; // TODO Magic number
        float x = 0.f;
        float y = 0.f;
        bool hidden = true; // TODO This probably doesn't work right

        HG_ENABLE_AUTOPACK(ViState, playerIndex, x, y);
    };

    static constexpr auto SERIALIZABLE_TAG = "PhysicsPlayer";

    static constexpr float MAX_SPEED = 5.f;
    static constexpr float GRAVITY = 1.f;
    static constexpr float JUMP_POWER = 16.f;

    PhysicsPlayer(QAO_RuntimeRef rtRef, SynchronizedObjectManager& syncObjMgr, SyncId syncId, 
                  const ViState& initialState);

    ~PhysicsPlayer();

    const ViState& getState() {
        return _ssch.getCurrentState();
    }

    void syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;
    void syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;
    void syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;

protected:
    void eventUpdate() override;
    void eventPostUpdate() override;
    void eventDraw1() override;

private:
    hg::util::StateScheduler<ViState> _ssch;

    hg::cpBodyUPtr _body;
    hg::cpShapeUPtr _shape;
    LightingController::LightHandle _lightHandle;

    friend RN_HANDLER_SIGNATURE(UpdatePhysicsPlayer, RN_ARGS(SyncId, syncId, PhysicsPlayer::ViState&, state));
};

#endif // !PLAYER_PHYSICS_PLAYER_HPP
