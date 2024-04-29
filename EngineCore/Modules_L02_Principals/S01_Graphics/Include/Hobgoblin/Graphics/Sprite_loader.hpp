// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_GRAPHICS_SPRITE_LOADER_HPP
#define UHOBGOBLIN_GRAPHICS_SPRITE_LOADER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Sprite.hpp>
#include <Hobgoblin/Graphics/Sprite_blueprint.hpp>
#include <Hobgoblin/Graphics/Multisprite.hpp>
#include <Hobgoblin/Graphics/Multisprite_blueprint.hpp>
#include <Hobgoblin/Graphics/Texture.hpp>
#include <Hobgoblin/Graphics/Texture_packing.hpp>

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! Used to refer to a sprite by a numerical handle.
using SpriteIdNumerical = PZInteger;

//! Used to refer to a sprite by a textual handle (name).
using SpriteIdTextual = std::string;

namespace detail {
class TextureBuilderImpl;
} // namespace detail

class SpriteLoader {
public:
    ///////////////////////////////////////////////////////////////////////////
    // TEXTUREBUILDER                                                        //
    ///////////////////////////////////////////////////////////////////////////

    class TextureBuilder {
    public:
        //! Creates a new sprite, then loads an image from a file and adds it as the
        //! sprite's first subsprite.
        //! 
        //! \param aSpriteId Identifier of the newly created sprite.
        //! \param aFilePath Path to the image file to load.
        //! 
        //! \throws <exception> If the sprite by the given ID already exists.
        //! 
        //! \note As the `addSubsprite` method will add a new sprite if it does not
        //!       already exist, this method is not mandatory to use and is
        //!       basically syntatic sugar.
        virtual NeverNull<TextureBuilder*> addSprite(SpriteIdNumerical aSpriteId,
                                                   const std::filesystem::path& aFilePath) = 0;                       

        //! Loads an image from a file and adds it as a subsprite.
        //! 
        //! \param aSpriteId Identifier of the sprite to which to add the subsprite.
        //!                  If there does not already exist a sprite by this ID, it will
        //!                  be created automatically.
        //! \param aSubspriteIndex Index under which to store the loaded subsprite. If multiple
        //!                        loaded images target the same subsprite of a single sprite,
        //!                        which one will be kept is undefined.
        //! \param aFilePath Path to the image file to load.
        virtual NeverNull<TextureBuilder*> addSubsprite(SpriteIdNumerical aSpriteId,
                                                      PZInteger aSubspriteIndex,
                                                      const std::filesystem::path& aFilePath) = 0;

        //! Loads an image from a file and adds it as a subsprite.
        //! 
        //! \param aSpriteId Identifier of the sprite to which to add the subsprite.
        //!                  If there does not already exist a sprite by this ID, it will
        //!                  be created automatically.
        //! \param aFilePath Path to the image file to load.
        //! 
        //! \note As this method does not take a subsprite index as a parameter, the loaded
        //!       image will be appended to the sprite's array of subsprites.
        virtual NeverNull<TextureBuilder*>addSubsprite(SpriteIdNumerical aSpriteId,
                                                     const std::filesystem::path& aFilePath) = 0;

        //! Creates a new sprite, then loads an image from a file and adds it as the
        //! sprite's first subsprite.
        //! 
        //! \param aSpriteId Identifier of the newly created sprite.
        //! \param aFilePath Path to the image file to load.
        //! 
        //! \throws <exception> If the sprite by the given ID already exists.
        //! 
        //! \note As the `addSubsprite` method will add a new sprite if it does not
        //!       already exist, this method is not mandatory to use and is
        //!       basically syntatic sugar.
        virtual NeverNull<TextureBuilder*> addSprite(const SpriteIdTextual& aSpriteId,
                                                   const std::filesystem::path& aFilePath) = 0;

