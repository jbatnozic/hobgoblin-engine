
#include <Hobgoblin/Graphics/Sprite_loader.hpp>
#include <Hobgoblin/Graphics/Texture_packing.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

TextureHandle SpriteLoader::addTexture(PZInteger width, PZInteger height) {
    _textures.emplace_back();
    if (!_textures.back().create(static_cast<unsigned>(width), static_cast<unsigned>(height))) {
        // TODO Handle error properly
        assert(false && "Could not create texture");
    }
    return static_cast<TextureHandle>(_textures.size() - 1);
}

// Loading from file:
SpriteLoader& SpriteLoader::loadFromFile(TextureHandle textureHandle, PZInteger spriteIndex,
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
        // TODO Handle error properly (exception)
        assert(false && "Could not load image");
    }

    subspriteData.textureHandle = textureHandle;
    subspriteData.vacant = false;
}

SpriteLoader& SpriteLoader::loadFromFile(TextureHandle texture, PZInteger spriteIndex,
                                         AutoIndexType, const std::string& filePath) {

}

SpriteLoader& SpriteLoader::loadFromFile(TextureHandle texture, std::string spriteName,
                                         PZInteger subspriteIndex, const std::string& filePath) {

}

SpriteLoader& SpriteLoader::loadFromFile(TextureHandle texture, std::string spriteName,
                                         AutoIndexType, const std::string& filePath) {

}

// Loading from directory:
SpriteLoader& SpriteLoader::loadFromDirectory(TextureHandle texture, PZInteger spriteIndex,
                                              const std::string& filePath) {
    
}

SpriteLoader& SpriteLoader::loadFromDirectory(TextureHandle texture, std::string spriteName,
                                              const std::string& filePath) {

}

// Loading from memory:
// TODO loadFromMemory

// Search loaded sprites:
Multisprite SpriteLoader::getSprite(PZInteger spriteIndex) const {
    return Multisprite{};
}

Multisprite SpriteLoader::getSprite(const std::string& spriteName) const {
    return Multisprite{};
}

// Other:
void SpriteLoader::finalize() {
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

        std::vector<sf::Image*> images;
        for (auto& subsprite : requestVec) {
            images.push_back(&(subsprite->image));
        }

        auto results = PackTexture(texture, images, TexturePackingHeuristic::BestAreaFit, nullptr);

        for (PZInteger j = 0; j < stopz(requestVec.size()); j += 1) {
            auto& subsprite = requestVec[j];
            //subsprite->
        }
    }

}

void SpriteLoader::clear() {

}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>