#include "Limb.hpp"

#include <Hobgoblin/Math.hpp>

#include <cmath>

namespace hobrobot {

Limb::Limb()
    : _upper{}
    , _fore{}
    , _tip{}
    , _params{}
    , _space{nullptr}
{
}

Limb::Limb(Limb&& aOther) noexcept
    : _upper{std::move(aOther._upper)}
    , _fore{std::move(aOther._fore)}
    , _tip{std::move(aOther._tip)}
    , _attachmentJointConstraints{std::move(aOther._attachmentJointConstraints)}
    , _middleJointConstraints{std::move(aOther._middleJointConstraints)}
    , _tipJointConstraints{std::move(aOther._tipJointConstraints)}
    , _params{aOther._params}
    , _space{aOther._space}
    , _attachmentBody{aOther._attachmentBody}
{
    aOther._space = nullptr;
    aOther._attachmentBody = nullptr;
}

Limb& Limb::operator=(Limb&& aOther) noexcept {
    if (this != &aOther) {
        _upper = std::move(aOther._upper);
        _fore = std::move(aOther._fore);
        _tip = std::move(aOther._tip);
        _params = aOther._params;
        _space = aOther._space;
        _attachmentBody = aOther._attachmentBody;

        _attachmentJointConstraints = std::move(aOther._attachmentJointConstraints);
        _middleJointConstraints     = std::move(aOther._middleJointConstraints);
        _tipJointConstraints        = std::move(aOther._tipJointConstraints);

        aOther._space = nullptr;
        aOther._attachmentBody = nullptr;
    }
    return *this;
}

Limb::Limb(cpSpace* aSpace,
           SingleShapeBody aUpper,
           SingleShapeBody aFore,
           SingleShapeBody aTip,
           const LimbParameters& aLimbParams)
    : _upper{std::move(aUpper)}
    , _fore{std::move(aFore)}
    , _tip{std::move(aTip)}
    , _params{aLimbParams}
    , _space{aSpace}
{
    // MIDDLE JOINT
    {
        auto* pivot = cpSpaceAddConstraint(_space, cpPivotJointNew2(
            _upper.getBody(),
            _fore.getBody(),
            _params.middleJoint.jointOffsetA,
            _params.middleJoint.jointOffsetB));

        _middleJointConstraints.pivot.reset(pivot);
    }
    {
        if (!_params.middleJoint.ignoreRotaryLimit) {
            auto* rotaryLimit = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(
                _upper.getBody(),
                _fore.getBody(),
                _params.middleJoint.rotaryLimitMin,
                _params.middleJoint.rotaryLimitMax));

            _middleJointConstraints.rotaryLimit.reset(rotaryLimit);
        }
    }
    {
        auto* rotaryServo = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(
            _upper.getBody(),
            _fore.getBody(),
            0.0,
            0.0));

        cpConstraintSetMaxForce(rotaryServo, 10'000'000.0);
        _middleJointConstraints.rotaryServo.reset(rotaryServo);
    }

    // TIP JOINT
    {
        auto* pivot = cpSpaceAddConstraint(_space, cpPivotJointNew2(
            _fore.getBody(),
            _tip.getBody(),
            _params.tipJoint.jointOffsetA,
            _params.tipJoint.jointOffsetB));

        _tipJointConstraints.pivot.reset(pivot);
    }
    {
        if (!_params.tipJoint.ignoreRotaryLimit) {
            auto* rotaryLimit = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(
                _fore.getBody(),
                _tip.getBody(),
                _params.tipJoint.rotaryLimitMin,
                _params.tipJoint.rotaryLimitMax));

            _tipJointConstraints.rotaryLimit.reset(rotaryLimit);
        }
    }
    {
        auto* rotaryServo = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(
            _fore.getBody(),
            _tip.getBody(),
            0.0,
            0.0));

        cpConstraintSetMaxForce(rotaryServo, 10'000'000.0);
        _tipJointConstraints.rotaryServo.reset(rotaryServo);
    }
}

