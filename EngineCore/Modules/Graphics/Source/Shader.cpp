
#include <Hobgoblin/Graphics/Shader.hpp>

#include <SFML/Graphics/Shader.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

#define VERIFY_VEC2_COMPATIBILITY(_hg_type_, _sf_type_) \
    static_assert( \
        sizeof(_hg_type_)  == sizeof(_sf_type_)          && \
        alignof(_hg_type_) == alignof(_sf_type_)         && \
        offsetof(_hg_type_, x) == offsetof(_sf_type_, x) && \
        offsetof(_hg_type_, y) == offsetof(_sf_type_, y) && \
        #_hg_type_ " is not bitwise-compatible with " #_sf_type_ \
    )

#define VERIFY_VEC3_COMPATIBILITY(_hg_type_, _sf_type_) \
    static_assert( \
        sizeof(_hg_type_)  == sizeof(_sf_type_)          && \
        alignof(_hg_type_) == alignof(_sf_type_)         && \
        offsetof(_hg_type_, x) == offsetof(_sf_type_, x) && \
        offsetof(_hg_type_, y) == offsetof(_sf_type_, y) && \
        offsetof(_hg_type_, z) == offsetof(_sf_type_, z) && \
        #_hg_type_ " is not bitwise-compatible with " #_sf_type_ \
    )

#define VERIFY_VEC4_COMPATIBILITY(_hg_type_, _sf_type_) \
    static_assert( \
        sizeof(_hg_type_)  == sizeof(_sf_type_)          && \
        alignof(_hg_type_) == alignof(_sf_type_)         && \
        offsetof(_hg_type_, x) == offsetof(_sf_type_, x) && \
        offsetof(_hg_type_, y) == offsetof(_sf_type_, y) && \
        offsetof(_hg_type_, z) == offsetof(_sf_type_, z) && \
        offsetof(_hg_type_, w) == offsetof(_sf_type_, w) && \
        #_hg_type_ " is not bitwise-compatible with " #_sf_type_ \
    )

// Checks to make sure we can reinterpret_cast between Hobgoblin's and SFML's Glsl types:

VERIFY_VEC2_COMPATIBILITY(Glsl::Vec2, sf::Glsl::Vec2);
VERIFY_VEC3_COMPATIBILITY(Glsl::Vec3, sf::Glsl::Vec3);
VERIFY_VEC4_COMPATIBILITY(Glsl::Vec4, sf::Glsl::Vec4);

VERIFY_VEC2_COMPATIBILITY(Glsl::Bvec2, sf::Glsl::Bvec2);
VERIFY_VEC3_COMPATIBILITY(Glsl::Bvec3, sf::Glsl::Bvec3);
VERIFY_VEC4_COMPATIBILITY(Glsl::Bvec4, sf::Glsl::Bvec4);

VERIFY_VEC2_COMPATIBILITY(Glsl::Ivec2, sf::Glsl::Ivec2);
VERIFY_VEC3_COMPATIBILITY(Glsl::Ivec3, sf::Glsl::Ivec3);
VERIFY_VEC4_COMPATIBILITY(Glsl::Ivec4, sf::Glsl::Ivec4);

using ImplType = sf::Shader;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

Shader::Shader() {
#ifdef _MSC_VER
    // WTF, Intellisense
    static_assert(STORAGE_SIZE  >= IMPL_SIZE,  "Shader::STORAGE_SIZE is too small.");
#else
    static_assert(STORAGE_SIZE  == IMPL_SIZE,  "Shader::STORAGE_SIZE is inadequate.");
#endif
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "Shader::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
}

Shader::~Shader() {
    SELF_IMPL->~ImplType();
}

bool Shader::loadFromFile(const std::string& aFilename, Type aType) {
    /* TODO */
    return false;
}

bool Shader::loadFromFile(const std::string& aVertexShaderFilename, const std::string& aFragmentShaderFilename) {
    /* TODO */
    return false;
}

bool Shader::loadFromFile(const std::string& aVertexShaderFilename, const std::string& aGeometryShaderFilename, const std::string& aFragmentShaderFilename) {
    /* TODO */
    return false;
}

bool Shader::loadFromMemory(const std::string& aShader, Type aType) {
    /* TODO */
    return false;
}

bool Shader::loadFromMemory(const std::string& aVertexShader, const std::string& aFragmentShader) {
    /* TODO */
    return false;
}

