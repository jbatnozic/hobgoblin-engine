
#include <Hobgoblin/Math.hpp>

#include <cmath>

#include "../Control/Controls_manager.hpp"
#include "../Control/Environment_manager.hpp"
#include "Physics_bullet.hpp"
#include "Physics_player.hpp"

static void customDampingVelocityFunc(cpBody* body, cpVect gravity, cpFloat damping, cpFloat dt) {
    cpBodySetAngularVelocity(body, cpBodyGetAngularVelocity(body) * 0.9);
    cpBodyUpdateVelocity(body, cpv(0.0, 0.0), 1.0, dt);
}

SPEMPE_GENERATE_CANNONICAL_HANDLERS(PhysicsPlayer);

SPEMPE_GENERATE_CANNONICAL_SYNC_IMPLEMENTATIONS(PhysicsPlayer);

PhysicsPlayer::PhysicsPlayer(QAO_RuntimeRef rtRef, SynchronizedObjectRegistry& syncObjReg, SyncId syncId,
                             const VisibleState& initialState)
    : SynchronizedObject{rtRef, SPEMPE_TYPEID_SELF, *PEXEPR_ENTITIES, "PhysicsPlayer", syncObjReg, syncId}
    , _ssch{ctx().syncBufferLength}
{
    for (auto& state : _ssch) {
        state = initialState;
        if (isMasterObject()) {
            state.hidden = false;
        }
    }

    if (ctx().isPrivileged()) {
        auto* space = ctx(DPhysicsSpace);

        cpFloat radius = 8.0;
        cpFloat mass = 1.0;
        cpFloat moment = cpMomentForCircle(mass, 0.0, radius, cpv(0, 0));

        _body = hg::cpBodyUPtr{cpSpaceAddBody(space, cpBodyNew(mass, moment))};
        _shape = hg::cpShapeUPtr{cpSpaceAddShape(space, cpCircleShapeNew(_body.get(), radius, cpv(0.0, 0.0)))};
        cpBodySetPosition(_body.get(), cpv(initialState.x, initialState.y));
        cpShapeSetElasticity(_shape.get(), 0.5);
        cpBodySetVelocityUpdateFunc(_body.get(), customDampingVelocityFunc);
        Collideables::initCreature(_shape.get(), *this);
    }

    _lightHandle = ctx(MEnvironment).addLight(initialState.x, initialState.y, hg::gr::Color::AntiqueWhite, 8.f);
}

PhysicsPlayer::~PhysicsPlayer() {
    if (isMasterObject()) {
        syncDestroy();
    }
}

