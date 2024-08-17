#include "Character_renderer.hpp"

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Math.hpp>

#include "Resource_manager_interface.hpp"
#include "Sprite_manifest.hpp"

#include <array>
#include <cmath>

CharacterRenderer::CharacterRenderer(spe::GameContext& aCtx, hg::gr::Color aColor)
    : _ctx{aCtx}
    , _color{aColor} {
    const auto& sprLoader = _ctx.getComponent<MResource>().getSpriteLoader();

    _legs       = sprLoader.getMultiBlueprint(SPR_LEGS).multispr();
    _openHand   = sprLoader.getMultiBlueprint(SPR_HAND_OPEN).multispr();
    _closedHand = sprLoader.getMultiBlueprint(SPR_HAND_CLOSED).multispr();
}

void CharacterRenderer::setSize(hg::PZInteger aSize) {
    const auto& sprLoader = _ctx.getComponent<MResource>().getSpriteLoader();
    switch (aSize) {
    case 1:
        {
            _body    = sprLoader.getMultiBlueprint(SPR_BODY_SIZE_1).multispr();
            _biceps  = sprLoader.getMultiBlueprint(SPR_BICEPS_SIZE_1).multispr();
            _forearm = sprLoader.getMultiBlueprint(SPR_FOREARM_SIZE_1).multispr();
        }
        break;

    case 2:
        {
            _body    = sprLoader.getMultiBlueprint(SPR_BODY_SIZE_2).multispr();
            _biceps  = sprLoader.getMultiBlueprint(SPR_BICEPS_SIZE_2).multispr();
            _forearm = sprLoader.getMultiBlueprint(SPR_FOREARM_SIZE_2).multispr();
        }
        break;

    case 3:
        {
            _body    = sprLoader.getMultiBlueprint(SPR_BODY_SIZE_3).multispr();
            _biceps  = sprLoader.getMultiBlueprint(SPR_BICEPS_SIZE_3).multispr();
            _forearm = sprLoader.getMultiBlueprint(SPR_FOREARM_SIZE_3).multispr();
        }
        break;

    case 4:
        {
            _body    = sprLoader.getMultiBlueprint(SPR_BODY_SIZE_4).multispr();
            _biceps  = sprLoader.getMultiBlueprint(SPR_BICEPS_SIZE_4).multispr();
            _forearm = sprLoader.getMultiBlueprint(SPR_FOREARM_SIZE_4).multispr();
        }
        break;

    case 5:
        {
            _body    = sprLoader.getMultiBlueprint(SPR_BODY_SIZE_5).multispr();
            _biceps  = sprLoader.getMultiBlueprint(SPR_BICEPS_SIZE_5).multispr();
            _forearm = sprLoader.getMultiBlueprint(SPR_FOREARM_SIZE_5).multispr();
        }
        break;

    default:
        HG_UNREACHABLE("Invalid body size ({})", aSize);
    }
}

void CharacterRenderer::setPosition(hg::math::Vector2f aPosition) {
    _position = aPosition;
}

void CharacterRenderer::setMode(Mode aMode) {
    if (_mode != aMode) {
        _mode = aMode;
        _frameCounter = 0.f;
    }
}

namespace {
constexpr double SHOULDER_Y_OFFSET = -1.5f * 12.f;
constexpr double SHOULDER_X_OFFSET = +5.f * 12.f;

constexpr double LEGS_Y_OFFSET = 82.5;

constexpr double BICEPS_LENGTH  = 7.f * 12.f;
constexpr double FOREARM_LENGTH = 6.f * 12.f;

// Climb
// clang-format off
const std::array<hg::math::Vector2d, 8> CLIMB_HAND_OFFSETS = {{
     {7.0 * 12.0, -6.0 * 12.0},
     {7.0 * 12.0, -7.0 * 12.0},
     {7.0 * 12.0, -8.0 * 12.0},
     {7.0 * 12.0, -7.0 * 12.0},
     {7.0 * 12.0, -6.0 * 12.0},
     {7.0 * 12.0, -5.0 * 12.0},
     {7.0 * 12.0, -4.0 * 12.0},
     {7.0 * 12.0, -5.0 * 12.0},
}};
// clang-format on

struct ArmDrawParams {
    hg::math::Vector2f bicepsOffset;
    hg::math::AngleF   bicepsAngle;

    hg::math::Vector2f forearmOffset;
    hg::math::AngleF   forearmAngle;

