#include "Character.hpp"

#include "Environment_manager.hpp"
#include "Player_controls.hpp"

#include <Hobgoblin/HGExcept.hpp>

#include "Config.hpp"
#include <Hobgoblin/Math.hpp>
#include <cmath>

namespace {
#define NUM_COLORS 12
static const hg::gr::Color COLORS[NUM_COLORS] = {
    hg::gr::COLOR_BLACK,
    hg::gr::COLOR_RED,
    hg::gr::COLOR_GREEN,
    hg::gr::COLOR_YELLOW,
    hg::gr::COLOR_BLUE,
    hg::gr::COLOR_ORANGE,
    hg::gr::COLOR_PURPLE,
    hg::gr::COLOR_TEAL,
    hg::gr::COLOR_BROWN,
    hg::gr::COLOR_FUCHSIA,
    hg::gr::COLOR_GREY,
    hg::gr::COLOR_WHITE
};
} // namespace

CharacterObject::CharacterObject(QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId)
    : SyncObjSuper{aRuntimeRef,
                   SPEMPE_TYPEID_SELF,
                   PRIORITY_PLAYERAVATAR,
                   "CharacterObject",
                   aRegId,
                   aSyncId}
    , _unibody{[this]() {
                   return _initColDelegate();
               },
               [this]() {
                   return hg::alvin::Body::createKinematic();
               },
               [this]() {
                   static constexpr cpFloat WIDTH  = 64.0;
                   static constexpr cpFloat HEIGHT = 64.0;
                   return hg::alvin::Shape::createBox(_unibody.body, WIDTH, HEIGHT);
               }} {
    if (isMasterObject()) {
        _getCurrentState().initMirror(); // To get autodiff optimization working
        _unibody.bindDelegate(*this);
        _unibody.addToSpace(ccomp<MEnvironment>().getSpace());
    } else {
        _renderer.emplace(ctx(), hg::gr::COLOR_RED);
        _renderer->setSize(1);
    }
}

CharacterObject::~CharacterObject() {
    if (isMasterObject()) {
        doSyncDestroy();
    }
}

void CharacterObject::init(int aOwningPlayerIndex, float aX, float aY) {
    HG_HARD_ASSERT(isMasterObject());

    auto& self             = _getCurrentState();
    self.x                 = aX;
    self.y                 = aY;
    self.owningPlayerIndex = aOwningPlayerIndex;

    cpBodySetPosition(_unibody, cpv(aX, aY));
}

void CharacterObject::_eventUpdate1(spe::IfDummy) {
    _renderer->update();
}
bool CharacterObject::getFling() const {
    return jump;
}

void CharacterObject::_eventUpdate1(spe::IfMaster) {
    if (ctx().getGameState().isPaused)
        return;

    auto& self = _getCurrentState();
    HG_HARD_ASSERT(self.owningPlayerIndex >= 0);

    auto& lobbyBackend = ccomp<MLobbyBackend>();
    if (const auto clientIndex = lobbyBackend.playerIdxToClientIdx(self.owningPlayerIndex);
        clientIndex != spe::CLIENT_INDEX_UNKNOWN) {

        spe::InputSyncManagerWrapper wrapper{ccomp<MInput>()};

        const auto left  = wrapper.getSignalValue<ControlDirectionType>(clientIndex, CTRL_ID_LEFT);
        const auto right = wrapper.getSignalValue<ControlDirectionType>(clientIndex, CTRL_ID_RIGHT);

        const auto up   = wrapper.getSignalValue<ControlDirectionType>(clientIndex, CTRL_ID_UP);
        const auto down = wrapper.getSignalValue<ControlDirectionType>(clientIndex, CTRL_ID_DOWN);

        float finalX = 0;
        float finalY = 0;

        float xInput = 0;
        float yInput = 0;


        if (grounded) {
            vSpeed = 0;
            yInput = (float)down - (float)up;
            xInput = (float)right - (float)left;
        } else {
            vSpeed -= gravity;
        }

        if (xInput != 0 || yInput != 0) {
            auto angle  = hg::math::AngleF::fromVector({xInput, yInput}) * -1.f;
            auto normal = angle.asNormalizedVector();

            finalX = character_speed * normal.x;
            finalY = character_speed * normal.y;
            wrapper.pollSimpleEvent(clientIndex, CTRL_ID_JUMP, [&]() {
                if (grounded && currentFlingCooldown <= 0) {
                    jumpDirection     = normal;
                    jump     = true;
                    grounded = false;
                    currentFlingCooldown = fling_timer;
                }
            });
        }
        if (!grounded) {
            finalX += jumpDirection.x * fling_speed;
            finalY += jumpDirection.y * fling_speed;
        } else {
            jumpDirection = {0.f, 0.f};
        }

        if (currentFlingCooldown > 0) {
            currentFlingCooldown--;
        }
        if (currentGroundTimer > 0) {
            currentGroundTimer--;
        }


        finalY -= vSpeed;



        bool  touchingTerrain = false;
        auto& space           = ccomp<MEnvironment>().getSpace();
        space.runShapeQuery(_unibody.shape,
                            cpShapeFilterNew(0, CP_ALL_CATEGORIES, CAT_TERRAIN),
                            [&, this](const hg::alvin::ShapeQueryInfo&) {
                                touchingTerrain = true;
                            });
        if (!touchingTerrain) {
            
            if (currentGroundTimer <= 0 && grounded != false) {
                
                currentGroundTimer = fall_timer;
                
            }
            grounded = false;
        } else {
            if (currentGroundTimer <= 0 && currentFlingCooldown <= 0) {
                grounded = true;
                jump     = false;
            }
        }

        if (self.y > y_floor) {
            self.y   = y_floor;
            grounded = true;
            jump     = false;
        }
        // HG_LOG_INFO(LOG_ID, "SADKASD KASMD {}, {}", currentGroundTimer, currentFlingCooldown);
        self.x += finalX;
        self.y += finalY;
        cpBodySetPosition(_unibody, cpv(self.x, self.y));
    }
}

