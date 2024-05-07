// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Graphics/Shader.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <SFML/Graphics/Shader.hpp>

#include <cstddef>
#include <new>
#include <utility>

#include "SFML_conversions.hpp"
#include "SFML_err.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

#define VERIFY_VEC2_COMPATIBILITY(_hg_type_, _sf_type_) \
    static_assert( \
        sizeof(_hg_type_)  == sizeof(_sf_type_)          && \
        alignof(_hg_type_) == alignof(_sf_type_)         && \
        offsetof(_hg_type_, x) == offsetof(_sf_type_, x) && \
        offsetof(_hg_type_, y) == offsetof(_sf_type_, y) ,  \
        #_hg_type_ " is not bitwise-compatible with " #_sf_type_ \
    )

#define VERIFY_VEC3_COMPATIBILITY(_hg_type_, _sf_type_) \
    static_assert( \
        sizeof(_hg_type_)  == sizeof(_sf_type_)          && \
        alignof(_hg_type_) == alignof(_sf_type_)         && \
        offsetof(_hg_type_, x) == offsetof(_sf_type_, x) && \
        offsetof(_hg_type_, y) == offsetof(_sf_type_, y) && \
        offsetof(_hg_type_, z) == offsetof(_sf_type_, z) ,  \
        #_hg_type_ " is not bitwise-compatible with " #_sf_type_ \
    )

#define VERIFY_VEC4_COMPATIBILITY(_hg_type_, _sf_type_) \
    static_assert( \
        sizeof(_hg_type_)  == sizeof(_sf_type_)          && \
        alignof(_hg_type_) == alignof(_sf_type_)         && \
        offsetof(_hg_type_, x) == offsetof(_sf_type_, x) && \
        offsetof(_hg_type_, y) == offsetof(_sf_type_, y) && \
        offsetof(_hg_type_, z) == offsetof(_sf_type_, z) && \
        offsetof(_hg_type_, w) == offsetof(_sf_type_, w) ,  \
        #_hg_type_ " is not bitwise-compatible with " #_sf_type_ \
    )

#define VERIFY_MATX_COMPATIBILITY(_hg_type_, _sf_type_) \
    static_assert( \
        sizeof(_hg_type_)  == sizeof(_sf_type_)                                           && \
        alignof(_hg_type_) == alignof(_sf_type_)                                          && \
        offsetof(_hg_type_, data) == offsetof(_sf_type_, array)                           && \
        sizeof(std::declval<_hg_type_>().data) == sizeof(std::declval<_sf_type_>().array) ,  \
        #_hg_type_ " is not bitwise-compatible with " #_sf_type_ \
    )

// Checks to make sure we can reinterpret_cast between Hobgoblin's and SFML's GLSL types:

VERIFY_VEC2_COMPATIBILITY(glsl::Vec2, sf::Glsl::Vec2);
VERIFY_VEC3_COMPATIBILITY(glsl::Vec3, sf::Glsl::Vec3);
VERIFY_VEC4_COMPATIBILITY(glsl::Vec4, sf::Glsl::Vec4);

VERIFY_VEC2_COMPATIBILITY(glsl::Bvec2, sf::Glsl::Bvec2);
VERIFY_VEC3_COMPATIBILITY(glsl::Bvec3, sf::Glsl::Bvec3);
VERIFY_VEC4_COMPATIBILITY(glsl::Bvec4, sf::Glsl::Bvec4);

VERIFY_VEC2_COMPATIBILITY(glsl::Ivec2, sf::Glsl::Ivec2);
VERIFY_VEC3_COMPATIBILITY(glsl::Ivec3, sf::Glsl::Ivec3);
VERIFY_VEC4_COMPATIBILITY(glsl::Ivec4, sf::Glsl::Ivec4);

VERIFY_MATX_COMPATIBILITY(glsl::Mat3, sf::Glsl::Mat3);
VERIFY_MATX_COMPATIBILITY(glsl::Mat4, sf::Glsl::Mat4);

namespace {
sf::Shader::Type ToSf(Shader::Type aType) {
    switch (aType) {
    case Shader::VERTEX:
        return sf::Shader::Vertex;

    case Shader::GEOMETRY:
        return sf::Shader::Geometry;

    case Shader::FRAGMENT:
        return sf::Shader::Fragment;

    default:
        HG_UNREACHABLE("Invalid value for hg::gr::Shader::Type ({}).", (int)aType);
    }

    return {};
}
} // namespace

#define  SELF_IMPL (&(SELF._impl->shader))
#define SELF_CIMPL static_cast<const sf::Shader*>(&(SELF._impl->shader))

class Shader::Impl {
public:
    sf::Shader shader;
};

Shader::Shader()
    : _impl{std::make_unique<Impl>()}
{
}

Shader::~Shader() = default;

void Shader::loadFromFile(const std::filesystem::path& aFile, Type aType) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->loadFromFile(FilesystemPathToSfPath(aFile), ToSf(aType))) {
        HG_THROW_TRACED(IOError, 0, sfErr.getErrorMessage());
    }
}

void Shader::loadFromFile(const std::filesystem::path& aVertexShaderFile,
                          const std::filesystem::path& aFragmentShaderFile) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->loadFromFile(FilesystemPathToSfPath(aVertexShaderFile),
                                 FilesystemPathToSfPath(aFragmentShaderFile))) {
        HG_THROW_TRACED(IOError, 0, sfErr.getErrorMessage());
    }
}