bool Shader::loadFromMemory(const std::string& aVertexShader, const std::string& aGeometryShader, const std::string& aFragmentShader) {
    /* TODO */
    return false;
}

//bool Shader::loadFromStream(InputStream& aStream, Type aType) {
//    /* TODO */
//}
//
//bool Shader::loadFromStream(InputStream& aVertexShaderStream, InputStream& aFragmentShaderStream) {
//    /* TODO */
//}
//
//bool Shader::loadFromStream(InputStream& aVertexShaderStream, InputStream& aGeometryShaderStream, InputStream& aFragmentShaderStream) {
//    /* TODO */
//}

void Shader::setUniform(const std::string& aName, float aX) {
    SELF_IMPL->setUniform(aName, aX);
}

void Shader::setUniform(const std::string& aName, const Glsl::Vec2& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Vec2&>(aVector));
}

void Shader::setUniform(const std::string& aName, const Glsl::Vec3& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Vec3&>(aVector));
}

void Shader::setUniform(const std::string& aName, const Glsl::Vec4& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Vec4&>(aVector));
}

void Shader::setUniform(const std::string& aName, int aX) {
    SELF_IMPL->setUniform(aName, aX);
}

void Shader::setUniform(const std::string& aName, const Glsl::Ivec2& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Ivec2&>(aVector));
}

void Shader::setUniform(const std::string& aName, const Glsl::Ivec3& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Ivec3&>(aVector));
}

void Shader::setUniform(const std::string& aName, const Glsl::Ivec4& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Ivec4&>(aVector));
}

void Shader::setUniform(const std::string& aName, bool aX) {
    SELF_IMPL->setUniform(aName, aX);
}

void Shader::setUniform(const std::string& aName, const Glsl::Bvec2& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Bvec2&>(aVector));
}

void Shader::setUniform(const std::string& aName, const Glsl::Bvec3& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Bvec3&>(aVector));
}

void Shader::setUniform(const std::string& aName, const Glsl::Bvec4& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Bvec4&>(aVector));
}

void Shader::setUniform(const std::string& aName, const Glsl::Mat3& aMatrix) { /* TODO */ }

void Shader::setUniform(const std::string& aName, const Glsl::Mat4& aMatrix) { /* TODO */ }

void Shader::setUniform(const std::string& aName, const Texture& aTexture) { /* TODO */ }

void Shader::setUniform(const std::string& aName, CurrentTextureType) {
    SELF_IMPL->setUniform(aName, sf::Shader::CurrentTexture);
}

void Shader::setUniformArray(const std::string& aName, const float* aScalarArray, PZInteger aLength) {
    SELF_IMPL->setUniformArray(aName, aScalarArray, pztos(aLength));
}

void Shader::setUniformArray(const std::string& aName, const Glsl::Vec2* aVectorArray, PZInteger aLength) {
    SELF_IMPL->setUniformArray(
        aName,
        reinterpret_cast<const sf::Glsl::Vec2*>(aVectorArray),
        pztos(aLength)
    );
}

void Shader::setUniformArray(const std::string& aName, const Glsl::Vec3* aVectorArray, PZInteger aLength) {
    SELF_IMPL->setUniformArray(
        aName,
        reinterpret_cast<const sf::Glsl::Vec3*>(aVectorArray),
        pztos(aLength)
    );
}

void Shader::setUniformArray(const std::string& aName, const Glsl::Vec4* aVectorArray, PZInteger aLength) {
    SELF_IMPL->setUniformArray(
        aName,
        reinterpret_cast<const sf::Glsl::Vec4*>(aVectorArray),
        pztos(aLength)
    );
}

void Shader::setUniformArray(const std::string& aName, const Glsl::Mat3* aMatrixArray, PZInteger aLength) {
    // TODO
}

void Shader::setUniformArray(const std::string& aName, const Glsl::Mat4* aMatrixArray, PZInteger aLength) {
    // TODO
}

unsigned int Shader::getNativeHandle() const {
    return SELF_CIMPL->getNativeHandle();
}

void Shader::bind(const Shader* aShader) {
    sf::Shader::bind((aShader != nullptr) ? CIMPLOF(*aShader) : nullptr);
}

bool Shader::isAvailable() {
    return sf::Shader::isAvailable();
}

bool Shader::isGeometryAvailable() {
    return sf::Shader::isGeometryAvailable();
}

void* Shader::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* Shader::_getSFMLImpl() const {
    return SELF_CIMPL;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
