
#include <Hobgoblin/Graphics/Sprite_loader.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

///////////////////////////////////////////////////////////////////////////
// TEXTURE BUILDER                                                       //
///////////////////////////////////////////////////////////////////////////

TextureBuilder::TextureBuilder(SpriteLoader& aLoader, PZInteger aTexWidth, PZInteger aTexHeight)
	: _loader{aLoader}
	, _texture{std::make_unique<sf::Texture>()}
{
	if (!_texture->create(static_cast<unsigned>(aTexWidth), static_cast<unsigned>(aTexHeight))) {
		throw TracedRuntimeError{"TextureBuilder - Could not allocate texture!"};
	}
}

TextureBuilder& TextureBuilder::addFromFile(PZInteger aSpriteId,
                                            int aSubspriteIndex,
                                            const std::string& aFilePath) {
	_assertNotFinalized();

	sf::Image image;
	if (!image.loadFromFile(aFilePath)) {
		throw TracedRuntimeError{"TextureBuilder - Could not load image at " + aFilePath};
	}

	auto& elem = _indexedRequests[aSpriteId];
	if (aSubspriteIndex < 0) {
		elem.subsprites.emplace_back();
		elem.subsprites.back().image = std::move(image);
		elem.subsprites.back().occupied = true;
	}
	else {
		if (elem.subsprites.size() <= static_cast<std::size_t>(aSubspriteIndex)) {
			elem.subsprites.resize(static_cast<std::size_t>(aSubspriteIndex) + 1);
		}
		auto& ss = elem.subsprites[static_cast<std::size_t>(aSubspriteIndex)];
		if (ss.occupied) {
			throw TracedLogicError{"TextureBuilder - This subsprite has already been loaded!"};
		}
		ss.image = std::move(image);
		ss.occupied = true;
	}

	return SELF;
}

TextureBuilder& TextureBuilder::addFromFile(const std::string& aSpriteId,
                                            int aSubspriteIndex,
                                            const std::string& aFilePath) {
	_assertNotFinalized();

	sf::Image image;
	if (!image.loadFromFile(aFilePath)) {
		throw TracedRuntimeError{"TextureBuilder - Could not load image at " + aFilePath};
	}

	auto& elem = _mappedRequests[aSpriteId];
	if (aSubspriteIndex < 0) {
		elem.subsprites.emplace_back();
		elem.subsprites.back().image = std::move(image);
		elem.subsprites.back().occupied = true;
	}
	else {
		if (elem.subsprites.size() <= static_cast<std::size_t>(aSubspriteIndex)) {
			elem.subsprites.resize(static_cast<std::size_t>(aSubspriteIndex) + 1);
		}
		auto& ss = elem.subsprites[static_cast<std::size_t>(aSubspriteIndex)];
		if (ss.occupied) {
			throw TracedLogicError{"TextureBuilder - This subsprite has already been loaded!"};
		}
		ss.image = std::move(image);
		ss.occupied = true;
	}

	return SELF;
}


