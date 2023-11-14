#ifndef UHOBGOBLIN_GRAPHICS_MULTISPRITE_BLUEPRINT_HPP
#define UHOBGOBLIN_GRAPHICS_MULTISPRITE_BLUEPRINT_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Graphics/Sprite.hpp>
#include <Hobgoblin/Graphics/Sprite_blueprint.hpp>
#include <Hobgoblin/Graphics/Multisprite.hpp>

#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class MultispriteBlueprint {
public:
    ///////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS                                                          //
    ///////////////////////////////////////////////////////////////////////////

    // Construct with single subsprite
    MultispriteBlueprint(const Texture& aTexture, TextureRect aTextureRect);

    // Construct with one or more subsprites
    template <class taRects>
    MultispriteBlueprint(const Texture& aTexture, const taRects& aTextureRects);

    ///////////////////////////////////////////////////////////////////////////
    // COPIES & MOVES                                                        //
    ///////////////////////////////////////////////////////////////////////////

    MultispriteBlueprint(const MultispriteBlueprint& aOther);

    MultispriteBlueprint& operator=(const MultispriteBlueprint& aOther);

    MultispriteBlueprint(MultispriteBlueprint&& aOther);

    MultispriteBlueprint& operator=(MultispriteBlueprint&& aOther);

    ///////////////////////////////////////////////////////////////////////////
    // GETTERS                                                               //
    ///////////////////////////////////////////////////////////////////////////

    //! Returns number of contained subsprites.
    PZInteger getSubspriteCount() const;

    //! Make a regular Sprite from a single subsprite from within the blueprint.
    Sprite subspr(PZInteger aSubspriteIndex) const;

    //! Make a complete Multisprite from the whole blueprint.
    Multisprite multispr() const;

    //! Extracts a SpriteBlueprint based on one of the subsprite contained in
    //! this MultispriteBlueprint.
    SpriteBlueprint extractBlueprint(PZInteger aSubspriteIndex) const;

private:
    const Texture* _texture;

    std::variant<TextureRect, std::vector<TextureRect>> _textureRects;

    PZInteger _subspriteCount;
};

template <class taRects>
MultispriteBlueprint::MultispriteBlueprint(const Texture& aTexture, const taRects& aTextureRects)
    : _texture{&aTexture}
    , _subspriteCount{stopz(aTextureRects.size())}
{
    if (_subspriteCount > 1) {
        _textureRects = std::vector<TextureRect>{};
        for (const auto& rect : aTextureRects) {
            std::get<std::vector<TextureRect>>(_textureRects).push_back(rect);
        }
        return;
    }

    if (_subspriteCount == 1) {
        _textureRects = *(aTextureRects.begin());
        return;
    }

    HG_THROW_TRACED(InvalidArgumentError, 0, "MultispriteBlueprint must be constructed with at least 1 subsprite.");
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_MULTISPRITE_BLUEPRINT_HPP
