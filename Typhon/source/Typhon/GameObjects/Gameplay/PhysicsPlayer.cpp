
#include <cmath>

#include "../Control/Controls_manager.hpp"
#include "PhysicsBullet.hpp"
#include "PhysicsPlayer.hpp"

SPEMPE_GENERATE_CANNONICAL_HANDLERS(PhysicsPlayer);

SPEMPE_GENERATE_CANNONICAL_SYNC_IMPLEMENTATIONS(PhysicsPlayer);

PhysicsPlayer::PhysicsPlayer(QAO_RuntimeRef rtRef, SynchronizedObjectRegistry& syncObjReg, SyncId syncId,
                             const VisibleState& initialState)
    : SynchronizedObject{rtRef, SPEMPE_TYPEID_SELF, EXEPR_CREATURES, "PhysicsPlayer", syncObjReg, syncId}
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
        Collideables::initCreature(_shape.get(), *this);
    }

    //_lightHandle = ctx().envMgr.addLight(initialState.x, initialState.y, hg::gr::Color::MediumBioletRed, 8.f);
    //_lightHandle = ctx().envMgr.addLight(initialState.x, initialState.y, hg::gr::Color::AntiqueWhite, 8.f);
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

        cpFloat left = controls.left;
        cpFloat right = controls.right;
        cpFloat up = controls.up;
        cpFloat down = controls.down;

        const cpVect pos = cpBodyGetPosition(_body.get());
        const cpVect force = cpv((right - left) * 1000.0, (down - up) * 1000.0);
        cpBodyApplyForceAtWorldPoint(_body.get(), force, pos);

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
        _ssch.advanceDownTo(ctx().syncBufferLength * 2);
    }

    {
        auto& view = ctx(MWindow).getView();
        auto& self = _ssch.getCurrentState();

        if (self.playerIndex == ctx().getLocalPlayerIndex()) {
            auto dist = hg::util::EuclideanDist<float>({self.x, self.y}, view.getCenter());
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
}