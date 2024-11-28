#pragma once

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Math.hpp>

#include "Single_shape_body.hpp"

namespace hobrobot {

struct LimbParameters {
    struct {
        cpVect  jointOffsetA;
        cpVect  jointOffsetB;
        cpFloat rotaryLimitMin;
        cpFloat rotaryLimitMax;
        bool    ignoreRotaryLimit = false;
    } attachmentJoint, middleJoint, tipJoint;
    bool invertedMiddleJoint = false;
};

class Limb {
public:
    Limb();

    Limb(cpSpace* aSpace,
         SingleShapeBody aUpper,
         SingleShapeBody aFore,
         SingleShapeBody aTip,
         const LimbParameters& aLimbParams);

    Limb(Limb&& aOther) noexcept;

    Limb& operator=(Limb&& aOther) noexcept;

    Limb(const Limb&) = delete;
    Limb& operator=(const Limb&) = delete;

    void attach(cpBody* aTargetBody);

    void update(cpFloat aD, cpFloat aAngle);

    const SingleShapeBody& getUpper() const;
    const SingleShapeBody& getFore() const;
    const SingleShapeBody& getTip() const;

private:
    SingleShapeBody _upper; // upper arm, thigh
    SingleShapeBody _fore;  // forearm, shin
    SingleShapeBody _tip;   // hand, foot

    struct {
        hg::cpConstraintUPtr pivot;
        hg::cpConstraintUPtr rotaryLimit;
        hg::cpConstraintUPtr rotaryServo;
        cpFloat offsetAngle = 0.0;
    } _attachmentJointConstraints;
    struct {
        hg::cpConstraintUPtr pivot;
        hg::cpConstraintUPtr rotaryLimit;
        hg::cpConstraintUPtr rotaryServo;
    } _middleJointConstraints;
    struct {
        hg::cpConstraintUPtr pivot;
        hg::cpConstraintUPtr rotaryLimit;
        hg::cpConstraintUPtr rotaryServo;
    } _tipJointConstraints;

    LimbParameters _params;
    cpSpace* _space;
    cpBody* _attachmentBody = nullptr;
    

    cpFloat _getUpperEffectiveHeight() const;
    cpFloat _getForeEffectiveHeight() const;
};

} // namespace hobrobot
