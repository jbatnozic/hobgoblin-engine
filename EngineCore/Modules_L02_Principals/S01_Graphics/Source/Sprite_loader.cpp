
#include <Hobgoblin/Graphics/Sprite_loader.hpp>

#include <Hobgoblin/Logging.hpp>

#include <cassert>

#include "SFML_conversions.hpp"
#include "SFML_err.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

///////////////////////////////////////////////////////////////////////////
// TEXTURE BUILDER                                                       //
///////////////////////////////////////////////////////////////////////////

namespace {
constexpr auto LOG_ID = "Hobgoblin";
} // namespace

namespace detail {
class TextureBuilderImpl : public SpriteLoader::TextureBuilder {
public:
	TextureBuilderImpl(SpriteLoader& aSpriteLoader, PZInteger aTextureWidth, PZInteger aTextureHeight)
		: _loader{aSpriteLoader}
	{
		_texture = std::make_unique<Texture>();
		_texture->create(aTextureWidth, aTextureHeight);
	}

	NotNull<TextureBuilder*> addSprite(SpriteIdNumerical aSpriteId,
                                       const std::filesystem::path& aFilePath) override {
		_assertNotFinalized();

		if (_indexedRequests.find(aSpriteId) != _indexedRequests.end()) {
			HG_THROW_TRACED(TracedLogicError, 0, "Sprite with this ID ({}) already exists!", aSpriteId);
		}

		auto image = _loadImage(aFilePath);

		auto& req = _indexedRequests[aSpriteId];
		req.subsprites.emplace_back();
		req.subsprites.back().image = std::move(image);
		req.subsprites.back().occupied = true;

		return this;
	}

	NotNull<TextureBuilder*> addSubsprite(SpriteIdNumerical aSpriteId,
										  PZInteger aSubspriteIndex,
										  const std::filesystem::path& aFilePath) override {
		_assertNotFinalized();

		auto image = _loadImage(aFilePath);

		auto& req = _indexedRequests[aSpriteId];
		if (req.subsprites.size() <= pztos(aSubspriteIndex)) {
			req.subsprites.resize(pztos(aSubspriteIndex) + 1);
		}
		auto& subsprite = req.subsprites[pztos(aSubspriteIndex)];
		if (subsprite.occupied) {
			HG_LOG_WARN(LOG_ID, 
						"addSubsprite() overwriting previously loaded subsprite at index "
						"{} of sprite by ID {}.",
						aSubspriteIndex,
						aSpriteId);
		}
		subsprite.image = std::move(image);
		subsprite.occupied = true;

		return this;
	}

	NotNull<TextureBuilder*> addSubsprite(SpriteIdNumerical aSpriteId,
                                          const std::filesystem::path& aFilePath) override {
		_assertNotFinalized();

		auto image = _loadImage(aFilePath);

		auto& req = _indexedRequests[aSpriteId];
		req.subsprites.emplace_back();
		req.subsprites.back().image = std::move(image);
		req.subsprites.back().occupied = true;
		
		return this;
	}

	NotNull<TextureBuilder*> addSprite(const SpriteIdTextual& aSpriteId,
                                       const std::filesystem::path& aFilePath) override {
		_assertNotFinalized();

		if (_mappedRequests.find(aSpriteId) != _mappedRequests.end()) {
			HG_THROW_TRACED(TracedLogicError, 0, "Sprite with this ID ({}) already exists!", aSpriteId);
		}

		auto image = _loadImage(aFilePath);

		auto& req = _mappedRequests[aSpriteId];
		req.subsprites.emplace_back();
		req.subsprites.back().image = std::move(image);
		req.subsprites.back().occupied = true;

		return this;
	}

