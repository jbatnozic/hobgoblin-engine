
#include <Hobgoblin/Graphics/Sprite_loader.hpp>
#include <Hobgoblin/Graphics/Texture_packing.hpp>
#include <Hobgoblin/Utility/Exceptions.hpp>

#include <cassert>
#include <filesystem>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

TextureHandle SpriteLoader::addTexture(PZInteger width, PZInteger height) {
    assertNotFinalized();

    _textures.emplace_back();
    if (!_textures.back().create(static_cast<unsigned>(width), static_cast<unsigned>(height))) {
        throw util::TracedRuntimeError("Could not create texture.");
    }
    return static_cast<TextureHandle>(_textures.size() - 1);
}

sf::Texture& SpriteLoader::getTexture(TextureHandle textureHandle) {
    auto textureIndex = static_cast<std::size_t>(textureHandle);
    return _textures[textureIndex];
}

// Loading from file:
SpriteLoader& SpriteLoader::loadFromFile(TextureHandle textureHandle, PZInteger spriteIndex,
                                         PZInteger subspriteIndex, const std::string& filePath) {
    assertNotFinalized();

    auto textureIndex = static_cast<std::size_t>(textureHandle);
    assert(textureIndex < _textures.size());

    auto spriteIndexSz = static_cast<std::size_t>(spriteIndex);
    if (spriteIndexSz >= _indexedSprites.size()) {
        _indexedSprites.resize(spriteIndexSz + 1);
    }
    auto& spriteData = _indexedSprites[spriteIndexSz];

    auto packRequestIndex = static_cast<std::size_t>(subspriteIndex);
    if (packRequestIndex >= spriteData.packRequests.size()) {
        spriteData.packRequests.resize(packRequestIndex + 1);
    }
    auto& packRequest = spriteData.packRequests[packRequestIndex];

    ///////////////////////////////////////////////////////////////////////////

    assert(packRequest.unused == true);
    
    if (!packRequest.image.loadFromFile(filePath)) {
        throw util::TracedRuntimeError("Could not load image: " + filePath);
    }

    packRequest.textureHandle = textureHandle;
    packRequest.unused = false;

    return Self;
}

SpriteLoader& SpriteLoader::loadFromFile(TextureHandle textureHandle, PZInteger spriteIndex,
                                         AutoIndexType, const std::string& filePath) {
    assertNotFinalized();
    
    auto textureIndex = static_cast<std::size_t>(textureHandle);
    assert(textureIndex < _textures.size());

    auto spriteIndexSz = static_cast<std::size_t>(spriteIndex);
    if (spriteIndexSz >= _indexedSprites.size()) {
        _indexedSprites.resize(spriteIndexSz + 1);
    }
    auto& spriteData = _indexedSprites[spriteIndexSz];

    spriteData.packRequests.resize(spriteData.packRequests.size() + 1);
    auto& packRequest = spriteData.packRequests.back();

    ///////////////////////////////////////////////////////////////////////////

    assert(packRequest.unused == true);

    if (!packRequest.image.loadFromFile(filePath)) {
        throw util::TracedRuntimeError("Could not load image: " + filePath);
    }

    packRequest.textureHandle = textureHandle;
    packRequest.unused = false;

    return Self;
}

SpriteLoader& SpriteLoader::loadFromFile(TextureHandle texture, std::string spriteName,
                                         PZInteger subspriteIndex, const std::string& filePath) {
    assertNotFinalized();
    // TODO
    return Self;
}

SpriteLoader& SpriteLoader::loadFromFile(TextureHandle texture, std::string spriteName,
                                         AutoIndexType, const std::string& filePath) {
    // TODO
    return Self;
}

// Loading from directory:
SpriteLoader& SpriteLoader::loadFromDirectory(TextureHandle texture, PZInteger spriteIndex,
                                              const std::string& filePath) {
    /*std::vector<std::string> filesInDirectory;
    for (auto& entry : std::filesystem::directory_iterator(filePath)) {
        filesInDirectory.push_back(entry.path().c_str());
    }*/
    return Self;
}

SpriteLoader& SpriteLoader::loadFromDirectory(TextureHandle texture, std::string spriteName,
                                              const std::string& filePath) {
    // TODO
    return Self;
}

// Loading from memory:
// TODO loadFromMemory

// Search loaded sprites:
Multisprite SpriteLoader::getSprite(PZInteger spriteIndex) const {
    auto spriteIndexSz = static_cast<std::size_t>(spriteIndex);
    return _indexedSprites[spriteIndexSz].multisprite;
}

Multisprite SpriteLoader::getSprite(const std::string& spriteName) const {
    auto iter = _mappedSprites.find(spriteName);
    if (iter == _mappedSprites.end()) {
        throw util::TracedLogicError("Sprite with name " + spriteName + " was not loaded.");
    }
    return (*iter).second.multisprite;
}

// Other:
void SpriteLoader::finalize(TexturePackingHeuristic heuristic) {
    assertNotFinalized();

    std::vector<std::vector<SpriteData::PackRequest*>> texturePackRequests;
    texturePackRequests.resize(_textures.size()); // Requests are per-texture

    // Get all pack requests for indexed sprites:
    for (auto& spriteData : _indexedSprites) {
        for (auto& packRequest : spriteData.packRequests) {
            if (!packRequest.unused) {
                texturePackRequests[static_cast<std::size_t>(packRequest.textureHandle)].push_back(&packRequest);
            }
        }
    }

    // Get all pack requests for mapped sprites:
    for (auto& pair : _mappedSprites) {
        for (auto& packRequest : pair.second.packRequests) {
            if (!packRequest.unused) {
                texturePackRequests[static_cast<std::size_t>(packRequest.textureHandle)].push_back(&packRequest);
            }
        }
    }

    // Try to fulfill all requests:
    for (std::size_t i = 0; i < texturePackRequests.size(); i += 1) {
        auto& currentTexture = _textures[i];
        auto& requestsForCurrentTexture = texturePackRequests[i];

        std::vector<sf::Image*> images;
        for (auto& request : requestsForCurrentTexture) {
            images.push_back(&(request->image));
        }

        auto results = PackTexture(currentTexture, images, heuristic, nullptr); // TODO Use occupancy

        for (PZInteger t = 0; t < stopz(requestsForCurrentTexture.size()); t += 1) {
            auto& subsprite = requestsForCurrentTexture[t];
            subsprite->rextureRect = results[t];
        }
    }

    // Move all generated data into multisprites & clean up requests:
    for (auto& spriteData : _indexedSprites) {
        for (auto& packRequest : spriteData.packRequests) {
            spriteData.multisprite.addSubsprite(getTexture(packRequest.textureHandle), packRequest.rextureRect);
        }
        spriteData.packRequests.clear();
    }

    for (auto& pair : _mappedSprites) {
        for (auto& packRequest : pair.second.packRequests) {
            pair.second.multisprite.addSubsprite(getTexture(packRequest.textureHandle), packRequest.rextureRect);
        }
        pair.second.packRequests.clear();
    }

    // Set finalized flag:
    _isFinalized = true;
}

void SpriteLoader::clear() {
    // TODO
    _isFinalized = false;
}

void SpriteLoader::assertNotFinalized() const {
    if (_isFinalized) {
        throw util::TracedLogicError("Sprite loader was already finalized.");
    }
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>