void Shader::loadFromFile(const std::filesystem::path& aVertexShaderFile,
                          const std::filesystem::path& aFragmentShaderFile,
                          const std::filesystem::path& aGeometryShaderFile) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->loadFromFile(FilesystemPathToSfPath(aVertexShaderFile),
                                 FilesystemPathToSfPath(aGeometryShaderFile),
                                 FilesystemPathToSfPath(aFragmentShaderFile))) {
        HG_THROW_TRACED(IOError, 0, sfErr.getErrorMessage());
    }
}


void Shader::loadFromMemory(const std::string& aShaderSource, Type aType) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->loadFromMemory(aShaderSource, ToSf(aType))) {
        HG_THROW_TRACED(TracedRuntimeError, 0, sfErr.getErrorMessage());
    }
}

void Shader::loadFromMemory(const std::string& aVertexShaderSource,
                            const std::string& aFragmentShaderSource) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->loadFromMemory(aVertexShaderSource, aFragmentShaderSource)) {
        HG_THROW_TRACED(TracedRuntimeError, 0, sfErr.getErrorMessage());
    }
}

void Shader::loadFromMemory(const std::string& aVertexShaderSource,
                            const std::string& aFragmentShaderSource,
                            const std::string& aGeometryShaderSource) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->loadFromMemory(aVertexShaderSource,
                                   aGeometryShaderSource,
                                   aFragmentShaderSource)) {
        HG_THROW_TRACED(TracedRuntimeError, 0, sfErr.getErrorMessage());
    }
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

void Shader::setUniform(const std::string& aName, const glsl::Vec2& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Vec2&>(aVector));
}

void Shader::setUniform(const std::string& aName, const glsl::Vec3& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Vec3&>(aVector));
}

void Shader::setUniform(const std::string& aName, const glsl::Vec4& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Vec4&>(aVector));
}

void Shader::setUniform(const std::string& aName, int aX) {
    SELF_IMPL->setUniform(aName, aX);
}

void Shader::setUniform(const std::string& aName, const glsl::Ivec2& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Ivec2&>(aVector));
}

void Shader::setUniform(const std::string& aName, const glsl::Ivec3& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Ivec3&>(aVector));
}

void Shader::setUniform(const std::string& aName, const glsl::Ivec4& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Ivec4&>(aVector));
}

void Shader::setUniform(const std::string& aName, bool aX) {
    SELF_IMPL->setUniform(aName, aX);
}

void Shader::setUniform(const std::string& aName, const glsl::Bvec2& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Bvec2&>(aVector));
}

void Shader::setUniform(const std::string& aName, const glsl::Bvec3& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Bvec3&>(aVector));
}

void Shader::setUniform(const std::string& aName, const glsl::Bvec4& aVector) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Bvec4&>(aVector));
}

void Shader::setUniform(const std::string& aName, const glsl::Mat3& aMatrix) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Mat3&>(aMatrix));
}

void Shader::setUniform(const std::string& aName, const glsl::Mat4& aMatrix) {
    SELF_IMPL->setUniform(aName, reinterpret_cast<const sf::Glsl::Mat4&>(aMatrix));
}

void Shader::setUniform(const std::string& aName, const Texture& aTexture) {
    const auto& sfTexture = detail::GraphicsImplAccessor::getImplOf<sf::Texture>(aTexture);
    SELF_IMPL->setUniform(aName, &sfTexture);
}

void Shader::setUniform(const std::string& aName, CurrentTextureType) {
    SELF_IMPL->setUniform(aName, sf::Shader::CurrentTexture);
}

void Shader::setUniformArray(const std::string& aName, const float* aScalarArray, PZInteger aLength) {
    SELF_IMPL->setUniformArray(aName, aScalarArray, pztos(aLength));
}

void Shader::setUniformArray(const std::string& aName, const glsl::Vec2* aVectorArray, PZInteger aLength) {
    SELF_IMPL->setUniformArray(
        aName,
        reinterpret_cast<const sf::Glsl::Vec2*>(aVectorArray),
        pztos(aLength)
    );
}

void Shader::setUniformArray(const std::string& aName, const glsl::Vec3* aVectorArray, PZInteger aLength) {
    SELF_IMPL->setUniformArray(
        aName,
        reinterpret_cast<const sf::Glsl::Vec3*>(aVectorArray),
        pztos(aLength)
    );
}

void Shader::setUniformArray(const std::string& aName, const glsl::Vec4* aVectorArray, PZInteger aLength) {
    SELF_IMPL->setUniformArray(
        aName,
        reinterpret_cast<const sf::Glsl::Vec4*>(aVectorArray),
        pztos(aLength)
    );
}

void Shader::setUniformArray(const std::string& aName, const glsl::Mat3* aMatrixArray, PZInteger aLength) {
    SELF_IMPL->setUniformArray(
        aName,
        reinterpret_cast<const sf::Glsl::Mat3*>(aMatrixArray),
        pztos(aLength)
    );
}

void Shader::setUniformArray(const std::string& aName, const glsl::Mat4* aMatrixArray, PZInteger aLength) {
    SELF_IMPL->setUniformArray(
        aName,
        reinterpret_cast<const sf::Glsl::Mat4*>(aMatrixArray),
        pztos(aLength)
    );
}

unsigned int Shader::getNativeHandle() const {
    return SELF_CIMPL->getNativeHandle();
}

void Shader::bind(const Shader* aShader) {
    sf::Shader::bind((aShader != nullptr) ? &(aShader->_impl->shader) : nullptr);
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

// clang-format on