	NotNull<TextureBuilder*> addSubsprite(const SpriteIdTextual& aSpriteId,
                                          PZInteger aSubspriteIndex,
                                          const std::filesystem::path& aFilePath) override {
		_assertNotFinalized();

		auto image = _loadImage(aFilePath);

		auto& req = _mappedRequests[aSpriteId];
		if (req.subsprites.size() <= pztos(aSubspriteIndex)) {
			req.subsprites.resize(pztos(aSubspriteIndex) + 1);
		}
		auto& subsprite = req.subsprites[pztos(aSubspriteIndex)];
		if (subsprite.occupied) {
			HG_LOG_WARN(LOG_ID, 
						"addSubsprite() overwriting previously loaded subsprite at index "
						"{} of sprite by ID {}.",
						aSubspriteIndex,
						aSpriteId);
		}
		subsprite.image = std::move(image);
		subsprite.occupied = true;

		return this;
	}

	NotNull<TextureBuilder*> addSubsprite(const SpriteIdTextual& aSpriteId,
										  const std::filesystem::path& aFilePath) override {
		_assertNotFinalized();

		auto image = _loadImage(aFilePath);

		auto& req = _mappedRequests[aSpriteId];
		req.subsprites.emplace_back();
		req.subsprites.back().image = std::move(image);
		req.subsprites.back().occupied = true;

		return this;
	}

	const Texture& finalize(TexturePackingHeuristic aHeuristic, float* aOccupancy) override {
		_assertNotFinalized();

		const Texture* result = _texture.get();

		// Part 1: Pack the texture
		{
			std::vector<SubspriteData*> subspritePointers;
			std::vector<Image*>         imagePointers;

			for (auto& pair : _indexedRequests) {
				for (auto& subsprite : pair.second.subsprites) {
					subspritePointers.push_back(std::addressof(subsprite));
					imagePointers.push_back(std::addressof(subsprite.image));
				}
			}

			for (auto& pair : _mappedRequests) {
				for (auto& subsprite : pair.second.subsprites) {
					subspritePointers.push_back(std::addressof(subsprite));
					imagePointers.push_back(std::addressof(subsprite.image));
				}
			}

			// Invoke packing algorithm
			const auto textureRects = PackTexture(*_texture, imagePointers, aHeuristic, aOccupancy);
			assert(subspritePointers.size() == imagePointers.size() &&
				   imagePointers.size() == textureRects.size());

			for (std::size_t i = 0; i < textureRects.size(); i += 1) {
				subspritePointers[i]->rect = textureRects[i];
			}
		}

		// Part 2: Make blueprints and push to the loader
		{
			std::vector<TextureRect> rects;

			for (auto& pair : _indexedRequests) {
				rects.clear();
				for (const auto& subsprite : pair.second.subsprites) {
					rects.push_back(subsprite.rect);
				}

				_loader._pushBlueprint(pair.first, {*_texture, rects});
			}

			for (auto& pair : _mappedRequests) {
				rects.clear();
				for (const auto& subsprite : pair.second.subsprites) {
					rects.push_back(subsprite.rect);
				}

				_loader._pushBlueprint(pair.first, {*_texture, rects});
			}

			_loader._pushTexture(std::move(_texture));
		}

		_indexedRequests.clear();
		_mappedRequests.clear();
		_finalized = true;

		return *result;
	}

	~TextureBuilderImpl() override {
		if (!_finalized) {
			HG_LOG_WARN(LOG_ID, "Non-finalized TextureBuilder being destroyed.");
		}
	}

private:
	SpriteLoader& _loader;

	std::unique_ptr<Texture> _texture;

	struct SubspriteData {
		Image image;
		TextureRect rect;
		bool occupied = false;
	};

	struct AddMultispriteRequest {
		std::vector<SubspriteData> subsprites;
	};

	std::unordered_map<SpriteIdNumerical, AddMultispriteRequest> _indexedRequests;
	std::unordered_map<SpriteIdTextual, AddMultispriteRequest> _mappedRequests;

	bool _finalized = false;

	void _assertNotFinalized() const {
		HG_HARD_ASSERT(!_finalized);
	}