void Limb::attach(cpBody* aTargetBody) {
    _attachmentBody = aTargetBody;
    {
        auto* pivot = cpSpaceAddConstraint(_space, cpPivotJointNew2(
            _attachmentBody,
            _upper.getBody(),
            _params.attachmentJoint.jointOffsetA,
            _params.attachmentJoint.jointOffsetB));

        _attachmentJointConstraints.pivot.reset(pivot);
    }
    {
        if (!_params.attachmentJoint.ignoreRotaryLimit) {
            auto* rotaryLimit = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(
                _attachmentBody,
                _upper.getBody(),
                _params.attachmentJoint.rotaryLimitMin,
                _params.attachmentJoint.rotaryLimitMax));

            _attachmentJointConstraints.rotaryLimit.reset(rotaryLimit);
        }
    }
    {
        auto* rotaryServo = cpSpaceAddConstraint(_space, cpRotaryLimitJointNew(
            _attachmentBody,
            _upper.getBody(),
            0.0,
            0.0));

        cpConstraintSetMaxForce(rotaryServo, 10'000'000.0);
        _attachmentJointConstraints.rotaryServo.reset(rotaryServo);
    }  
}

void Limb::update(cpFloat aD, cpFloat aAngle) {
    using hg::math::Clamp;
    using hg::math::Sqr;
    using hg::math::PI;

    const auto u = _getUpperEffectiveHeight();
    const auto f = _getForeEffectiveHeight();
    const auto dMin = _params.invertedMiddleJoint ?
        sqrt(Sqr(u) + Sqr(f) - 2.0 * u * f * std::cos(PI - _params.middleJoint.rotaryLimitMin)) :
        sqrt(Sqr(u) + Sqr(f) - 2.0 * u * f * std::cos(PI - _params.middleJoint.rotaryLimitMax));
    const auto dMax = u + f;
    const auto d = Clamp(aD, dMin, dMax);
    
    const auto uInnerAngle = std::acos((Sqr(f) + Sqr(d) - Sqr(u)) / (2.0 * u * d));
    const auto fInnerAngle = std::acos((Sqr(u) + Sqr(d) - Sqr(f)) / (2.0 * f * d));
    const auto dInnerAngle = std::acos((Sqr(u) + Sqr(f) - Sqr(d)) / (2.0 * u * f));

    const auto mul = _params.invertedMiddleJoint ? 1.0 : -1.0;

    {
        auto& offsetAngle = _attachmentJointConstraints.offsetAngle;
        offsetAngle += hg::math::ShortestDistanceBetweenAngles(offsetAngle, aAngle);
        if (!_params.attachmentJoint.ignoreRotaryLimit) {
            offsetAngle = Clamp(offsetAngle, 
                                _params.attachmentJoint.rotaryLimitMin, 
                                _params.attachmentJoint.rotaryLimitMax);
        }
        cpRotaryLimitJointSetMin(_attachmentJointConstraints.rotaryServo.get(), fInnerAngle * mul + offsetAngle);
        cpRotaryLimitJointSetMax(_attachmentJointConstraints.rotaryServo.get(), fInnerAngle * mul + offsetAngle);
    }

    cpRotaryLimitJointSetMin(_middleJointConstraints.rotaryServo.get(), (dInnerAngle - PI) * mul);
    cpRotaryLimitJointSetMax(_middleJointConstraints.rotaryServo.get(), (dInnerAngle - PI) * mul);

    cpRotaryLimitJointSetMin(_tipJointConstraints.rotaryServo.get(), uInnerAngle * mul);
    cpRotaryLimitJointSetMax(_tipJointConstraints.rotaryServo.get(), uInnerAngle * mul);
}

const SingleShapeBody& Limb::getUpper() const {
    return _upper;
}

const SingleShapeBody& Limb::getFore() const {
    return _fore;
}

const SingleShapeBody& Limb::getTip() const {
    return _tip;
}

cpFloat Limb::_getUpperEffectiveHeight() const {
    return hg::math::EuclideanDist<cpFloat>(
        _params.attachmentJoint.jointOffsetB.x, _params.attachmentJoint.jointOffsetB.y, 
        _params.middleJoint.jointOffsetA.x, _params.middleJoint.jointOffsetA.y
    );
}

cpFloat Limb::_getForeEffectiveHeight() const {
    return hg::math::EuclideanDist<cpFloat>(
        _params.middleJoint.jointOffsetB.x, _params.middleJoint.jointOffsetB.y,
        _params.tipJoint.jointOffsetA.x, _params.tipJoint.jointOffsetA.y  
    );
}

} // namespace hobrobot