void PhysicsPlayer::eventUpdate() {
    if (ctx().isPrivileged()) {
        auto& self = _ssch.getCurrentState();
        PlayerControls controls = ctx(MControls).getCurrentControlsForPlayer(self.playerIndex);

        const cpFloat left = controls.left;
        const cpFloat right = controls.right;
        const cpFloat up = controls.up;
        const cpFloat down = controls.down;

        // Propulsion
        cpBody* const body = _body.get();

        if (up) {
            const cpVect pos = cpBodyGetPosition(body);
            const cpVect force = cpBodyGetRotation(body) * 100; //cpv((right - left) * 1000.0, (down - up) * 1000.0);
            cpBodyApplyForceAtWorldPoint(body, force, pos);
        } 

        // Rotation
        {
            // Manual turning:
#if 0
            if (left) {
                const auto rotForce = cpv(0.0, -15.0);
                cpBodyApplyForceAtLocalPoint(body, rotForce, cpv(12.0, 0.0));
                cpBodyApplyForceAtLocalPoint(body, cpvneg(rotForce), cpvzero);
            }
            else if (right) {
                const auto rotForce = cpv(0.0, +15.0);
                cpBodyApplyForceAtLocalPoint(body, rotForce, cpv(12.0, 0.0));
                cpBodyApplyForceAtLocalPoint(body, cpvneg(rotForce), cpvzero);
            }
#else
            using hg::math::Angle;
            using hg::math::PointDirection;
            const Angle currentRotation = Angle::fromVector(cpBodyGetRotation(body).x,
                                                            cpBodyGetRotation(body).y);

            const auto selfPos  = cpBodyGetPosition(body);
            const auto mousePos = cpv(controls.mouseX, controls.mouseY);

            const Angle targetRotation = PointDirection(selfPos, mousePos);

            const Angle diff = currentRotation.shortestDistanceTo(targetRotation);

            cpVect rotForce;
            if (diff.asRadians() < 0.0) {
                rotForce = cpv(0.0, -75.0);
            }
            else {
                rotForce = cpv(0.0, +75.0);
            }

            rotForce = rotForce * std::sin(0.5 * std::abs(diff.asRadians()));

            cpBodyApplyForceAtLocalPoint(body, rotForce, cpv(12.0, 0.0));
            cpBodyApplyForceAtLocalPoint(body, cpvneg(rotForce), cpvzero);

            //cpVect turningN = cpv(1.0, 1.1);
            ////if (right) {
            ////    turningN = cpv(1.0, 1.0);
            ////}
            ////else {
            ////    turningN = cpvzero;
            ////}

            //const cpFloat dot = cpvdot(turningN, cpvnormalize(cpBodyGetRotation(body)));
            //const cpFloat cross = cpvcross(turningN, cpvnormalize(cpBodyGetRotation(body)));

            //cpVect rotN;
            //if (cross <= 0) {
            //    rotN = cpvperp(cpvnormalize(cpBodyGetRotation(body)));
            //}
            //else {
            //    rotN = cpvrperp(cpvnormalize(cpBodyGetRotation(body)));
            //}

            //cpVect rotF = cpvmult(rotN, 300 * (1 - dot));
            //cpBodyApplyForceAtLocalPoint(body, rotF, cpv(1.0, 1.0));
            //cpBodyApplyForceAtLocalPoint(body, cpvneg(rotF), cpvzero);
#endif
        }

        if (CountPeriodic(&_fireCounter, 12, controls.fire)) {
            auto selfPos = cpBodyGetPosition(_body.get());

            PhysicsBullet::VisibleState vs;
            vs.x = selfPos.x;
            vs.y = selfPos.y;
            auto* bullet = QAO_PCreate<PhysicsBullet>(getRuntime(), ctx().getSyncObjReg(), SYNC_ID_NEW, vs);
            bullet->initWithSpeed(this, std::atan2(controls.mouseY - selfPos.y, controls.mouseX - selfPos.x), 50.0);
        }
    }
    else {
        _ssch.scheduleNewStates();
        _ssch.advance();
        _ssch.advanceDownTo(ctx().syncBufferLength * 2);
    }

    {
        auto& view = ctx(MWindow).getView();
        auto& self = _ssch.getCurrentState();

        if (self.playerIndex == ctx().getLocalPlayerIndex()) {
            auto dist = hg::math::EuclideanDist<float>({self.x, self.y}, view.getCenter());
            if (dist < 4.f) {
                view.setCenter({self.x, self.y});
            }
            else {
                auto direction = atan2(self.y - view.getCenter().y, self.x - view.getCenter().x);
                view.move({std::cosf(direction) * dist / 2.f, std::sinf(direction) * dist / 2.f});
            }
            view.setCenter(std::roundf(view.getCenter().x), std::roundf(view.getCenter().y));
        }
    }
}

void PhysicsPlayer::eventPostUpdate() {
    auto& self = _ssch.getCurrentState();
    if (ctx().isPrivileged()) {
        auto physicsPos = cpBodyGetPosition(_body.get());
        self.x = static_cast<float>(physicsPos.x);
        self.y = static_cast<float>(physicsPos.y);
    }

    _lightHandle.setPosition(self.x, self.y);
}

void PhysicsPlayer::eventDraw1() {
    static const sf::Color COLORS[] = {sf::Color::Red, sf::Color::Yellow, sf::Color::Blue, sf::Color::Green};

    auto& canvas = ctx(MWindow).getCanvas();
    auto& self = _ssch.getCurrentState();

    sf::CircleShape circ{12.0};
    circ.setOrigin(circ.getRadius(), circ.getRadius());
    circ.setFillColor(COLORS[self.playerIndex]);
    circ.setOutlineColor(sf::Color::Black);
    circ.setOutlineThickness(2.f);
    circ.setPosition({self.x, self.y});
    canvas.draw(circ);

    if (isMasterObject()) {
        sf::CircleShape circ{2.0};
        circ.setOrigin(circ.getRadius(), circ.getRadius());
        circ.setFillColor(hg::gr::Color::AliceBlue);

        float xoff = std::cos(cpBodyGetAngle(_body.get())) * 12.0;
        float yoff = std::sin(cpBodyGetAngle(_body.get())) * 12.0;

        circ.setPosition({self.x + xoff, self.y + yoff});
        canvas.draw(circ);
    }
}