        //! Loads an image from a file and adds it as a subsprite.
        //! 
        //! \param aSpriteId Identifier of the sprite to which to add the subsprite.
        //!                  If there does not already exist a sprite by this ID, it will
        //!                  be created automatically.
        //! \param aSubspriteIndex Index under which to store the loaded subsprite. If multiple
        //!                        loaded images target the same subsprite of a single sprite,
        //!                        which one will be kept is undefined.
        //! \param aFilePath Path to the image file to load.
        virtual NeverNull<TextureBuilder*> addSubsprite(const SpriteIdTextual& aSpriteId,
                                                      PZInteger aSubspriteIndex,
                                                      const std::filesystem::path& aFilePath) = 0;

        //! Loads an image from a file and adds it as a subsprite.
        //! 
        //! \param aSpriteId Identifier of the sprite to which to add the subsprite.
        //!                  If there does not already exist a sprite by this ID, it will
        //!                  be created automatically.
        //! \param aFilePath Path to the image file to load.
        //! 
        //! \note As this method does not take a subsprite index as a parameter, the loaded
        //!       image will be appended to the sprite's array of subsprites.
        virtual NeverNull<TextureBuilder*> addSubsprite(const SpriteIdTextual& aSpriteId,
                                                      const std::filesystem::path& aFilePath) = 0;

        // TODO: loading images from memory

        //! Builds a texture from all the added images.
        //! 
        //! \param aHeuristic Texture packing heuristic to use.
        //! \param aOccupancy Pointer to a float. If not null, it will be set to a value between
        //!                   0.f and 1.f signifying how much of the texture's surface was taken
        //!                   up by the loaded images.
        //! 
        //! \returns Const reference to the built texture. Note that the lifetime of the texture
        //!          will be managed by the parent `SpriteLoader` of this `TextureBuilder`.
        //! 
        //! \throws TextturePackignError in case of failure (most commonly not enough room on the
        //!                              target texture for all the requested sprites).
        virtual const Texture& finalize(TexturePackingHeuristic aHeuristic, 
                                        float* aOccupancy = nullptr) = 0;

        //! Virtual destructor.
        virtual ~TextureBuilder() = default;
    };

    ///////////////////////////////////////////////////////////////////////////
    // SPRITELOADER                                                          //
    ///////////////////////////////////////////////////////////////////////////

    //! Allocates a new texture with the given width and height (in pixels).
    //! 
    //! \returns Unique pointer to `TextureBuilder` which can be used to
    //!          load sprite and subsprite images to the texture. The pointer is
    //!          guaranteed to not be null.
    //!          Don't forget to call `finalize()`!
    AvoidNull<std::unique_ptr<TextureBuilder>> startTexture(PZInteger aWidth, PZInteger aHeight);

    void removeTexture(Texture& aTexture);

    SpriteBlueprint getBlueprint(SpriteIdNumerical aSpriteId) const;

    SpriteBlueprint getBlueprint(const SpriteIdTextual& aSpriteId) const;

    const MultispriteBlueprint& getMultiBlueprint(SpriteIdNumerical aSpriteId) const;

    const MultispriteBlueprint& getMultiBlueprint(const SpriteIdTextual& aSpriteId) const;

    //! Deletes all texture and sprite blueprint data from the loader,
    //! returning it to its initial state. Be careful not to keep using
    //! any sprites or sprite blueprints which you previously got from
    //! this loader, which will now reference deallocated textures.
    void clear();

private:
    friend class detail::TextureBuilderImpl;

    std::vector<std::unique_ptr<Texture>> _textures;

    std::unordered_map<PZInteger, MultispriteBlueprint> _indexedBlueprints;
    std::unordered_map<std::string, MultispriteBlueprint> _mappedBlueprints;

    void _pushBlueprint(SpriteIdNumerical aSpriteId, MultispriteBlueprint aBlueprint);

    void _pushBlueprint(const SpriteIdTextual& aSpriteId, MultispriteBlueprint aBlueprint);

    void _pushTexture(std::unique_ptr<Texture> aTexture);
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_SPRITE_LOADER_HPP
