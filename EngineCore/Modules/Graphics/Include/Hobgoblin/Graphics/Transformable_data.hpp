#ifndef UHOBGOBLIN_GRAPHICS_TRANSFORMABLE_DATA_HPP
#define UHOBGOBLIN_GRAPHICS_TRANSFORMABLE_DATA_HPP

#include <Hobgoblin/Graphics/Transform.hpp>
#include <Hobgoblin/Graphics/Transformable.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! Easy and convenient way to calculate transforms for classes
//! which need to implement the `Transformable` interface.
class TransformableData {
public:
    void setPosition(float aX, float aY);

    void setPosition(const math::Vector2f& aPosition);

    void setRotation(math::AngleF aAngle);

    void setScale(float aFactorX, float aFactorY);

    void setScale(const math::Vector2f& aFactors);

    void setOrigin(float aX, float aY);

    void setOrigin(const math::Vector2f& aOrigin);

    math::Vector2f getPosition() const;

    math::AngleF getRotation() const;

    math::Vector2f getScale() const;

    math::Vector2f getOrigin() const;

    void move(float aOffsetX, float aOffsetY);

    void move(const math::Vector2f& aOffset);

    void rotate(math::AngleF aAngle);

    void scale(float aFactorX, float aFactorY);

    void scale(const math::Vector2f& aFactor);

    Transform getTransform() const;

    Transform getInverseTransform() const;

private:
    math::Vector2f    m_origin;                           //!< Origin of translation/rotation/scaling of the object
    math::Vector2f    m_position;                         //!< Position of the object in the 2D world
    math::AngleF      m_rotation{math::AngleF::zero()};   //!< Orientation of the object
    math::Vector2f    m_scale{1, 1};                      //!< Scale of the object
    mutable Transform m_transform;                        //!< Combined transformation of the object
    mutable Transform m_inverseTransform;                 //!< Combined transformation of the object
    mutable bool      m_transformNeedUpdate{true};        //!< Does the transform need to be recomputed?
    mutable bool      m_inverseTransformNeedUpdate{true}; //!< Does the transform need to be recomputed?
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_TRANSFORMABLE_DATA_HPP
