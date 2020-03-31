
#include <Hobgoblin/Graphics/Sprite_manager.hpp>

#include <Hobgoblin/Utility/MaxRectsBinPack.hpp> // TODO Temp.
#include <Hobgoblin/Utility/Rect.h> // TODO Temp.

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

TextureHandle SpriteManager::addTexture(PZInteger width, PZInteger height) {
    _textures.emplace_back();
    if (!_textures.back().create(static_cast<unsigned>(width), static_cast<unsigned>(height))) {
        // TODO Handle error properly
        assert(false && "Could not create texture");
    }
    return static_cast<TextureHandle>(_textures.size() - 1);
}

// Loading from file:
void SpriteManager::loadFromFile(TextureHandle textureHandle, PZInteger spriteIndex,
                                 PZInteger subspriteIndex, const std::string& filePath) {
    assert(textureHandle < static_cast<TextureHandle>(_textures.size()));

    if (spriteIndex >= stopz(_indexedSprites.size())) {
        _indexedSprites.resize(pztos(spriteIndex + 1));
    }
    auto& spriteData = _indexedSprites[pztos(spriteIndex)];

    if (subspriteIndex >= stopz(spriteData.subspriteData.size())) {
        spriteData.subspriteData.resize(pztos(subspriteIndex + 1));
    }
    auto& subspriteData = spriteData.subspriteData[pztos(subspriteIndex)];

    ///////////////////////////////////////////////////////////////////////////

    assert(subspriteData.vacant == true);
    
    if (!subspriteData.image.loadFromFile(filePath)) {
        // TODO Handle error properly
        assert(false && "Could not load image");
    }

    subspriteData.textureHandle = textureHandle;
    subspriteData.vacant = false;
}

void SpriteManager::loadFromFile(TextureHandle texture, PZInteger spriteIndex,
                                 AutoIndexType, const std::string& filePath) {

}

void SpriteManager::loadFromFile(TextureHandle texture, std::string spriteName,
                                 PZInteger subspriteIndex, const std::string& filePath) {

}

void SpriteManager::loadFromFile(TextureHandle texture, std::string spriteName,
                                 AutoIndexType, const std::string& filePath) {

}

// Loading from directory:
void SpriteManager::loadFromDirectory(TextureHandle texture, PZInteger spriteIndex, const std::string& filePath) {
    
}

void SpriteManager::loadFromDirectory(TextureHandle texture, std::string spriteName, const std::string& filePath) {

}

// Loading from memory:
// TODO loadFromMemory

// Search loaded sprites:
Multisprite SpriteManager::getSprite(PZInteger spriteIndex) const {
    return Multisprite{};
}

Multisprite SpriteManager::getSprite(const std::string& spriteName) const {
    return Multisprite{};
}

// Other:
void SpriteManager::finalize() {
    std::vector<std::vector<SpriteData::A*>> texturePackRequests;
    texturePackRequests.resize(_textures.size());

    // Get all pack requests:
    for (auto& sprite : _indexedSprites) {
        for (auto& subsprite : sprite.subspriteData) {
            if (!subsprite.vacant) {
                texturePackRequests[static_cast<std::size_t>(subsprite.textureHandle)].push_back(&subsprite);
            }
        }
    }

    // Try to fulfill all requests:
    for (PZInteger i = 0; i < stopz(texturePackRequests.size()); i += 1) {
        auto& requestVec = texturePackRequests[pztos(i)];
        auto& texture = _textures[pztos(i)];

        std::vector<rbp::RectSize> rectSizes;
        for (auto& subsprite : requestVec) {
            rbp::RectSize rectSize;
            rectSizes.push_back(rbp::RectSize{(int)subsprite->image.getSize().x,
                                              (int)subsprite->image.getSize().y});
        }

        std::vector<rbp::Rect> placedRects;
        rbp::MaxRectsBinPack packer(texture.getSize().x, texture.getSize().y);
        packer.Insert(rectSizes, placedRects, false, rbp::MaxRectsBinPack::RectBestAreaFit); // TODO No verification is done!

        for (PZInteger j = 0; j < stopz(requestVec.size()); j += 1) {
            auto& subsprite = requestVec[j];
            subsprite->rect = placedRects[j];
        }
    }

}

void SpriteManager::clear() {

}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>