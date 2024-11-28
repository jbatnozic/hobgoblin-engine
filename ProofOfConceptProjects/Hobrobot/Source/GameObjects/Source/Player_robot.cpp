
#include <HobRobot/GameObjects/Player_robot.hpp>

#include <HobRobot/Common/Player_controls.hpp>
#include <HobRobot/Context/Managers/Environment_manager_interface.hpp>

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Math.hpp>

#include "Single_shape_body.hpp"
#include "Limb.hpp"

#include <iostream>

namespace hobrobot {

using namespace hg::cp;

namespace {
constexpr auto LOG_ID = "Hobrobot";

void DrawBodyPart(hg::gr::Canvas& aCanvas,
                  const BodyPart& aBodyPart,
                  const hg::gr::Color& aFillColor,
                  const hg::gr::Color& aOutlineColor,
                  float aOutlineThickness) {
    hg::gr::RectangleShape rect;
    rect.setSize({aBodyPart.width, aBodyPart.height});
    rect.setOrigin({aBodyPart.width * 0.5f, aBodyPart.height * 0.5f});
    rect.setPosition({aBodyPart.x, aBodyPart.y});
    rect.setFillColor(aFillColor);
    rect.setOutlineThickness(aOutlineThickness);
    rect.setOutlineColor(aOutlineColor);
    rect.setRotation(hg::math::AngleF::fromDegrees(-aBodyPart.angle));
    aCanvas.draw(rect);

    //sf::CircleShape cir;
    //cir.setRadius(2.5f);
    //cir.setPosition({aBodyPart.x, aBodyPart.y});
    //cir.setFillColor(sf::Color::Red);
    //aCanvas.draw(cir);
}

// TORSO
constexpr cpFloat kTorsoWidth  = 40.0;
constexpr cpFloat kTorsoHeight = 80.0;
constexpr cpFloat kTorsoAttachmentJointXOffset_leg = 0.0;
constexpr cpFloat kTorsoAttachmentJointYOffset_leg = 35.0;
constexpr cpFloat kTorsoAttachmentJointXOffset_arm = 0.0;
constexpr cpFloat kTorsoAttachmentJointYOffset_arm = -30.0;

// LEGS
constexpr cpFloat kThighWidth  = 20.0;
constexpr cpFloat kThighHeight = 50.0;
constexpr cpFloat kThighAttachmentJointXOffset = 0.0;
constexpr cpFloat kThighAttachmentJointYOffset = -20.0;
constexpr cpFloat kThighMiddleJointXOffset = 0.0;
constexpr cpFloat kThighMiddleJointYOffset = 20.0;

constexpr cpFloat kShinWidth   = 15.0;
constexpr cpFloat kShinHeight  = 50.0;
constexpr cpFloat kShinMiddleJointXOffset = 0.0;
constexpr cpFloat kShinMiddleJointYOffset = -20.0;
constexpr cpFloat kShinTipJointXOffset = 0.0;
constexpr cpFloat kShinTipJointYOffset = 20.0;

constexpr cpFloat kFootWidth   = 30.0;
constexpr cpFloat kFootHeight  = 10.0;
constexpr cpFloat kFootTipJointXOffset = 0.0;
constexpr cpFloat kFootTipJointYOffset = -10.0;

// ARMS
constexpr cpFloat kUpperArmWidth  = 20.0;
constexpr cpFloat kUpperArmHeight = 50.0;
constexpr cpFloat kUpperArmAttachmentJointXOffset = 0.0;
constexpr cpFloat kUpperArmAttachmentJointYOffset = -20.0;
constexpr cpFloat kUpperArmMiddleJointXOffset = 0.0;
constexpr cpFloat kUpperArmMiddleJointYOffset = 20.0;

constexpr cpFloat kForeArmWidth   = 15.0;
constexpr cpFloat kForeArmHeight  = 50.0;
constexpr cpFloat kForeArmMiddleJointXOffset = 0.0;
constexpr cpFloat kForeArmMiddleJointYOffset = -20.0;
constexpr cpFloat kForeArmTipJointXOffset = 0.0;
constexpr cpFloat kForeArmTipJointYOffset = 20.0;

constexpr cpFloat kHandWidth      = 25.0;
constexpr cpFloat kHandHeight     = 25.0;
constexpr cpFloat kHandTipJointXOffset = 0.0;
constexpr cpFloat kHandTipJointYOffset = -15.0;

Limb CreateLeg(cpSpace* aSpace, cpFloat aX, cpFloat aY) {
    auto thigh = SingleShapeBody::createDynamicBox(aSpace, kThighWidth, kThighHeight, 1.0, 100.0);
    auto shin  = SingleShapeBody::createDynamicBox(aSpace, kShinWidth, kShinHeight, 1.0, 100.0);
    auto foot  = SingleShapeBody::createDynamicBox(aSpace, kFootWidth, kFootHeight, 1.0, 100.0);

    thigh.setPosition(cpv(aX, aY + kThighHeight / 2.0));
    shin.setPosition(cpv(aX, aY + kThighHeight + kShinHeight / 2.0));
    foot.setPosition(cpv(aX, aY + kThighHeight + kShinHeight + kFootHeight / 2.0));

    cpShapeSetFilter(thigh.getShape(), cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
    cpShapeSetFilter(shin.getShape(), cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
    cpShapeSetFilter(foot.getShape(), cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));

    cpShapeSetFriction(thigh.getShape(), 0.2);
    cpShapeSetFriction(shin.getShape(),  0.2);
    cpShapeSetFriction(foot.getShape(),  0.9);

    cpShapeSetElasticity(thigh.getShape(), 0.2);
    cpShapeSetElasticity(shin.getShape(),  0.2);
    cpShapeSetElasticity(foot.getShape(),  0.2);

    LimbParameters lp;
    lp.attachmentJoint.jointOffsetA = cpv(kTorsoAttachmentJointXOffset_leg,  kTorsoAttachmentJointYOffset_leg);
    lp.attachmentJoint.jointOffsetB = cpv(kThighAttachmentJointXOffset, kThighAttachmentJointYOffset);
    lp.attachmentJoint.rotaryLimitMin = hg::math::DegToRad(-135.0);
    lp.attachmentJoint.rotaryLimitMax = hg::math::DegToRad(45.0);
    lp.middleJoint.jointOffsetA = cpv(kThighMiddleJointXOffset,  kThighMiddleJointYOffset);
    lp.middleJoint.jointOffsetB = cpv(kShinMiddleJointXOffset, kShinMiddleJointYOffset);
    lp.middleJoint.rotaryLimitMin = hg::math::DegToRad(0.0);
    lp.middleJoint.rotaryLimitMax = hg::math::DegToRad(135.0);
    lp.tipJoint.jointOffsetA = cpv(kShinTipJointXOffset, kShinTipJointYOffset);
    lp.tipJoint.jointOffsetB = cpv(kFootTipJointXOffset, kFootTipJointYOffset);
    lp.tipJoint.rotaryLimitMin = hg::math::DegToRad(-70.0);
    lp.tipJoint.rotaryLimitMax = hg::math::DegToRad(70.0);

    return {aSpace, std::move(thigh), std::move(shin), std::move(foot), lp};
}

Limb CreateArm(cpSpace* aSpace, cpFloat aX, cpFloat aY) {
    auto upperArm = SingleShapeBody::createDynamicBox(aSpace, kUpperArmWidth, kUpperArmHeight, 1.0, 100.0);
    auto foreArm  = SingleShapeBody::createDynamicBox(aSpace, kForeArmWidth, kForeArmHeight, 1.0, 100.0);
    auto hand     = SingleShapeBody::createDynamicBox(aSpace, kHandWidth, kHandHeight, 1.0, 100.0);

    upperArm.setPosition(cpv(aX, aY));
    foreArm.setPosition(cpv(aX, aY + kUpperArmHeight / 2.0));
    hand.setPosition(cpv(aX, aY + 60.0));

    cpShapeSetFilter(upperArm.getShape(), cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
    cpShapeSetFilter(foreArm.getShape(), cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
    cpShapeSetFilter(hand.getShape(), cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));

    cpShapeSetFriction(upperArm.getShape(), 0.2);
    cpShapeSetFriction(foreArm.getShape(),  0.2);
    cpShapeSetFriction(hand.getShape(),     0.7);

    cpShapeSetElasticity(upperArm.getShape(), 0.2);
    cpShapeSetElasticity(foreArm.getShape(),  0.2);
    cpShapeSetElasticity(hand.getShape(),     0.2);

    LimbParameters lp;
    lp.invertedMiddleJoint = true;
    lp.attachmentJoint.jointOffsetA = cpv(kTorsoAttachmentJointXOffset_arm, kTorsoAttachmentJointYOffset_arm);
    lp.attachmentJoint.jointOffsetB = cpv(kUpperArmAttachmentJointXOffset, kUpperArmAttachmentJointYOffset);
    lp.attachmentJoint.ignoreRotaryLimit = true;
    lp.middleJoint.jointOffsetA = cpv(kUpperArmMiddleJointXOffset,  kUpperArmMiddleJointYOffset);
    lp.middleJoint.jointOffsetB = cpv(kForeArmMiddleJointXOffset, kForeArmMiddleJointYOffset);
    lp.middleJoint.rotaryLimitMin = hg::math::DegToRad(-135.0);
    lp.middleJoint.rotaryLimitMax = hg::math::DegToRad(0.0);
    lp.tipJoint.jointOffsetA = cpv(kForeArmTipJointXOffset,  kForeArmTipJointYOffset);
    lp.tipJoint.jointOffsetB = cpv(kHandTipJointXOffset, kHandTipJointYOffset);
    lp.tipJoint.rotaryLimitMin = hg::math::DegToRad(-70.0);
    lp.tipJoint.rotaryLimitMax = hg::math::DegToRad(70.0);

    return {aSpace, std::move(upperArm), std::move(foreArm), std::move(hand), lp};
}

template <class taReal, class taVecFrom, class taVecTo>
hg::math::Angle<taReal> PointDirection2(const taVecFrom& vecFrom, const taVecTo& vecTo) {
    const auto xDiff = static_cast<taReal>(vecTo.x) - static_cast<taReal>(vecFrom.x);
    const auto yDiff = static_cast<taReal>(vecTo.y) - static_cast<taReal>(vecFrom.y);
    return hg::math::Angle<taReal>::fromRadians(std::atan2(-yDiff, xDiff));
}

template <class taReal>
hg::math::Angle<taReal> PointDirection2(taReal xFrom, taReal yFrom, taReal xTo, taReal yTo) {
    return PointDirection2<taReal>(hg::math::Vector2<taReal>{xFrom, yFrom}, hg::math::Vector2<taReal>{xTo, yTo});
}
} // namespace

class PlayerRobot::MasterData {
public:
    SingleShapeBody torso;
    Limb backLeg;
    Limb frontLeg;
    Limb backArm;
    Limb frontArm;
};

PlayerRobot::PlayerRobot(QAO_RuntimeRef aRuntimeRef,
                                 spe::RegistryId aRegId,
                                 spe::SyncId aSyncId)
    : SyncObjSuper{aRuntimeRef, SPEMPE_TYPEID_SELF, PRIORITY_PLAYERAVATAR,
                   "PlayerRobot", aRegId, aSyncId}
{
}

PlayerRobot::~PlayerRobot() {
    if (isMasterObject()) {
        doSyncDestroy();
    }
}

void PlayerRobot::init(cpFloat aX, cpFloat aY) {
    assert(isMasterObject());

    _masterData = std::make_unique<MasterData>();

    auto* space = ccomp<EnvironmentManagerInterface>().getPhysicsSpace();

    {
        auto torso = SingleShapeBody::createDynamicBox(space, kTorsoWidth, kTorsoHeight, 1.0, 1100.0);
        torso.setPosition(cpv(aX, aY));
        cpShapeSetFilter(torso.getShape(), cpShapeFilterNew(1, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
        cpShapeSetFriction(torso.getShape(), 0.3);
        cpShapeSetElasticity(torso.getShape(), 0.2);
        _masterData->torso = std::move(torso);
    }
    {
        _masterData->backLeg = CreateLeg(space, aX, aY);
        _masterData->backLeg.attach(_masterData->torso.getBody());
    }
    {
        _masterData->frontLeg = CreateLeg(space, aX, aY);
        _masterData->frontLeg.attach(_masterData->torso.getBody());
    }
    {
        _masterData->backArm = CreateArm(space, aX, aY);
        _masterData->backArm.attach(_masterData->torso.getBody());
    }
    {
        _masterData->frontArm = CreateArm(space, aX, aY);
        _masterData->frontArm.attach(_masterData->torso.getBody());
    }
}

void PlayerRobot::_eventUpdate1(spe::IfMaster) {
    if (ctx().getGameState().isPaused) return;

    auto& self = _getCurrentState();

    auto copyData = [this](const SingleShapeBody& aBody, BodyPart& aBodyPart) {
        const auto pos = aBody.getPosition();
        const auto siz = cpv(aBody.getWidth(), aBody.getHeight());
        const auto rot = aBody.getAngle().asDegrees();
        aBodyPart.x = static_cast<float>(pos.x);
        aBodyPart.y = static_cast<float>(pos.y);
        aBodyPart.width = static_cast<float>(siz.x);
        aBodyPart.height = static_cast<float>(siz.y);
        aBodyPart.angle = static_cast<float>(rot);
    };

    // Torso
    copyData(_masterData->torso, self.torso);
    // Legs
    copyData(_masterData->backLeg.getUpper(), self.backThigh);
    copyData(_masterData->backLeg.getFore(), self.backShin);
    copyData(_masterData->backLeg.getTip(), self.backFoot);
    copyData(_masterData->frontLeg.getUpper(), self.frontThigh);
    copyData(_masterData->frontLeg.getFore(), self.frontShin);
    copyData(_masterData->frontLeg.getTip(), self.frontFoot);
    // Arms
    copyData(_masterData->backArm.getUpper(), self.backUpperArm);
    copyData(_masterData->backArm.getFore(), self.backForeArm);
    copyData(_masterData->backArm.getTip(), self.backHand);
    copyData(_masterData->frontArm.getUpper(), self.frontUpperArm);
    copyData(_masterData->frontArm.getFore(), self.frontForeArm);
    copyData(_masterData->frontArm.getTip(), self.frontHand);
    
    // INPUT BEGIN
    auto& lobbyBackMgr = ccomp<MLobbyBackend>();
    auto& inputMgr = ccomp<MInput>();
    const spe::InputSyncManagerWrapper wrapper{inputMgr};

    constexpr auto _90deg = hg::math::Angle<cpFloat>::fromDegrees(90.0);

    // Player 1 (front leg)
    {
        const auto clientIndex = lobbyBackMgr.playerIdxToClientIdx(1);
        if (clientIndex == spe::CLIENT_INDEX_UNKNOWN) goto SKIP1;

        const auto cursorX = static_cast<cpFloat>(wrapper.getSignalValue<float>(clientIndex, CTRLNAME_CURSOR_X));
        const auto cursorY = static_cast<cpFloat>(wrapper.getSignalValue<float>(clientIndex, CTRLNAME_CURSOR_Y));
        const auto legOrigin = _masterData->torso.getPosition() + 
            cpvforangle((_masterData->torso.getAngle() + _90deg).asRadians()) * kTorsoAttachmentJointYOffset_leg;

        auto dist  = hg::math::EuclideanDist<cpFloat>(legOrigin.x, legOrigin.y, cursorX, cursorY);
        if (wrapper.getSignalValue<bool>(clientIndex, CTRLNAME_MIN_EXT)) {
            dist = 0.0;
        }
        if (wrapper.getSignalValue<bool>(clientIndex, CTRLNAME_MAX_EXT)) {
            dist = 1000.0;
        }

        const auto cursorToOrigin = PointDirection2<cpFloat>(cursorX, cursorY, legOrigin.x, legOrigin.y) * -1.0 - _90deg;
        const auto torsoAngle = _masterData->torso.getAngle();
        const auto cursorToOriginInTorsoPerspective = cursorToOrigin - torsoAngle;
        const auto cursorToOriginInTorsoPerspectiveMod = cursorToOriginInTorsoPerspective - (_90deg * 2.0);

        _masterData->frontLeg.update(dist, cursorToOriginInTorsoPerspectiveMod.asRadians());
    }
SKIP1: (void)0;

    // Player 2 (back leg)
    {
        const auto clientIndex = lobbyBackMgr.playerIdxToClientIdx(2);
        if (clientIndex == spe::CLIENT_INDEX_UNKNOWN) goto SKIP2;

        const auto cursorX = static_cast<cpFloat>(wrapper.getSignalValue<float>(clientIndex, CTRLNAME_CURSOR_X));
        const auto cursorY = static_cast<cpFloat>(wrapper.getSignalValue<float>(clientIndex, CTRLNAME_CURSOR_Y));
        const auto legOrigin = _masterData->torso.getPosition() + 
            cpvforangle((_masterData->torso.getAngle() + _90deg).asRadians()) * kTorsoAttachmentJointYOffset_leg;

        auto dist  = hg::math::EuclideanDist<cpFloat>(legOrigin.x, legOrigin.y, cursorX, cursorY);
        if (wrapper.getSignalValue<bool>(clientIndex, CTRLNAME_MIN_EXT)) {
            dist = 0.0;
        }
        if (wrapper.getSignalValue<bool>(clientIndex, CTRLNAME_MAX_EXT)) {
            dist = 1000.0;
        }
        
        const auto cursorToOrigin = PointDirection2<cpFloat>(cursorX, cursorY, legOrigin.x, legOrigin.y) * -1.0 - _90deg;
        const auto torsoAngle = _masterData->torso.getAngle();
        const auto cursorToOriginInTorsoPerspective = cursorToOrigin - torsoAngle;
        const auto cursorToOriginInTorsoPerspectiveMod = cursorToOriginInTorsoPerspective - (_90deg * 2.0);

        _masterData->backLeg.update(dist, cursorToOriginInTorsoPerspectiveMod.asRadians());
    }
SKIP2: (void)0;

    // Player 3 (front arm)
    {
        const auto clientIndex = lobbyBackMgr.playerIdxToClientIdx(3);
        if (clientIndex == spe::CLIENT_INDEX_UNKNOWN) goto SKIP3;

        const auto cursorX = wrapper.getSignalValue<float>(clientIndex, CTRLNAME_CURSOR_X);
        const auto cursorY = wrapper.getSignalValue<float>(clientIndex, CTRLNAME_CURSOR_Y);
        const auto armOrigin = _masterData->torso.getPosition() +
            cpvforangle((_masterData->torso.getAngle() + _90deg).asRadians()) * kTorsoAttachmentJointYOffset_arm;

        auto dist  = hg::math::EuclideanDist<cpFloat>(armOrigin.x, armOrigin.y, cursorX, cursorY);
        if (wrapper.getSignalValue<bool>(clientIndex, CTRLNAME_MIN_EXT)) {
            dist = 0.0;
        }
        if (wrapper.getSignalValue<bool>(clientIndex, CTRLNAME_MAX_EXT)) {
            dist = 1000.0;
        }
        
        const auto cursorToOrigin = PointDirection2<cpFloat>(cursorX, cursorY, armOrigin.x, armOrigin.y) * -1.0 - _90deg;
        const auto torsoAngle = _masterData->torso.getAngle();
        const auto cursorToOriginInTorsoPerspective = cursorToOrigin - torsoAngle;
        const auto cursorToOriginInTorsoPerspectiveMod = cursorToOriginInTorsoPerspective - (_90deg * 2.0);

        _masterData->frontArm.update(dist, cursorToOriginInTorsoPerspectiveMod.asRadians());
    }
SKIP3: (void)0;

    // Player 4 (back arm)
    {
        const auto clientIndex = lobbyBackMgr.playerIdxToClientIdx(4);
        if (clientIndex == spe::CLIENT_INDEX_UNKNOWN) goto SKIP4;

        const auto cursorX = wrapper.getSignalValue<float>(clientIndex, CTRLNAME_CURSOR_X);
        const auto cursorY = wrapper.getSignalValue<float>(clientIndex, CTRLNAME_CURSOR_Y);
        const auto armOrigin = _masterData->torso.getPosition() +
            cpvforangle((_masterData->torso.getAngle() + _90deg).asRadians()) * kTorsoAttachmentJointYOffset_arm;

        auto dist  = hg::math::EuclideanDist<cpFloat>(armOrigin.x, armOrigin.y, cursorX, cursorY);
        if (wrapper.getSignalValue<bool>(clientIndex, CTRLNAME_MIN_EXT)) {
            dist = 0.0;
        }
        if (wrapper.getSignalValue<bool>(clientIndex, CTRLNAME_MAX_EXT)) {
            dist = 1000.0;
        }

        const auto cursorToOrigin = PointDirection2<cpFloat>(cursorX, cursorY, armOrigin.x, armOrigin.y) * -1.0 - _90deg;
        const auto torsoAngle = _masterData->torso.getAngle();
        const auto cursorToOriginInTorsoPerspective = cursorToOrigin - torsoAngle;
        const auto cursorToOriginInTorsoPerspectiveMod = cursorToOriginInTorsoPerspective - (_90deg * 2.0);

        _masterData->backArm.update(dist, cursorToOriginInTorsoPerspectiveMod.asRadians());
    }
SKIP4: (void)0;
    // INPUT END
}

void PlayerRobot::_eventUpdate1(spe::IfDummy) {
    if (!isDeactivated()) {
        _adjustCamera();
    }
}

void PlayerRobot::_eventDraw1() {
    if (this->isDeactivated()) return;

    const auto& self = _getCurrentState();

    auto& canvas = ccomp<MWindow>().getCanvas();

    using hg::gr::Color;

    // Back arm
    {
        const auto fillColor = Color(191, 128, 64);
        const auto outlineColor = Color(217, 179, 140);
        const auto outlineThickness = 2.f;
        DrawBodyPart(canvas, self.backForeArm, fillColor, outlineColor, outlineThickness);
        DrawBodyPart(canvas, self.backUpperArm, fillColor, outlineColor, outlineThickness);
        DrawBodyPart(canvas, self.backHand, Color{128, 0, 0}, Color{179, 0, 0}, outlineThickness);
    }
    // Back leg
    {
        const auto fillColor = Color(0, 0, 153);
        const auto outlineColor = Color(26, 26, 255);
        const auto outlineThickness = 2.f;
        DrawBodyPart(canvas, self.backShin, fillColor, outlineColor, outlineThickness);
        DrawBodyPart(canvas, self.backThigh, fillColor, outlineColor, outlineThickness);
        DrawBodyPart(canvas, self.backFoot, Color{51, 51, 51}, Color{140, 140, 140}, outlineThickness);
    }
    // Front leg
    {
        const auto fillColor = Color(0, 0, 200);
        const auto outlineColor = Color(33, 33, 255);
        const auto outlineThickness = 2.f;
        DrawBodyPart(canvas, self.frontShin, fillColor, outlineColor, outlineThickness);
        DrawBodyPart(canvas, self.frontThigh, fillColor, outlineColor, outlineThickness);
        DrawBodyPart(canvas, self.frontFoot, Color{63, 63, 63}, Color{170, 170, 170}, outlineThickness);
    }
    // Torso
    {
        DrawBodyPart(canvas, self.torso, Color(230, 230, 230), Color(166, 166, 166), 2.0);
    }
    // Front arm
    {
        const auto fillColor = Color(210, 166, 121);
        const auto outlineColor = Color(236, 217, 198);
        const auto outlineThickness = 2.f;
        DrawBodyPart(canvas, self.frontForeArm, fillColor, outlineColor, outlineThickness);
        DrawBodyPart(canvas, self.frontUpperArm, fillColor, outlineColor, outlineThickness);
        DrawBodyPart(canvas, self.frontHand, Color{204, 0, 0}, Color{255, 0, 0}, outlineThickness);
    }    
}

SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(PlayerRobot, (CREATE, UPDATE, DESTROY));

void PlayerRobot::_syncCreateImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_CREATE_DEFAULT_IMPL(PlayerRobot, aSyncCtrl);
}

void PlayerRobot::_syncUpdateImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(PlayerRobot, aSyncCtrl);
}

void PlayerRobot::_syncDestroyImpl(spe::SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(PlayerRobot, aSyncCtrl);
}

void PlayerRobot::_adjustCamera() {
    const auto& self = _getCurrentState();

    auto& winMgr = ccomp<MWindow>();
    auto& view  = winMgr.getView();

    const auto& targetPos = sf::Vector2f{self.torso.x, self.torso.y};

    const auto viewCenter = view.getCenter();
    const auto dist  = hg::math::EuclideanDist<float>(viewCenter.x, viewCenter.y, targetPos.x, targetPos.y);
    const auto theta = PointDirection2<float>(view.getCenter(), targetPos).asRadians();
    if (dist >= 2.f) {
        view.move(
            +std::cosf(theta) * dist * 0.025f,
            -std::sinf(theta) * dist * 0.025f
        );
    } else {
        view.setCenter(targetPos);
    }

}

} // namespace hobrobot
