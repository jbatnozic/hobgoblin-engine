#ifndef UHOBGOBLIN_GR_SPRITE_MANAGER_HPP
#define UHOBGOBLIN_GR_SPRITE_MANAGER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Multisprite.hpp>

#include <Hobgoblin/Utility/Rect.h> // TODO Temp.

#include <string>
#include <unordered_map>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

using TextureHandle = int;

constexpr struct AutoIndexType {} AUTO_INDEX;

class SpriteManager {
public:

    TextureHandle addTexture(PZInteger width, PZInteger height);

    // Loading from file:
    void loadFromFile(TextureHandle textureHandle, PZInteger spriteIndex,
                      PZInteger subspriteIndex, const std::string& filePath);

    void loadFromFile(TextureHandle textureHandle, PZInteger spriteIndex,
                      AutoIndexType, const std::string& filePath);

    void loadFromFile(TextureHandle textureHandle, std::string spriteName,
                      PZInteger subspriteIndex, const std::string& filePath);

    void loadFromFile(TextureHandle textureHandle, std::string spriteName,
                      AutoIndexType, const std::string& filePath);

    // Loading from directory:
    void loadFromDirectory(TextureHandle textureHandle, PZInteger spriteIndex, const std::string& filePath);

    void loadFromDirectory(TextureHandle textureHandle, std::string spriteName, const std::string& filePath);

    // Loading from memory:
    // TODO loadFromMemory

    // Search loaded sprites:
    Multisprite getSprite(PZInteger spriteIndex) const;
    Multisprite getSprite(const std::string& spriteName) const;

    // Other:
    void finalize();
    void clear();

private:
    struct SpriteData {
        struct A { // TODO
            TextureHandle textureHandle;
            sf::Image image;
            rbp::Rect rect;
            bool vacant = true;
        };

        Multisprite multisprite;
        std::vector<A> subspriteData;
    };

    std::vector<sf::Texture> _textures;
    std::vector<SpriteData> _indexedSprites;
    std::unordered_map<std::string, SpriteData> _mappedSprites;
};


} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_SPRITE_MANAGER_HPP