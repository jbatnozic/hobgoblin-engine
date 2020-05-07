
#include <cmath>

#include "GameContext/Game_context.hpp"
#include "GameObjects/Framework/Execution_priorities.hpp"
#include "GameObjects/Gameplay/PhysicsBullet.hpp"
#include "GameObjects/Gameplay/PhysicsPlayer.hpp"

template <class T>
void CannonicalCreateImpl(RN_Node& node, SyncId syncId, typename T::ViState& state) {
    node.visit(
        [&](NetworkingManager::ClientType& client) {
            auto& ctx = *client.getUserData<GameContext>();
            auto& runtime = ctx.qaoRuntime;
            auto& syncObjMapper = ctx.syncObjMgr;
            QAO_PCreate<T>(&runtime, syncObjMapper, syncId, state);
        },
        [](NetworkingManager::ServerType& server) {
            // TODO ERROR
        }
    );
}

template <class T>
void CannonicalUpdateImpl(RN_Node& node, SyncId syncId, typename T::ViState& state) {
    node.visit(
        [&](NetworkingManager::ClientType& client) {
            auto& ctx = *client.getUserData<GameContext>();
            auto& runtime = ctx.qaoRuntime;
            auto& syncObjMapper = ctx.syncObjMgr;
            auto& object = *static_cast<T*>(syncObjMapper.getMapping(syncId));

            const std::chrono::microseconds delay = client.getServer().getRemoteInfo().latency / 2LL;
            object.applyUpdate(state, ctx.calcDelay(delay));
        },
        [](NetworkingManager::ServerType& server) {
            // TODO ERROR
        }
    );
}

template <class T>
void CannonicalDestroyImpl(RN_Node& node, SyncId syncId) {
    node.visit(
        [&](NetworkingManager::ClientType& client) {
            auto& ctx = *client.getUserData<GameContext>();
            auto& runtime = ctx.qaoRuntime;
            auto& syncObjMapper = ctx.syncObjMgr;
            auto* object = static_cast<T*>(syncObjMapper.getMapping(syncId));

            QAO_PDestroy(object);
        },
        [](NetworkingManager::ServerType& server) {
            // TODO ERROR
        }
    );
}

#define GENERATE_CANNONICAL_HANDLERS(_class_name_) \
    RN_DEFINE_HANDLER(Create##_class_name_, RN_ARGS(SyncId, syncId, _class_name_::ViState&, state)) { \
        CannonicalCreateImpl<_class_name_>(RN_NODE_IN_HANDLER(), syncId, state); \
    } \
    RN_DEFINE_HANDLER(Update##_class_name_, RN_ARGS(SyncId, syncId, _class_name_::ViState&, state)) { \
        CannonicalUpdateImpl<_class_name_>(RN_NODE_IN_HANDLER(), syncId, state); \
    } \
    RN_DEFINE_HANDLER(Destroy##_class_name_, RN_ARGS(SyncId, syncId)) { \
        CannonicalDestroyImpl<_class_name_>(RN_NODE_IN_HANDLER(), syncId); \
    }

GENERATE_CANNONICAL_HANDLERS(PhysicsPlayer);

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

    if (ctx().isPrivileged()) {
        auto* space = ctx().getPhysicsSpace();

        cpFloat radius = 8.0;
        cpFloat mass = 1.0;
        cpFloat moment = cpMomentForCircle(mass, 0.0, radius, cpv(0, 0));

        _body = hg::cpBodyUPtr{cpSpaceAddBody(space, cpBodyNew(mass, moment))};
        _shape = hg::cpShapeUPtr{cpSpaceAddShape(space, cpCircleShapeNew(_body.get(), radius, cpv(0.0, 0.0)))};
        cpBodySetPosition(_body.get(), cpv(initialState.x, initialState.y));
        cpShapeSetElasticity(_shape.get(), 0.5);
    }

    //_lightHandle = ctx().envMgr.addLight(initialState.x, initialState.y, hg::gr::Color::MediumBioletRed, 8.f);
    _lightHandle = ctx().envMgr.addLight(initialState.x, initialState.y, hg::gr::Color::AntiqueWhite, 8.f);
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
    if (ctx().isPrivileged()) {
        auto& self = _ssch.getCurrentState();
        PlayerControls controls = ctx().controlsMgr.getCurrentControlsForPlayer(self.playerIndex);

        cpFloat left = controls.left;
        cpFloat right = controls.right;
        cpFloat up = controls.up;
        cpFloat down = controls.down;

        const cpVect pos = cpBodyGetPosition(_body.get());
        const cpVect force = cpv((right - left) * 1000.0, (down - up) * 1000.0);
        cpBodyApplyForceAtWorldPoint(_body.get(), force, pos);

        if (kbi().keyPressed(KbKey::Space, KbMode::Repeat)) {
            std::cout << "Creating bullet\n";       
            auto mousePos = ctx().windowMgr.getMousePos();
            auto selfPos = cpBodyGetPosition(_body.get());

            PhysicsBullet::ViState vs;
            vs.x = selfPos.x;
            vs.y = selfPos.y;
            auto* bullet = QAO_PCreate<PhysicsBullet>(getRuntime(), ctx().syncObjMgr, SYNC_ID_CREATE_MASTER, vs);
            bullet->initWithSpeed(std::atan2(mousePos.y - selfPos.y, mousePos.x - selfPos.x), 50.0);
        }
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
    if (ctx().isPrivileged()) {
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