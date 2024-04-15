// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Utility/Math.hpp>

#include <cmath>
#include <iostream>

#include "GameContext/Game_context.hpp"
#include "GameObjects/Framework/Execution_priorities.hpp"
#include "GameObjects/Gameplay/Player.hpp"

namespace {
using hg::util::EuclideanDist;
} // namespace

RN_DEFINE_HANDLER(CreatePlayer, RN_ARGS(GOF_SyncId, syncId, Player::State&, state)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GameContext>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjReg = global.syncObjReg;
            QAO_PCreate<Player>(&runtime, syncObjReg, syncId, state.x, state.y, state.playerIndex);
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

RN_DEFINE_HANDLER(UpdatePlayer, RN_ARGS(GOF_SyncId, syncId, Player::State&, state)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GameContext>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjReg = global.syncObjReg;
            auto* player = static_cast<Player*>(syncObjReg.getMapping(syncId));

            const std::chrono::microseconds delay = client.getServer().getRemoteInfo().latency / 2LL;
            player->_ssch.putNewState(state, global.calcDelay(delay));
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

RN_DEFINE_HANDLER(DestroyPlayer, RN_ARGS(GOF_SyncId, syncId)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GameContext>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjReg = global.syncObjReg;
            auto* player = static_cast<Player*>(syncObjReg.getMapping(syncId));
            QAO_PDestroy(player);
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

void Player::syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    Compose_CreatePlayer(node, rec, getSyncId(), _ssch.getCurrentState());
}

void Player::syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    Compose_UpdatePlayer(node, rec, getSyncId(), _ssch.getCurrentState()); // TODO
}

void Player::syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    Compose_DestroyPlayer(node, rec, getSyncId()); // TODO
}

///////////////////////////////////////////////////////////////////////////////////////////////////

Player::Player(QAO_Runtime* runtime, GOF_SynchronizedObjectRegistry& syncObjReg, GOF_SyncId syncId,
               float x, float y, hg::PZInteger playerIndex)
    : GOF_SynchronizedObject{runtime, TYPEID_SELF, EXEPR_CREATURES, "Player", syncObjReg, syncId}
    , _ssch{ctx().syncBufferLength}
{
    _ssch.setDiscardIfOld(true);

    for (auto& state : _ssch) {
        state.playerIndex = playerIndex;
        state.x = x;
        state.y = y;
    }
    _doppelganger.x = x;
    _doppelganger.y = y;
    _doppelganger.playerIndex = playerIndex;
}

Player::~Player() {
    if (isMasterObject()) {
        syncDestroy();
    }
}

void Player::_eventBeginUpdate() {
    if (!ctx().isPrivileged()) {
        _ssch.advance();
    }
}

void Player::_eventUpdate() {
    if (ctx().isPrivileged()) {
        move(_ssch.getCurrentState());
    }
    else {
        _ssch.scheduleNewStates();
        _ssch.advanceDownTo(ctx().syncBufferLength * 2);
        
        // Try to predict doppelganger movement: [PREDICTIVE EXECUTION]
        //move(_doppelganger);

        _doppelganger = _ssch.getLatestState();

        //// Interpolate doppelganger state:
        //auto& self = _ssch.getCurrentState();
        //float dist = EuclideanDist<float>({_doppelganger.x, _doppelganger.y}, {self.x, self.y});  
        //if (dist <= 1.f) { // TODO Magic numbers
        //    _doppelganger = self;
        //}
        //else if (dist < std::max(20.f, 2.f * EuclideanDist<float>({0.f, 0.f},
        //                                                          {self.xspeed, self.yspeed}))) {

        //    /*double theta = std::atan2(self.y - _doppelganger.y, self.x - _doppelganger.x);
        //    _doppelganger.x += std::cos(theta) * dist * 0.1f;
        //    _doppelganger.y += std::sin(theta) * dist * 0.1f;*/
        //}
        //else {
        //    double theta = std::atan2(self.y - _doppelganger.y, self.x - _doppelganger.x);
        //    _doppelganger.x += std::cos(theta) * dist;// *; 0.25f;
        //    _doppelganger.y += std::sin(theta) * dist;// *0.25f;
        //}
    }
}

void Player::_eventDraw1() {
    static const sf::Color COLORS[] = {sf::Color::Blue, sf::Color::Red, sf::Color::Green, sf::Color::Yellow};

    if (ctx().isPrivileged()) {
        auto& self = _ssch.getCurrentState();
        sf::RectangleShape rect{{self.width, self.height}};

        rect.setFillColor(COLORS[self.playerIndex]);
        rect.setPosition(self.x, self.y);
        ctx().windowMgr.getCanvas().draw(rect);
    }
    else {
        auto& self = _ssch.getCurrentState();
        sf::RectangleShape rect{{self.width, self.height}};

        rect.setFillColor(COLORS[self.playerIndex]);
        rect.setPosition(_doppelganger.x, _doppelganger.y);
        ctx().windowMgr.getCanvas().draw(rect);

        if (kbi().keyPressed(KbKey::L)) return; // TODO Temp.

        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(2.0f);
        rect.setPosition(self.x, self.y);
        ctx().windowMgr.getCanvas().draw(rect);
    }
}

void Player::move(State& self) {
    PlayerControls controls = ctx().controlsMgr.getCurrentControlsForPlayer(self.playerIndex);

    if (self.y < static_cast<float>(800) - self.height) {
        self.yspeed += GRAVITY;
    }
    else {
        self.y = static_cast<float>(800) - self.height;
        self.yspeed = 0.f;
    }

    if (controls.up && !oldUp) {
        self.yspeed -= JUMP_POWER;
    }

    self.xspeed = (static_cast<float>(controls.right) - static_cast<float>(controls.left)) * MAX_SPEED;

    self.x += self.xspeed;
    self.y += self.yspeed;

    oldUp = controls.up;
}

// clang-format on
