
#include "Single_shape_body.hpp"

namespace hobrobot {

SingleShapeBody::SingleShapeBody(SingleShapeBody&& aOther) noexcept
    : _body{std::move(aOther._body)}
    , _shape{std::move(aOther._shape)}
    , _width{aOther._width}
    , _height{aOther._height}
{
}

SingleShapeBody& SingleShapeBody::operator=(SingleShapeBody&& aOther) noexcept {
    if (this != &aOther) {
        _body = std::move(aOther._body);
        _shape = std::move(aOther._shape);
        _width = aOther._width;
        _height = aOther._height;
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////
// FACTORY METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

SingleShapeBody SingleShapeBody::createDynamicBox(cpSpace* aSpace,
                                        cpFloat aWidth, 
                                        cpFloat aHeight,
                                        cpFloat aCornerRadius,
                                        cpFloat aMass) {
    SingleShapeBody obj;
    obj._width = aWidth;
    obj._height = aHeight;
    obj._body.reset(cpSpaceAddBody(aSpace, cpBodyNew(0.0, 0.0)));
    obj._shape.reset(cpSpaceAddShape(aSpace, cpBoxShapeNew(obj._body.get(), aWidth, aHeight, aCornerRadius)));
    cpShapeSetMass(obj._shape.get(), aMass);
    return obj;
}

///////////////////////////////////////////////////////////////////////////
// PROPERTY GETTERS & SETTERS                                            //
///////////////////////////////////////////////////////////////////////////

cpBody* SingleShapeBody::getBody() const {
    return _body.get();
}

cpShape* SingleShapeBody::getShape() const {
    return _shape.get();
}

cpFloat SingleShapeBody::getWidth() const {
    return _width;
}

cpFloat SingleShapeBody::getHeight() const {
    return _height;
}

void SingleShapeBody::setUserData(CP_DATA_POINTER_TYPE aUserData) {
    assert(_body);
    cpBodySetUserData(_body.get(), aUserData);
}

CP_DATA_POINTER_TYPE SingleShapeBody::getUserData() const {
    assert(_body);
    return cpBodyGetUserData(_body.get());
}

void SingleShapeBody::setPosition(cpVect aPos) {
    assert(_body);
    cpBodySetPosition(_body.get(), aPos);
}

cpVect SingleShapeBody::getPosition() const {
    assert(_body);
    return cpBodyGetPosition(_body.get());
}

void SingleShapeBody::setAngle(hg::math::Angle<cpFloat> aAngle) {
    assert(_body);
    cpBodySetAngle(_body.get(), aAngle.asRadians());
}

hg::math::Angle<cpFloat> SingleShapeBody::getAngle() const {
    assert(_body);
    return hg::math::Angle<cpFloat>::fromRadians(cpBodyGetAngle(_body.get()));
}

void SingleShapeBody::setElasticity(cpFloat aElasticity) {
    assert(_shape);
    cpShapeSetElasticity(_shape.get(), aElasticity);
}

cpFloat SingleShapeBody::getElasticity() const {
    assert(_shape);
    return cpShapeGetElasticity(_shape.get());
}

} // namespace hobrobot