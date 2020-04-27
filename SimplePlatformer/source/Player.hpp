#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/State_scheduler.hpp>

#include "Game_object_framework.hpp"

class Player : public GOF_SynchronizedObject {
public:
    static constexpr auto SERIALIZABLE_TAG = "Player";

    static constexpr float MAX_SPEED = 5.f;
    static constexpr float GRAVITY = 1.f;
    static constexpr float JUMP_POWER = 16.f;

    Player(QAO_Runtime* runtime, SynchronizedObjectManager& syncObjMgr, SyncId syncId,
           float x, float y, hg::PZInteger playerIndex);

    ~Player();

    void serialize(hg::util::Packet& packet) const {
    }

    static void deserialize(hg::util::Packet& packet, hg::util::AnyPtr context, int /* contextTag */) {
    }

    virtual void syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;
    virtual void syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;
    virtual void syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;

protected:
    void eventPreUpdate() override;
    void eventUpdate() override;
    void eventDraw1() override;

public: // State object must be public
    struct State {
        hg::PZInteger playerIndex;
        float x, y;
        float xspeed = 0.f, yspeed = 0.f;
        float width = 48.f, height = 64.f;
        HG_ENABLE_AUTOPACK(State, playerIndex, x, y, xspeed, yspeed, width, height);

        // State scheduling:
        void debounce(const std::deque<State>& q, std::size_t currentPos) {}
        void unstuck(const std::deque<State>& history) {}
        void integrate(const State& other) { *this = other; }
    };

private:
    hg::util::StateScheduler<State> _ssch;
    State _doppelganger;

    bool oldUp = false;
    void move(State& self);

    friend RN_HANDLER_SIGNATURE(UpdatePlayer, RN_ARGS(SyncId, syncId, State&, state));
};

#endif // !PLAYER_HPP
