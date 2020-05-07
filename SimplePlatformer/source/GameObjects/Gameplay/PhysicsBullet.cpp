
#include <cmath>

#include "GameContext/Game_context.hpp"
#include "GameObjects/Framework/Execution_priorities.hpp"
#include "GameObjects/Gameplay/PhysicsBullet.hpp"

RN_DEFINE_HANDLER(CreatePhysicsBullet, RN_ARGS(SyncId, syncId, PhysicsBullet::ViState&, state)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GameContext>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjMapper = global.syncObjMgr;

            QAO_PCreate<PhysicsBullet>(&runtime, syncObjMapper, syncId, state);
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

RN_DEFINE_HANDLER(UpdatePhysicsBullet, RN_ARGS(SyncId, syncId, PhysicsBullet::ViState&, state)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GameContext>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjMapper = global.syncObjMgr;
            auto* player = static_cast<PhysicsBullet*>(syncObjMapper.getMapping(syncId));

            const std::chrono::microseconds delay = client.getServer().getRemoteInfo().latency / 2LL;
            player->_ssch.putNewState(state, global.calcDelay(delay));
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

RN_DEFINE_HANDLER(DestroyPhysicsBullet, RN_ARGS(SyncId, syncId)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GameContext>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjMapper = global.syncObjMgr;
            auto* player = static_cast<PhysicsBullet*>(syncObjMapper.getMapping(syncId));

            QAO_PDestroy(player);
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

static void customDampingVelocityFunc(cpBody* body, cpVect gravity, cpFloat damping, cpFloat dt) {
    cpBodyUpdateVelocity(body, cpv(0.0, 0.0), 0.998, dt);
}

PhysicsBullet::PhysicsBullet(QAO_RuntimeRef rtRef, SynchronizedObjectManager& syncObjMgr, SyncId syncId,
                             const ViState& initialState)
    : GOF_SynchronizedObject{rtRef, TYPEID_SELF, EXEPR_CREATURES, "PhysicsPlayer", syncObjMgr, syncId}
    , _ssch{ctx().syncBufferLength}
{
    for (auto& state : _ssch) {
        state = initialState;
    }

    if (isMasterObject()) {
        auto* space = ctx().getPhysicsSpace();

        const cpFloat radius = 4.0;
        const cpFloat mass = 0.1;
        const cpFloat moment = cpMomentForCircle(mass, 0.0, radius, cpv(0, 0));

        _body = hg::cpBodyUPtr{cpSpaceAddBody(space, cpBodyNew(mass, moment))};
        _shape = hg::cpShapeUPtr{cpSpaceAddShape(space, cpCircleShapeNew(_body.get(), radius, cpv(0.0, 0.0)))};
        cpBodySetPosition(_body.get(), cpv(initialState.x, initialState.y));
        cpShapeSetElasticity(_shape.get(), 0.75);
        cpBodySetVelocityUpdateFunc(_body.get(), customDampingVelocityFunc);
        //cpShapeSetC
    }
}

PhysicsBullet::~PhysicsBullet() {
    if (isMasterObject()) {
        syncDestroy();
    }
}

void PhysicsBullet::initWithSpeed(double direction, double speed) {
    const cpVect force = cpv(std::cos(direction) * speed, std::sin(direction) * speed);
    cpBodyApplyImpulseAtLocalPoint(_body.get(), force, cpv(0, 0));
}

void PhysicsBullet::syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    Compose_CreatePhysicsBullet(node, rec, getSyncId(), _ssch.getCurrentState());
}

void PhysicsBullet::syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    Compose_UpdatePhysicsBullet(node, rec, getSyncId(), _ssch.getCurrentState());
}

void PhysicsBullet::syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    Compose_DestroyPhysicsBullet(node, rec, getSyncId());
}

void PhysicsBullet::eventUpdate() {
    if (!ctx().isPrivileged()) {
        _ssch.scheduleNewStates();
        _ssch.advanceDownTo(ctx().syncBufferLength * 2);
    }
}

void PhysicsBullet::eventPostUpdate() {   
    if (ctx().isPrivileged()) {
        auto& self = _ssch.getCurrentState();
        auto physicsPos = cpBodyGetPosition(_body.get());
        self.x = static_cast<float>(physicsPos.x);
        self.y = static_cast<float>(physicsPos.y);
    }
}

void PhysicsBullet::eventDraw1() {
    auto& canvas = ctx().windowMgr.getCanvas();
    auto& self = _ssch.getCurrentState();

    sf::CircleShape circ{4.0};
    circ.setOrigin(circ.getRadius(), circ.getRadius());
    circ.setFillColor(hg::gr::Color::Gold);
    circ.setPosition({self.x, self.y});
    canvas.draw(circ);
}