	static Image _loadImage(const std::filesystem::path& aPath) {
		Image image;
		image.loadFromFile(FilesystemPathToSfPath(aPath));
		return image;
	}
};
} // namespace detail


///////////////////////////////////////////////////////////////////////////
// SPRITE LOADER                                                         //
///////////////////////////////////////////////////////////////////////////

std::unique_ptr<SpriteLoader::TextureBuilder> SpriteLoader::startTexture(PZInteger aWidth,
																		 PZInteger aHeight) {
	return std::make_unique<detail::TextureBuilderImpl>(SELF, aWidth, aHeight);
}

void SpriteLoader::removeTexture(Texture& aTextureToRemove) {
	const auto iter = std::remove_if(
		_textures.begin(),
		_textures.end(),
		[&](const std::unique_ptr<Texture>& aElem) {
			return (aElem.get() == &aTextureToRemove);
		});
	_textures.erase(iter, _textures.end());
}

SpriteBlueprint SpriteLoader::getBlueprint(SpriteIdNumerical aSpriteId) const {
	const auto iter = _indexedBlueprints.find(aSpriteId);
	if (iter == _indexedBlueprints.end()) {
		HG_THROW_TRACED(TracedRuntimeError, 0, "No blueprint with this ID ({}) was found!", aSpriteId);
	}

	return iter->second.extractBlueprint(0);
}

SpriteBlueprint SpriteLoader::getBlueprint(const SpriteIdTextual& aSpriteId) const {
	const auto iter = _mappedBlueprints.find(aSpriteId);
	if (iter == _mappedBlueprints.end()) {
		HG_THROW_TRACED(TracedRuntimeError, 0, "No blueprint with this ID ({}) was found!", aSpriteId);
	}

	return iter->second.extractBlueprint(0);
}

const MultispriteBlueprint& SpriteLoader::getMultiBlueprint(SpriteIdNumerical aSpriteId) const {
	const auto iter = _indexedBlueprints.find(aSpriteId);
	if (iter == _indexedBlueprints.end()) {
		HG_THROW_TRACED(TracedRuntimeError, 0, "No blueprint with this ID ({}) was found!", aSpriteId);
	}

	return iter->second;
}

const MultispriteBlueprint& SpriteLoader::getMultiBlueprint(const SpriteIdTextual& aSpriteId) const {
	const auto iter = _mappedBlueprints.find(aSpriteId);
	if (iter == _mappedBlueprints.end()) {
		HG_THROW_TRACED(TracedRuntimeError, 0, "No blueprint with this ID ({}) was found!", aSpriteId);
	}

	return iter->second;
}

void SpriteLoader::clear() {
	_indexedBlueprints.clear();
	_mappedBlueprints.clear();
	_textures.clear();
}

void SpriteLoader::_pushBlueprint(SpriteIdNumerical aSpriteId, MultispriteBlueprint aBlueprint) {
	const auto iter = _indexedBlueprints.find(aSpriteId);
	if (iter != _indexedBlueprints.end()) {
		HG_THROW_TRACED(TracedRuntimeError, 0, "Blueprint for sprite with this ID ({}) already exists!", aSpriteId);
	}

	_indexedBlueprints.emplace(aSpriteId, std::move(aBlueprint));
}

void SpriteLoader::_pushBlueprint(const SpriteIdTextual& aSpriteId, MultispriteBlueprint aBlueprint) {
	const auto iter = _mappedBlueprints.find(aSpriteId);
	if (iter != _mappedBlueprints.end()) {
		HG_THROW_TRACED(TracedRuntimeError, 0, "Blueprint for sprite with this ID ({}) already exists!", aSpriteId);
	}

	_mappedBlueprints.emplace(aSpriteId, std::move(aBlueprint));
}

void SpriteLoader::_pushTexture(std::unique_ptr<Texture> aTexture) {
	_textures.push_back(std::move(aTexture));
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>