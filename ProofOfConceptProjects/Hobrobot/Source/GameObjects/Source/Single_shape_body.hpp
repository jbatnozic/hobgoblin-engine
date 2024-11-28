#pragma once

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Math.hpp>

namespace hobrobot {

//! Represents a Chipmunk Physics rigid body (of any type) and its singular shape.
class SingleShapeBody {
public:
    //! Creates an empty object (no body and no shape).
    SingleShapeBody() = default;

    ~SingleShapeBody() = default;

    SingleShapeBody(SingleShapeBody&& aOther) noexcept;

    SingleShapeBody(const SingleShapeBody&) = delete;

    SingleShapeBody& operator=(SingleShapeBody&& aOther) noexcept;

    SingleShapeBody& operator=(const SingleShapeBody&) = delete;

    ///////////////////////////////////////////////////////////////////////////
    // FACTORY METHODS                                                       //
    ///////////////////////////////////////////////////////////////////////////

    static SingleShapeBody createDynamicBox(cpSpace* aSpace,
                                            cpFloat aWidth, 
                                            cpFloat aHeight,
                                            cpFloat aCornerRadius,
                                            cpFloat aMass);

    ///////////////////////////////////////////////////////////////////////////
    // PROPERTY GETTERS & SETTERS                                            //
    ///////////////////////////////////////////////////////////////////////////

    cpBody* getBody() const;

    cpShape* getShape() const;

    cpFloat getWidth() const;

    cpFloat getHeight() const;
    
    void setUserData(CP_DATA_POINTER_TYPE aUserData);

    CP_DATA_POINTER_TYPE getUserData() const;

    void setPosition(cpVect aPos);

    cpVect getPosition() const;

    void setAngle(hg::math::Angle<cpFloat> aAngle);

    hg::math::Angle<cpFloat> getAngle() const;

    void setElasticity(cpFloat aElasticity);

    cpFloat getElasticity() const;

private:
    hg::cpBodyUPtr _body = nullptr;
    hg::cpShapeUPtr _shape = nullptr;
    cpFloat _width = 0.0;
    cpFloat _height = 0.0;
};

} // namespace hobrobot
