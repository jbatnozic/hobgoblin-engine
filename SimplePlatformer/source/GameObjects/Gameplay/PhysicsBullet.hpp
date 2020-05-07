#ifndef PLAYER_PHYSICS_BULLET_HPP
#define PLAYER_PHYSICS_BULLET_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_Macros.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/State_scheduler.hpp>

#include "GameObjects/Framework/Game_object_framework.hpp"
#include "Experimental/Lighting.hpp"

class PhysicsBullet : public GOF_SynchronizedObject {
public:
    struct ViState { // Visible state object must be public
        // hg::PZInteger playerIndex = -1; // TODO Magic number
        float x = 0.f;
        float y = 0.f;
        // bool hidden = true; // TODO This probably doesn't work right

        HG_ENABLE_AUTOPACK(ViState, x, y);
    };

    static constexpr auto SERIALIZABLE_TAG = "PhysicsBullet";

    PhysicsBullet(QAO_RuntimeRef rtRef, SynchronizedObjectManager& syncObjMgr, SyncId syncId,
                  const ViState& initialState = ViState{});

    ~PhysicsBullet();

    void initWithSpeed(double direction, double speed);

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

    friend RN_HANDLER_SIGNATURE(UpdatePhysicsBullet, RN_ARGS(SyncId, syncId, PhysicsBullet::ViState&, state));
};

#endif // !PLAYER_PHYSICS_BULLET_HPP
