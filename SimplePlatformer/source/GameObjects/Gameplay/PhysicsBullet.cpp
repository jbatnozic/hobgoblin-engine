
#include <cmath>

#include "GameContext/Game_context.hpp"
#include "GameObjects/Framework/Execution_priorities.hpp"
#include "GameObjects/Gameplay/PhysicsBullet.hpp"



GOF_GENERATE_CANNONICAL_HANDLERS(PhysicsBullet);

GOF_GENERATE_CANNONICAL_SYNC_IMPLEMENTATIONS(PhysicsBullet);

static void customDampingVelocityFunc(cpBody* body, cpVect gravity, cpFloat damping, cpFloat dt) {
    cpBodyUpdateVelocity(body, cpv(0.0, 0.0), 0.998, dt);
}

PhysicsBullet::PhysicsBullet(QAO_RuntimeRef rtRef, GOF_SynchronizedObjectRegistry& syncObjReg, GOF_SyncId syncId,
                             const VisibleState& initialState)
    : GOF_SynchronizedObject{rtRef, TYPEID_SELF, EXEPR_CREATURES, "PhysicsPlayer", syncObjReg, syncId}
    , _ssch{ctx().syncBufferLength}
{
    for (auto& state : _ssch) {
        state = initialState;
    }

    if (isMasterObject()) {
        auto* space = ctx().getPhysicsSpace();

        const cpFloat radius = 8.0;
        const cpFloat mass = 0.1;
        const cpFloat moment = cpMomentForCircle(mass, 0.0, radius, cpv(0, 0));

        _body = hg::cpBodyUPtr{cpSpaceAddBody(space, cpBodyNew(mass, moment))};
        _shape = hg::cpShapeUPtr{cpSpaceAddShape(space, cpCircleShapeNew(_body.get(), radius, cpv(0.0, 0.0)))};
        cpBodySetPosition(_body.get(), cpv(initialState.x, initialState.y));
        cpShapeSetElasticity(_shape.get(), 0.75);
        cpBodySetVelocityUpdateFunc(_body.get(), customDampingVelocityFunc);
        Collideables::initProjectile(_shape.get(), *this);
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

void PhysicsBullet::cannonicalSyncApplyUpdate(const VisibleState& state, int delay) {
    _ssch.putNewState(state, delay);
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