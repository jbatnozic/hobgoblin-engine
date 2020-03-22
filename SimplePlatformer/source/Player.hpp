#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include "Game_object_framework.hpp"

class Player : public GOF_SynchronizedObject {
public:
    static constexpr auto SERIALIZABLE_TAG = "Player";

    static constexpr float MAX_SPEED = 5.f;
    static constexpr float GRAVITY = 1.f;
    static constexpr float JUMP_POWER = 16.f;

    Player(QAO_Runtime* runtime, SynchronizedObjectManager& syncObjMgr, 
           float x, float y, hg::PZInteger playerIndex);

    Player(QAO_Runtime* runtime, SynchronizedObjectManager& syncObjMgr, SyncId masterSyncId,
           float x, float y, hg::PZInteger playerIndex);

    void serialize(hg::util::Packet& packet) const {
    }

    static void deserialize(hg::util::Packet& packet, hg::util::AnyPtr context, int /* contextTag */) {
    }

    virtual void syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;
    virtual void syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;
    virtual void syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;

    friend RN_HANDLER_SIGNATURE(UpdatePlayer, RN_ARGS(SyncId, syncId, float, x, float, y));

protected:
    void eventUpdate() override;
    void eventDraw1() override;

private:
    hg::PZInteger playerIndex;
    float x, y;
    float xspeed = 0.f, yspeed = 0.f;
    float width = 48.f, height = 64.f;
    bool oldUp = false;
};

#endif // !PLAYER_HPP