void CharacterObject::_eventPostUpdate(spe::IfMaster) {
    _getCurrentState().commit();
}

void CharacterObject::_eventDraw1() {
    if (this->isDeactivated())
        return;

    const auto& self = _getCurrentState();

    _renderer->setColor(COLORS[self.owningPlayerIndex % NUM_COLORS]);
    _renderer->setPosition({self.x, self.y});
    _renderer->draw(ccomp<MWindow>().getCanvas());

    auto& lobbyBackend = ccomp<MLobbyBackend>();
    if (lobbyBackend.getLocalPlayerIndex() == self.owningPlayerIndex) {
        auto& camera = ccomp<MWindow>().getViewController().getView(0);
        camera.setCenter(self.x, self.y);
    }
}

void CharacterObject::_eventDraw2() {
    if (this->isDeactivated())
        return;

    const auto& self = _getCurrentState();

    auto& lobbyBackend = ccomp<MLobbyBackend>();
    if (self.owningPlayerIndex > 0) {
        const auto&  name = lobbyBackend.getLockedInPlayerInfo(self.owningPlayerIndex).name;
        hg::gr::Text text{hg::gr::BuiltInFonts::getFont(hg::gr::BuiltInFonts::TITILLIUM_REGULAR),
                          name,
                          40};
        text.setFillColor(hg::gr::COLOR_WHITE);
        text.setOutlineColor(hg::gr::COLOR_BLACK);
        text.setOutlineThickness(2.f);
        const auto& localBounds = text.getLocalBounds();
        text.setOrigin({localBounds.w / 2.f, localBounds.h / 2.f});
        text.setPosition({self.x, self.y - 120.f});
        ccomp<MWindow>().getCanvas().draw(text);
    }
}

hg::alvin::CollisionDelegate CharacterObject::_initColDelegate() {
    auto builder = hg::alvin::CollisionDelegateBuilder{};

    /* builder.addInteraction<LootInterface>(
        hg::alvin::COLLISION_POST_SOLVE,
        [this](LootInterface& aLoot, const hg::alvin::CollisionData& aCollisionData) {
            // DO INTERACTION
        });
    builder.addInteraction<TerrainInterface>(
        hg::alvin::COLLISION_SEPARATE,
        [this](TerrainInterface& aTerrain, const hg::alvin::CollisionData& aCollisionData) {
            // DO INTERACTION
            HG_LOG_INFO(LOG_ID, "SADKASD KASMD ");
            grounded = false;
            currentGroundTimer = fall_timer;

        });
        builder.addInteraction<TerrainInterface>(
        hg::alvin::COLLISION_PRE_SOLVE,
        [this](TerrainInterface& aTerrain, const hg::alvin::CollisionData& aCollisionData) {
            // DO INTERACTION
            if (currentGroundTimer <= 0 && currentFlingCooldown <= 0) {
                grounded = true;
            }
            return hg::alvin::Decision::ACCEPT_COLLISION;
        });
        
        */
    builder.addInteraction<CharacterInterface>(
        hg::alvin::COLLISION_PRE_SOLVE,
        [this](CharacterInterface& aCharacter, const hg::alvin::CollisionData& aCollisionData) {
            // DO INTERACTION
            
            if (aCharacter.getFling()) {
                grounded = false;
                currentGroundTimer = fall_timer;
            }
            return hg::alvin::Decision::ACCEPT_COLLISION;
        });


    
    return builder.finalize();
}

SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(CharacterObject, (CREATE, UPDATE, DESTROY));

void CharacterObject::_syncCreateImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_CREATE_DEFAULT_IMPL(CharacterObject, aSyncCtrl);
}

void CharacterObject::_syncUpdateImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(CharacterObject, aSyncCtrl);
}

void CharacterObject::_syncDestroyImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(CharacterObject, aSyncCtrl);
}
