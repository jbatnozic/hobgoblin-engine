
#include <Hobgoblin/Graphics/Transformable_data.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

void TransformableData::setPosition(float aX, float aY) {
    setPosition({aX, aY});
}

void TransformableData::setPosition(const math::Vector2f& position) {
    m_position                   = position;
    m_transformNeedUpdate        = true;
    m_inverseTransformNeedUpdate = true;
}

void TransformableData::setRotation(math::AngleF angle) {
    m_rotation = angle;
    m_transformNeedUpdate        = true;
    m_inverseTransformNeedUpdate = true;
}

void TransformableData::setScale(float aFactorX, float aFactorY) {
    setScale({aFactorX, aFactorY});
}

void TransformableData::setScale(const math::Vector2f& factors) {
    m_scale                      = factors;
    m_transformNeedUpdate        = true;
    m_inverseTransformNeedUpdate = true;
}

void TransformableData::setOrigin(float aX, float aY) {
    setOrigin({aX, aY});
}

void TransformableData::setOrigin(const math::Vector2f& origin) {
    m_origin                     = origin;
    m_transformNeedUpdate        = true;
    m_inverseTransformNeedUpdate = true;
}

math::Vector2f TransformableData::getPosition() const {
    return m_position;
}

math::AngleF TransformableData::getRotation() const {
    return m_rotation;
}

math::Vector2f TransformableData::getScale() const {
    return m_scale;
}

math::Vector2f TransformableData::getOrigin() const {
    return m_origin;
}

void TransformableData::move(float aOffsetX, float aOffsetY) {
    const auto currentPosition = getPosition();
    setPosition(currentPosition.x + aOffsetX, currentPosition.y + aOffsetY);
}

void TransformableData::move(const math::Vector2f& offset) {
    setPosition(m_position + offset);
}

void TransformableData::rotate(math::AngleF angle) {
    setRotation(m_rotation + angle);
}

void TransformableData::scale(float aFactorX, float aFactorY) {
    setScale({aFactorX, aFactorY});
}

void TransformableData::scale(const math::Vector2f& factor) {
    setScale({m_scale.x * factor.x, m_scale.y * factor.y});
}

Transform TransformableData::getTransform() const {
    // Recompute the combined transform if needed
    if (m_transformNeedUpdate)
    {
        const float angle  = -m_rotation.asRadians();
        const float cosine = std::cos(angle);
        const float sine   = std::sin(angle);
        const float sxc    = m_scale.x * cosine;
        const float syc    = m_scale.y * cosine;
        const float sxs    = m_scale.x * sine;
        const float sys    = m_scale.y * sine;
        const float tx     = -m_origin.x * sxc - m_origin.y * sys + m_position.x;
        const float ty     = m_origin.x * sxs - m_origin.y * syc + m_position.y;

        m_transform = Transform( sxc, sys, tx,
                                -sxs, syc, ty,
                                 0.f, 0.f, 1.f);
        m_transformNeedUpdate = false;
    }

    return m_transform;
}

Transform TransformableData::getInverseTransform() const {
    // Recompute the inverse transform if needed
    if (m_inverseTransformNeedUpdate)
    {
        m_inverseTransform           = getTransform().getInverse();
        m_inverseTransformNeedUpdate = false;
    }

    return m_inverseTransform;
}
} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>
