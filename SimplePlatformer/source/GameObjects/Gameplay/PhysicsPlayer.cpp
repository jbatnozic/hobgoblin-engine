
#include "GameContext/Game_context.hpp"
#include "GameObjects/Framework/Execution_priorities.hpp"
#include "GameObjects/Gameplay/PhysicsPlayer.hpp"

RN_DEFINE_HANDLER(CreatePhysicsPlayer, RN_ARGS(SyncId, syncId, PhysicsPlayer::ViState&, state)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GameContext>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjMapper = global.syncObjMgr;

            QAO_PCreate<PhysicsPlayer>(&runtime, syncObjMapper, syncId, state);
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

RN_DEFINE_HANDLER(UpdatePhysicsPlayer, RN_ARGS(SyncId, syncId, PhysicsPlayer::ViState&, state)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GameContext>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjMapper = global.syncObjMgr;
            auto* player = static_cast<PhysicsPlayer*>(syncObjMapper.getMapping(syncId));

            const std::chrono::microseconds delay = client.getServer().getRemoteInfo().latency / 2LL;
            player->_ssch.putNewState(state, global.calcDelay(delay));
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

RN_DEFINE_HANDLER(DestroyPhysicsPlayer, RN_ARGS(SyncId, syncId)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GameContext>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjMapper = global.syncObjMgr;
            auto* player = static_cast<PhysicsPlayer*>(syncObjMapper.getMapping(syncId));

            QAO_PDestroy(player);
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

PhysicsPlayer::PhysicsPlayer(QAO_RuntimeRef rtRef, SynchronizedObjectManager& syncObjMgr, SyncId syncId,
                             const ViState& initialState)
    : GOF_SynchronizedObject{rtRef, TYPEID_SELF, EXEPR_CREATURES, "PhysicsPlayer", syncObjMgr, syncId}
    , _ssch{ctx().syncBufferLength}
{
    for (auto& state : _ssch) {
        state = initialState;
        if (isMasterObject()) {
            state.hidden = false;
        }
    }

    if (isMasterObject()) {
        auto* space = ctx().getPhysicsSpace();

        cpFloat radius = 12.0;
        cpFloat mass = 1.0;
        cpFloat moment = cpMomentForCircle(mass, 0.0, radius, cpv(0, 0));

        _body = hg::cpBodyUPtr{cpSpaceAddBody(space, cpBodyNew(mass, moment))};
        _shape = hg::cpShapeUPtr{cpSpaceAddShape(space, cpCircleShapeNew(_body.get(), radius, cpv(0.0, 0.0)))};
        cpBodySetPosition(_body.get(), cpv(initialState.x, initialState.y));
        cpShapeSetElasticity(_shape.get(), 0.5);
    }

    //_lightHandle = ctx().envMgr.addLight(initialState.x, initialState.y, hg::gr::Color::MediumBioletRed, 8.f);
    _lightHandle = ctx().envMgr.addLight(initialState.x, initialState.y, hg::gr::Color::White, 8.f);
}

PhysicsPlayer::~PhysicsPlayer() {
    if (isMasterObject()) {
        syncDestroy();
    }
}

void PhysicsPlayer::syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    Compose_CreatePhysicsPlayer(node, rec, getSyncId(), _ssch.getCurrentState());
}

void PhysicsPlayer::syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    Compose_UpdatePhysicsPlayer(node, rec, getSyncId(), _ssch.getCurrentState());
}

void PhysicsPlayer::syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    Compose_DestroyPhysicsPlayer(node, rec, getSyncId());
}

void PhysicsPlayer::eventUpdate() {
    if (isMasterObject()) {
        auto& self = _ssch.getCurrentState();
        PlayerControls controls = ctx().controlsMgr.getCurrentControlsForPlayer(self.playerIndex);

        cpFloat left = controls.left;
        cpFloat right = controls.right;
        cpFloat up = controls.up;
        cpFloat down = controls.down;

        const cpVect pos = cpBodyGetPosition(_body.get());
        const cpVect force = cpv((right - left) * 1000.0, (down - up) * 1000.0);
        cpBodyApplyForceAtWorldPoint(_body.get(), force, pos);
    }
    else {
        _ssch.scheduleNewStates();
        _ssch.advanceDownTo(ctx().syncBufferLength * 2);
    }

    {
        auto& view = ctx().windowMgr.getView();
        auto& self = _ssch.getCurrentState();

        if (self.playerIndex == ctx().playerIndex) {
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
    if (isMasterObject()) {
        auto physicsPos = cpBodyGetPosition(_body.get());
        self.x = static_cast<float>(physicsPos.x);
        self.y = static_cast<float>(physicsPos.y);
    }

    _lightHandle.setPosition(self.x, self.y);
}

void PhysicsPlayer::eventDraw1() {
    static const sf::Color COLORS[] = {sf::Color::Red, sf::Color::Yellow, sf::Color::Blue, sf::Color::Green};

    auto& canvas = ctx().windowMgr.getCanvas();
    auto& self = _ssch.getCurrentState();

    sf::CircleShape circ{12.0};
    circ.setOrigin(circ.getRadius(), circ.getRadius());
    circ.setFillColor(COLORS[self.playerIndex]);
    circ.setOutlineColor(sf::Color::Black);
    circ.setOutlineThickness(2.f);
    circ.setPosition({self.x, self.y});
    canvas.draw(circ);
}