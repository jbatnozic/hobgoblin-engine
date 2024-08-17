#pragma once

#include "Character_renderer.hpp"
#include "Collisions.hpp"
#include "Engine.hpp"

#include <Hobgoblin/Alvin.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>

#include <cstdint>
#include <memory>
#include <optional>

SPEMPE_DEFINE_AUTODIFF_STATE(CharacterObject_VisibleState,
                             SPEMPE_MEMBER(float, x, 0.f),
                             SPEMPE_MEMBER(float, y, 0.f),
                             SPEMPE_MEMBER(std::int32_t,
                                           owningPlayerIndex,
                                           spe::PLAYER_INDEX_UNKNOWN)){};

/**
 * Implementation of a synchronized object with autodiff state optimization enabled.
 * (members that didn't change are automatically detected and are not sent to clients).
 */
class CharacterObject
    : public spe::SynchronizedObject<CharacterObject_VisibleState>
    , public CharacterInterface {
public:
    CharacterObject(QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId);

    ~CharacterObject() override;

    void init(int aOwningPlayerIndex, float aX, float aY);
    bool getFling() const override;

private:
    bool grounded  = false;
    bool  jump           = false;
    sf::Vector2f jumpDirection          = {0.f, 0.f};
    float vSpeed   = 0;
    float currentFlingCooldown = 0;
    float currentGroundTimer = 0;

    hg::alvin::Unibody _unibody;

    std::optional<CharacterRenderer> _renderer;

    hg::alvin::CollisionDelegate _initColDelegate();

    void _eventUpdate1(spe::IfDummy) override;
    void _eventUpdate1(spe::IfMaster) override;
    void _eventPostUpdate(spe::IfMaster) override;
    void _eventDraw1() override;

    void _syncCreateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncUpdateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncDestroyImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
};
