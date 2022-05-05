#ifndef UHOBGOBLIN_GR_SPRITE_LOADER_HPP
#define UHOBGOBLIN_GR_SPRITE_LOADER_HPP

#include <Hobgoblin/Graphics/Multisprite.hpp>
#include <Hobgoblin/Graphics/Texture_packing.hpp>
#include <SFML/Graphics.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

inline constexpr int SUBSPRITE_APPEND = -1;

class TextureBuilder {
public:
	//! aSpriteId - ID to file the (multi)sprite under
	//! aSubspriteIndex - Which subsprite is this? (-1 to append)
	//! aFilePath - location of the image on the file system
	TextureBuilder& addFromFile(PZInteger aSpriteId,
								int aSubspriteIndex,
								const std::string& aFilePath);

	TextureBuilder& addFromFile(const std::string& aSpriteId,
								int aSubspriteIndex,
								const std::string& aFilePath);

	//TextureBuilder& addFromMemory(std::string aSpriteId,
	//							  int aSubspriteIndex,
	//							  ...);

	const sf::Texture* build(TexturePackingHeuristic aHeuristic, float* aOccupancy = nullptr);

private:
	friend class SpriteLoader;

	TextureBuilder(SpriteLoader& aLoader, PZInteger aTexWidth, PZInteger aTexHeight);

	SpriteLoader& _loader;

	std::unique_ptr<sf::Texture> _texture;

	struct E {
		sf::Image image;
		sf::IntRect rect = sf::IntRect{0, 0, 0, 0};
		bool occupied = false;
	};

	struct AddMultispriteRequest {
		std::vector<E> subsprites;
	};

	std::unordered_map<PZInteger, AddMultispriteRequest> _indexedRequests;
	std::unordered_map<std::string, AddMultispriteRequest> _mappedRequests;

	bool _finalized = false;

	void _assertNotFinalized() const;
};

class SpriteLoader {
public:
	SpriteLoader();

	TextureBuilder startTexture(PZInteger aWidth, PZInteger aHeight);

	void removeTexture(sf::Texture& aTexture);

	const MultispriteBlueprint& getBlueprint(PZInteger aSpriteId) const;

	const MultispriteBlueprint& getBlueprint(const std::string& aSpriteId) const;

	void clear();

private:
	friend class TextureBuilder;

	std::vector<std::unique_ptr<sf::Texture>> _textures;

	std::unordered_map<PZInteger, MultispriteBlueprint> _indexedBlueprints;
	std::unordered_map<std::string, MultispriteBlueprint> _mappedBlueprints;

	void _pushBlueprint(PZInteger aSpriteId, MultispriteBlueprint aBlueprint);

	void _pushBlueprint(const std::string& aSpriteId, MultispriteBlueprint aBlueprint);

	void _pushTexture(std::unique_ptr<sf::Texture> aTexture);
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_SPRITE_LOADER_HPP