
#include <cmath>

#include "Physics_bullet.hpp"

SPEMPE_GENERATE_CANNONICAL_HANDLERS(PhysicsBullet);

SPEMPE_GENERATE_CANNONICAL_SYNC_IMPLEMENTATIONS(PhysicsBullet);

static void customDampingVelocityFunc(cpBody* body, cpVect gravity, cpFloat damping, cpFloat dt) {
    //cpBodySetAngularVelocity(body, cpBodyGetAngularVelocity(body) * 0.9);
    cpBodyUpdateVelocity(body, cpv(0.0, 0.0), 1.0, dt);
}

PhysicsBullet::PhysicsBullet(QAO_RuntimeRef rtRef, SynchronizedObjectRegistry& syncObjReg, SyncId syncId,
                             const VisibleState& initialState)
    : SynchronizedObject{rtRef, SPEMPE_TYPEID_SELF, *PEXEPR_ENTITIES, "PhysicsPlayer", syncObjReg, syncId}
    , _ssch{ctx().syncBufferLength}
{
    for (auto& state : _ssch) {
        state = initialState;
    }

    if (isMasterObject()) {
        auto* space = ctx(DPhysicsSpace);

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

void PhysicsBullet::initWithSpeed(const Collideables::ICreature* creator, double direction, double speed) {
    const cpVect force = cpv(std::cos(direction) * speed, std::sin(direction) * speed);
    cpBodyApplyImpulseAtLocalPoint(_body.get(), force, cpv(0, 0));
    _creator = creator;
}

void PhysicsBullet::destroySelfIn(int steps) {
    ctx().addPostStepAction(std::max(steps, 0),
                            [this](GameContext&) {
                                QAO_PDestroy(this);
                            });
}

void PhysicsBullet::cannonicalSyncApplyUpdate(const VisibleState& state, int delay) {
    _ssch.putNewState(state, delay);
}

void PhysicsBullet::eventUpdate() {
    if (ctx().isPrivileged()) {
        if (_hitSomething) {
            QAO_PDestroy(this);
            //destroySelfInPostStep();
        }
    }
    else{
        _ssch.scheduleNewStates();
        _ssch.advance();
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
    auto& canvas = ctx(MWindow).getCanvas();
    auto& self = _ssch.getCurrentState();

    sf::CircleShape circ{8.0};
    circ.setOrigin(circ.getRadius(), circ.getRadius());
    circ.setFillColor(hg::gr::Color::Gold);
    circ.setPosition({self.x, self.y});
    canvas.draw(circ);
}

bool PhysicsBullet::collisionBegin(Collideables::ICreature* other, cpArbiter* arbiter) const {
    if (_hitSomething || _creator == other) {
        return REJECT_COLLISION;
    }
    return ACCEPT_COLLISION;
}

void PhysicsBullet::collisionPostSolve(Collideables::ICreature* other, cpArbiter* arbiter) {
    _hitSomething = true;
}

void PhysicsBullet::collisionSeparate(Collideables::ICreature* other, cpArbiter* arbiter) {
    if (_creator == other) {
        _creator = nullptr;
    }
}

void PhysicsBullet::collisionPostSolve(Collideables::ITerrain* other, cpArbiter* arbiter) {
    _hitSomething = true;
}
