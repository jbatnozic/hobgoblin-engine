
#include <Hobgoblin/Graphics.hpp>
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

static std::string MakeBarString(hg::PZInteger amount) {
    std::string rv;
    rv.resize(hg::ToSz(amount), '|');
    return rv;
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
        cpFloat mass = 0.3;
        //cpFloat moment = cpMomentForCircle(mass, 0.0, radius, cpv(0, 0));
        cpFloat moment = cpMomentForBox(mass, 58.0, 38.0);

        _body.reset(cpSpaceAddBody(space, cpBodyNew(mass, moment)));
        //_shape.reset(cpSpaceAddShape(space, cpCircleShapeNew(_body.get(), radius, cpv(0.0, 0.0))));
        _shape.reset(cpSpaceAddShape(space, cpBoxShapeNew(_body.get(), 58.0, 38.0, 1.0)));
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

    auto& self = _ssch.getCurrentState();
    std::cout << "Player " << self.playerIndex << " died.\n";
}

void PhysicsPlayer::eventUpdate() {
    using hg::math::Angle;
    using hg::math::PointDirection;

    if (ctx().isPrivileged()) {
        if (_health <= 0.0) {
            QAO_PDestroy(this);
            return;
        }

        _shield = std::min(_shield + SHIELD_REGEN_RATE, MAX_SHIELD);

        auto& self = _ssch.getCurrentState();
        PlayerControls controls = ctx(MControls).getCurrentControlsForPlayer(self.playerIndex);

        const cpFloat left = controls.left;
        const cpFloat right = controls.right;
        const cpFloat up = controls.up;
        const cpFloat down = controls.down;

        // Propulsion
        cpBody* const body = _body.get();
        const cpVect pos = cpBodyGetPosition(body);
        const cpFloat propulsionStrength = 100.0;

        if (up) {
            const cpVect force = cpvmult(cpBodyGetRotation(body), propulsionStrength);
            cpBodyApplyForceAtWorldPoint(body, force, pos);
        }

        if (down) {
            const cpVect force = cpvmult(cpBodyGetRotation(body), -0.5 * propulsionStrength);
            cpBodyApplyForceAtWorldPoint(body, force, pos);
        }

        if (left) {
            const cpVect force = cpvmult(cpBodyGetRotation(body), 0.5 * propulsionStrength);
            cpBodyApplyForceAtWorldPoint(body, cpvrperp(force), pos);
        }

        if (right) {
            const cpVect force = cpvmult(cpBodyGetRotation(body), 0.5 * propulsionStrength);
            cpBodyApplyForceAtWorldPoint(body, cpvperp(force), pos);
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
            const Angle currentRotation = Angle::fromVector(cpBodyGetRotation(body).x,
                                                            cpBodyGetRotation(body).y);

            const auto selfPos  = cpBodyGetPosition(body);
            const auto mousePos = cpv(controls.mouseX, controls.mouseY);

            const Angle targetRotation = PointDirection(selfPos, mousePos);

            const Angle diff = currentRotation.shortestDistanceTo(targetRotation);

            cpVect rotForce;
            if (diff.asRadians() < 0.0) {
                rotForce = cpv(0.0, -200.0);
            }
            else {
                rotForce = cpv(0.0, +200.0);
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

        if (CountPeriodic(&_fireCounter, 7, controls.fire)) {
            auto selfVel = cpBodyGetVelocity(_body.get());
            auto selfPos = cpBodyGetPosition(_body.get());
            auto selfRot = cpBodyGetRotation(_body.get());

            PhysicsBullet::VisibleState vs;
            vs.x = selfPos.x;
            vs.y = selfPos.y;
            auto* bullet = QAO_PCreate<PhysicsBullet>(getRuntime(), ctx().getSyncObjReg(), SYNC_ID_NEW, vs);
            //bullet->initWithSpeed(this, std::atan2(controls.mouseY - selfPos.y, controls.mouseX - selfPos.x), 50.0);
            bullet->initWithSpeed(this, 0.0, 0.0);

            cpBody* bulletBody = bullet->getPhysicsBody();
            cpFloat bulletMass = cpBodyGetMass(bulletBody);
            cpFloat bulletSpeed = 100.0;

            // Match ship's speed:
            cpBodyApplyImpulseAtLocalPoint(bulletBody, cpvmult(selfVel, bulletMass), cpvzero);

            // Launch projectile:
            constexpr double aimingAngle = hg::math::DegToRad(20.0);

            const Angle selfAngle = Angle::fromVector(selfRot.x, selfRot.y);

            const Angle desiredLaunchAngle = PointDirection(selfPos, cpv(controls.mouseX, controls.mouseY));

            const Angle desiredAngleOffset = selfAngle.shortestDistanceTo(desiredLaunchAngle);

            const Angle actualAngleOffset = Angle::fromRadians(
                hg::math::Clamp(desiredAngleOffset.asRadians(), -aimingAngle, +aimingAngle)
            );

            const Angle actualLaunchAngle = selfAngle + actualAngleOffset;

            const auto actualLaunchAngleCp = cpv(actualLaunchAngle.asNormalizedVector().x, 
                                                 actualLaunchAngle.asNormalizedVector().y);

            //cpBodyApplyImpulseAtLocalPoint(bulletBody, cpvmult(cpvnormalize(selfRot), bulletSpeed), cpvzero);
            cpBodyApplyImpulseAtLocalPoint(bulletBody, cpvmult(cpvnormalize(actualLaunchAngleCp), bulletSpeed), 
                                           cpvzero);

            // Action and reaction baby
            //cpBodyApplyImpulseAtWorldPoint(_body.get(), cpvmult(cpvnormalize(selfRot), -5.0), selfPos);
            cpBodyApplyImpulseAtWorldPoint(_body.get(), cpvmult(cpvnormalize(actualLaunchAngleCp), -3.0), selfPos);
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
        self.health = static_cast<float>(_health);
        self.shield = static_cast<float>(_shield);

        auto physicsRot = cpBodyGetRotation(_body.get());
        self.angle = static_cast<float>(hg::math::Angle::fromVector(physicsRot.x, physicsRot.y).asRadians());
    }

    _lightHandle.setPosition(self.x, self.y);
}

void PhysicsPlayer::eventDraw1() {
    static const sf::Color COLORS[] = {sf::Color::White, sf::Color::Red, sf::Color::Green,
        sf::Color::Yellow, sf::Color::Blue};

    auto& canvas = ctx(MWindow).getCanvas();
    auto& self = _ssch.getCurrentState();

    auto glow = ctx(DSprite, SpriteId::WhiteGlow).getSubsprite(0);
    glow.setOrigin(glow.getLocalBounds().width / 2, glow.getLocalBounds().height / 2);
    glow.setPosition(self.x, self.y);
    glow.setColor(COLORS[self.playerIndex]);
    ctx(MWindow).getCanvas().draw(glow);

    auto sprite = ctx(DSprite, SpriteId::Ship).getSubsprite(0);

    sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
    sprite.setPosition(self.x, self.y);
    sprite.setRotation(hg::math::RadToDeg(self.angle) + 90.f);
    ctx(MWindow).getCanvas().draw(sprite);
}

void PhysicsPlayer::eventDrawGUI() {
    auto& self = _ssch.getCurrentState();

    if (self.playerIndex == ctx().getLocalPlayerIndex()) {
        const auto guiHeight = ctx(MWindow).getWindow().getSize().y;

        // Shield:
        {
            sf::Text text;
            text.setPosition(20.f, guiHeight - 52.f);
            text.setFont(hg::gr::BuiltInFonts::getFont(hg::gr::BuiltInFonts::TitilliumRegular));
            text.setFillColor(hg::gr::Color::LightBlue);
            text.setOutlineColor(hg::gr::Color::Black);
            text.setOutlineThickness(4.f);
            text.setCharacterSize(16);
            text.setString(MakeBarString(static_cast<int>(100.0 * self.shield / MAX_SHIELD)));

            ctx(MWindow).getCanvas().draw(text);
        }

        // Health:
        {
            sf::Text text;
            text.setPosition(20.f, guiHeight - 32.f);
            text.setFont(hg::gr::BuiltInFonts::getFont(hg::gr::BuiltInFonts::TitilliumRegular));
            text.setFillColor(hg::gr::Color::Red);
            text.setOutlineColor(hg::gr::Color::Black);
            text.setOutlineThickness(4.f);
            text.setCharacterSize(16);
            text.setString(MakeBarString(static_cast<int>(100.0 * self.health / MAX_HEALTH)));

            ctx(MWindow).getCanvas().draw(text);
        }
    }
}

void PhysicsPlayer::collisionPostSolve(Collideables::ICreature* terr, cpArbiter* arb) {
    //std::cout << "hit POSTSOLVE; damage = " << std::round(cpArbiterTotalKE(arb) / 1000.0) << '\n';
    _takeDamage(cpArbiterTotalKE(arb) / 2500.0);
}

void PhysicsPlayer::collisionPostSolve(Collideables::IProjectile* proj, cpArbiter* arb) {
    //std::cout << "hit POSTSOLVE; damage = " << std::round(cpArbiterTotalKE(arb) / 1000.0) << '\n';
    _takeDamage(25.0);
}

void PhysicsPlayer::collisionPostSolve(Collideables::ITerrain* terr, cpArbiter* arb) {
    //std::cout << "hit POSTSOLVE; damage = " << std::round(cpArbiterTotalKE(arb) / 1000.0) << '\n';
    _takeDamage(cpArbiterTotalKE(arb) / 2500.0);
}

void PhysicsPlayer::_takeDamage(double damage) {
    if (damage <= _shield) {
        _shield -= damage;
        return;
    }

    _health -= (damage - _shield);
    _shield = 0.0;
}