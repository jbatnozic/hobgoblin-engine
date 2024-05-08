// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Graphics/Font.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <SFML/Graphics/Font.hpp>

#include <new>
#include <utility>

#include "SFML_conversions.hpp"
#include "SFML_err.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType = sf::Font;
constexpr auto IMPL_SIZE = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define IMPLOF(_obj_)  (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define SELF_IMPL      (IMPLOF(SELF))
#define SELF_CIMPL     (CIMPLOF(SELF))

Font::Font() {
    static_assert(STORAGE_SIZE == IMPL_SIZE, "Font::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "Font::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
}

Font::Font(const Font& aOther) {
    new (&_storage) ImplType(*CIMPLOF(aOther));
}

Font& Font::operator=(const Font& aOther) {
    if (this != &aOther) {
        *SELF_IMPL = *CIMPLOF(aOther);
    }
    return SELF;
}

Font::Font(Font&& aOther) noexcept {
    new (&_storage) ImplType(std::move(*IMPLOF(aOther)));
}

Font& Font::operator=(Font&& aOther) noexcept {
    if (this != &aOther) {
        *SELF_IMPL = std::move(*IMPLOF(aOther));
    }
    return SELF;
}

Font::~Font() {
    SELF_IMPL->~ImplType();
}

void Font::loadFromFile(const std::filesystem::path& aFile) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->loadFromFile(FilesystemPathToSfPath(aFile))) {
        HG_THROW_TRACED(IOError, 0, sfErr.getErrorMessage());
    }
}

void Font::loadFromMemory(const void* aData, PZInteger aByteCount) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->loadFromMemory(aData, aByteCount)) {
        HG_THROW_TRACED(IOError, 0, sfErr.getErrorMessage());
    }
}

Glyph Font::getGlyph(std::uint32_t aCodePoint,
                     PZInteger     aCharacterSize,
                     bool          aBold,
                     float         aOutlineThickness) const {
    return ToHg(SELF_CIMPL->getGlyph(aCodePoint,
                                     static_cast<unsigned>(aCharacterSize),
                                     aBold,
                                     aOutlineThickness));
}

bool Font::hasGlyph(std::uint32_t aCodePoint) const {
    return SELF_CIMPL->hasGlyph(aCodePoint);
}

float Font::getKerning(std::uint32_t aFirst,
                       std::uint32_t aSecond,
                       PZInteger     aCharacterSize,
                       bool          aBold) const {
    return SELF_CIMPL->getKerning(aFirst, aSecond, static_cast<unsigned>(aCharacterSize), aBold);
}

float Font::getLineSpacing(PZInteger aCharacterSize) const {
    return SELF_CIMPL->getLineSpacing(aCharacterSize);
}

float Font::getUnderlinePosition(PZInteger aCharacterSize) const {
    return SELF_CIMPL->getUnderlinePosition(aCharacterSize);
}

float Font::getUnderlineThickness(PZInteger aCharacterSize) const {
    return SELF_CIMPL->getUnderlineThickness(aCharacterSize);
}

void Font::setSmooth(bool aSmooth) {
    SELF_IMPL->setSmooth(aSmooth);
}

bool Font::isSmooth() const {
    return SELF_CIMPL->isSmooth();
}

void* Font::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* Font::_getSFMLImpl() const {
    return SELF_CIMPL;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
