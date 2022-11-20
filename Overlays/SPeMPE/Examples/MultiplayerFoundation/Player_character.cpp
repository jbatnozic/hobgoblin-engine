
#include "Player_character.hpp"

PlayerCharacter::PlayerCharacter(QAO_RuntimeRef aRuntimeRef,
                                 spe::RegistryId aRegId,
                                 spe::SyncId aSyncId)
    : SyncObjSuper{aRuntimeRef, SPEMPE_TYPEID_SELF, PRIORITY_PLAYERAVATAR,
                   "PlayerCharacter", aRegId, aSyncId}
{
}

PlayerCharacter::~PlayerCharacter() {
    if (isMasterObject()) {
        doSyncDestroy();
    }
}

void PlayerCharacter::_eventUpdate(spe::IfMaster) {
    if (ctx().getGameState().isPaused) return;

    auto& self = _getCurrentState();
    assert(self.owningPlayerIndex >= 0);

    spe::InputSyncManagerWrapper wrapper{ccomp<MInput>()};

    const bool left = wrapper.getSignalValue<bool>(self.owningPlayerIndex, "left");
    const bool right = wrapper.getSignalValue<bool>(self.owningPlayerIndex, "right");

    self.x += (10.f * ((float)right - (float)left));

    const bool up = wrapper.getSignalValue<bool>(self.owningPlayerIndex, "up");
    const bool down = wrapper.getSignalValue<bool>(self.owningPlayerIndex, "down");

    self.y += (10.f * ((float)down - (float)up));

    wrapper.pollSimpleEvent(self.owningPlayerIndex, "jump",
                            [&]() {
                                self.y -= 16.f;
                            });
}

void PlayerCharacter::_eventDraw1() {
    #define NUM_COLORS 12
    static const hg::gr::Color COLORS[NUM_COLORS] = {
        hg::gr::Color::Red,
        hg::gr::Color::Green,
        hg::gr::Color::Yellow,
        hg::gr::Color::Blue,
        hg::gr::Color::Orange,
        hg::gr::Color::Purple,
        hg::gr::Color::Teal,
        hg::gr::Color::Brown,
        hg::gr::Color::Fuchsia,
        hg::gr::Color::Grey,
        hg::gr::Color::White,
        hg::gr::Color::Aqua,
    };

    const auto& self = _getCurrentState();

    sf::CircleShape circle{20.f};
    circle.setFillColor(COLORS[self.owningPlayerIndex % NUM_COLORS]);
    circle.setPosition({self.x, self.y});
    ccomp<MWindow>().getCanvas().draw(circle);
}

SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(PlayerCharacter, (CREATE, UPDATE, DESTROY));

void PlayerCharacter::_syncCreateImpl(spe::SyncDetails& aSyncDetails) const {
    SPEMPE_SYNC_CREATE_DEFAULT_IMPL(PlayerCharacter, aSyncDetails);
}

void PlayerCharacter::_syncUpdateImpl(spe::SyncDetails& aSyncDetails) const {
    SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(PlayerCharacter, aSyncDetails);
}

void PlayerCharacter::_syncDestroyImpl(spe::SyncDetails& aSyncDetails) const {
    SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(PlayerCharacter, aSyncDetails);
}