const sf::Texture* TextureBuilder::build(TexturePackingHeuristic aHeuristic, float* aOccupancy) {
	_assertNotFinalized();

	const sf::Texture* result = _texture.get();

	// Part 1: Pack the texture
	{
		std::vector<E*> subspritePointers;
		std::vector<sf::Image*> imagePointers;

		for (auto& pair : _indexedRequests) {
			for (auto& ss : pair.second.subsprites) {
				subspritePointers.push_back(std::addressof(ss));
				imagePointers.push_back(std::addressof(ss.image));
			}
		}

		for (auto& pair : _mappedRequests) {
			for (auto& ss : pair.second.subsprites) {
				subspritePointers.push_back(std::addressof(ss));
				imagePointers.push_back(std::addressof(ss.image));
			}
		}

		try {
			const auto texRects = PackTexture(*_texture, imagePointers, aHeuristic, aOccupancy);
			assert(subspritePointers.size() == imagePointers.size() &&
				   imagePointers.size() == texRects.size());

			for (std::size_t i = 0; i < texRects.size(); i += 1) {
				subspritePointers[i]->rect = sf::IntRect(
					texRects[i].x,
					texRects[i].y,
					texRects[i].w,
					texRects[i].h
				);
			}
		}
		catch (const TexturePackingError& ex) {
			result = nullptr;
			goto END;
		}
	}

	// Part 2: Make blueprints and push to the loader
	{
		std::vector<sf::IntRect> rects;

		for (auto& pair : _indexedRequests) {
			rects.clear();
			for (const auto& ss : pair.second.subsprites) {
				rects.push_back(ss.rect);
			}
			
			_loader._pushBlueprint(pair.first, {*_texture, rects});
		}

		for (auto& pair : _mappedRequests) {
			rects.clear();
			for (const auto& ss : pair.second.subsprites) {
				rects.push_back(ss.rect);
			}

			_loader._pushBlueprint(pair.first, {*_texture, rects});
		}

		_loader._pushTexture(std::move(_texture));
	}

	// Part 3: Finalize builder
	END: {
		_indexedRequests.clear();
		_mappedRequests.clear();
		_finalized = true;
	}

	return result;
}

void TextureBuilder::_assertNotFinalized() const {
	if (_finalized) {
		throw TracedLogicError("SpriteBuilder - Texture already built and finalized!");
	}
}

///////////////////////////////////////////////////////////////////////////
// SPRITE LOADER                                                         //
///////////////////////////////////////////////////////////////////////////

SpriteLoader::SpriteLoader() = default;

TextureBuilder SpriteLoader::startTexture(PZInteger aWidth, PZInteger aHeight) {
	return TextureBuilder{SELF, aWidth, aHeight};
}

void SpriteLoader::removeTexture(sf::Texture& aTexture) {
	const auto iter = std::remove_if(
		_textures.begin(),
		_textures.end(),
		[&](const std::unique_ptr<sf::Texture>& aTexturePtr) {
			return (aTexturePtr.get() == &aTexture);
		});
	_textures.erase(iter, _textures.end());
}

const MultispriteBlueprint& SpriteLoader::getBlueprint(PZInteger aSpriteId) const {
	const auto iter = _indexedBlueprints.find(aSpriteId);
	if (iter == _indexedBlueprints.end()) {
		throw TracedRuntimeError{"SpriteLoader - No blueprint with this ID was found!"};
	}

	return iter->second;
}

const MultispriteBlueprint& SpriteLoader::getBlueprint(const std::string& aSpriteId) const {
	const auto iter = _mappedBlueprints.find(aSpriteId);
	if (iter == _mappedBlueprints.end()) {
		throw TracedRuntimeError{"SpriteLoader - No blueprint with this ID was found!"};
	}

	return iter->second;
}

void SpriteLoader::clear() {
	_indexedBlueprints.clear();
	_mappedBlueprints.clear();
	_textures.clear();
}

void SpriteLoader::_pushBlueprint(PZInteger aSpriteId, MultispriteBlueprint aBlueprint) {
	const auto iter = _indexedBlueprints.find(aSpriteId);
	if (iter != _indexedBlueprints.end()) {
		throw TracedRuntimeError{"SpriteLoader - Blueprint for sprite with this ID already exists!"};
	}

	_indexedBlueprints.emplace(aSpriteId, std::move(aBlueprint));
}

void SpriteLoader::_pushBlueprint(const std::string& aSpriteId, MultispriteBlueprint aBlueprint) {
	const auto iter = _mappedBlueprints.find(aSpriteId);
	if (iter != _mappedBlueprints.end()) {
		throw TracedRuntimeError{"SpriteLoader - Blueprint for sprite with this ID already exists!"};
	}

	_mappedBlueprints.emplace(aSpriteId, std::move(aBlueprint));
}

void SpriteLoader::_pushTexture(std::unique_ptr<sf::Texture> aTexture) {
	_textures.push_back(std::move(aTexture));
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>