    hg::math::Vector2f handOffset;
    hg::math::AngleF   handAngle;
};

ArmDrawParams GetArmDrawParams_Climb(hg::PZInteger aAnimationFrame) {
    using namespace hg::math;

    ArmDrawParams result;

    const double C = EuclideanDist(0.0,
                                   0.0,
                                   CLIMB_HAND_OFFSETS[aAnimationFrame].x,
                                   CLIMB_HAND_OFFSETS[aAnimationFrame].y);

    const double cosAf = (Sqr(BICEPS_LENGTH) + Sqr(C) - Sqr(FOREARM_LENGTH)) / (2.0 * BICEPS_LENGTH * C);

    const auto Af = AngleD::fromRad(std::acos(cosAf));

    const auto P = AngleD::fromVector(
        {CLIMB_HAND_OFFSETS[aAnimationFrame].x, CLIMB_HAND_OFFSETS[aAnimationFrame].y});

    const auto Pp = P - Af;

    const double elbowOffsetX = +Pp.cos() * BICEPS_LENGTH;
    const double elbowOffsetY = -Pp.sin() * BICEPS_LENGTH;

    const double bicepsCenterX = elbowOffsetX / 2; // from shoulder
    const double bicepsCenterY = elbowOffsetY / 2; // from shoulder

    result.bicepsOffset = {static_cast<float>(SHOULDER_X_OFFSET + bicepsCenterX),
                           static_cast<float>(SHOULDER_Y_OFFSET + bicepsCenterY)};
    result.bicepsAngle  = PointDirection<float>(0.f,
                                               0.f,
                                               static_cast<float>(elbowOffsetX),
                                               static_cast<float>(elbowOffsetY));

    const double forearmCenterX =
        (CLIMB_HAND_OFFSETS[aAnimationFrame].x + elbowOffsetX) / 2.0; // from shoulder
    const double forearmCenterY =
        (CLIMB_HAND_OFFSETS[aAnimationFrame].y + elbowOffsetY) / 2.0; // from shoulder

    result.forearmOffset = {static_cast<float>(SHOULDER_X_OFFSET + forearmCenterX),
                            static_cast<float>(SHOULDER_Y_OFFSET + forearmCenterY)};
    result.forearmAngle =
        PointDirection<float>(static_cast<float>(elbowOffsetX),
                              static_cast<float>(elbowOffsetY),
                              static_cast<float>(CLIMB_HAND_OFFSETS[aAnimationFrame].x),
                              static_cast<float>(CLIMB_HAND_OFFSETS[aAnimationFrame].y)) -
        AngleF::fromDegrees(90.f);

    result.handOffset = {static_cast<float>(SHOULDER_X_OFFSET + CLIMB_HAND_OFFSETS[aAnimationFrame].x),
                         static_cast<float>(SHOULDER_Y_OFFSET + CLIMB_HAND_OFFSETS[aAnimationFrame].y)};
    result.handAngle  = result.forearmAngle;

    return result;
}

} // namespace

void CharacterRenderer::update() {
    _frameCounter += 0.25f;
}

void CharacterRenderer::draw(hg::gr::Canvas& aCanvas) {
    switch (_mode) {
    case Mode::CRAWL:
        _drawClimb(aCanvas);
        break;

    case Mode::FLING:
    case Mode::HUNKER:
        break;
        
    default:
        HG_UNREACHABLE("Invalid draw mode.");
    }
}

void CharacterRenderer::_drawClimb(hg::gr::Canvas& aCanvas) {
    using namespace hg::math;

    const float scale = 0.5;

    _body.setPosition(_position);
    _body.setScale({scale, scale});
    aCanvas.draw(_body);

    unsigned animationFrame = static_cast<unsigned>(_frameCounter) % 8;

    // Right arm
    {
        const auto params = GetArmDrawParams_Climb(animationFrame);

        _biceps.setPosition({_position.x + scale * params.bicepsOffset.x, _position.y + scale * params.bicepsOffset.y});
        _biceps.setRotation(params.bicepsAngle);
        _biceps.setScale({scale * 1.f, scale * 1.f});

        _forearm.setPosition(
            {_position.x + scale * params.forearmOffset.x, _position.y + scale * params.forearmOffset.y});
        _forearm.setRotation(params.forearmAngle);
        _forearm.setScale({scale * 1.f, scale * 1.f});

        hg::gr::Multisprite* handSprite =
            (animationFrame >= 3 && animationFrame <= 6) ? &_closedHand : &_openHand;
        handSprite->setPosition({_position.x + scale * params.handOffset.x, _position.y + scale * params.handOffset.y});
        handSprite->setRotation(params.handAngle);
        handSprite->setScale({scale * 1.f, scale * 1.f});

        aCanvas.draw(_forearm);
        aCanvas.draw(*handSprite);
        aCanvas.draw(_biceps);
    }

    // Left arm
    {
        const auto params = GetArmDrawParams_Climb((animationFrame + 4) % 8);

        _biceps.setPosition({_position.x - scale * params.bicepsOffset.x, _position.y + scale * params.bicepsOffset.y});
        _biceps.setRotation(-params.bicepsAngle);
        _biceps.setScale({scale * -1.f, scale * 1.f});

        _forearm.setPosition(
            {_position.x - scale * params.forearmOffset.x, _position.y + scale * params.forearmOffset.y});
        _forearm.setRotation(params.forearmAngle + AngleF::fromDegrees(180.0));
        _forearm.setScale({scale * 1.f, scale * -1.f});

        hg::gr::Multisprite* handSprite =
            (((animationFrame + 4) % 8) >= 3 && ((animationFrame + 4) % 8) <= 6) ? &_closedHand
                                                                                 : &_openHand;
        handSprite->setPosition({_position.x - scale * params.handOffset.x, _position.y + scale * params.handOffset.y});
        handSprite->setRotation(params.handAngle);
        handSprite->setScale({scale * -1.f, scale * 1.f});

        aCanvas.draw(_forearm);
        aCanvas.draw(*handSprite);
        aCanvas.draw(_biceps);
    }

    // Legs
    {
        _legs.setPosition({_position.x, _position.y + scale * static_cast<float>(LEGS_Y_OFFSET)});
        _legs.setRotation(AngleF::zero());
        _legs.setScale({scale, scale});
        aCanvas.draw(_legs);
    }
}
