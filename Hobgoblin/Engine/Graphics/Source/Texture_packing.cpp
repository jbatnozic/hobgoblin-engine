
#include <Hobgoblin/Graphics/Texture_packing.hpp>
#include <Hobgoblin/Private/Max_rects_bin_pack.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

namespace {

rbp::MaxRectsBinPack::FreeRectChoiceHeuristic ConvertHeuristic(TexturePackingHeuristic h) {
	switch (h) {
	case TexturePackingHeuristic::BestShortSideFit: return rbp::MaxRectsBinPack::RectBestShortSideFit;
	case TexturePackingHeuristic::BestLongSideFit:  return rbp::MaxRectsBinPack::RectBestLongSideFit;
	case TexturePackingHeuristic::BestAreaFit:      return rbp::MaxRectsBinPack::RectBestAreaFit;
	case TexturePackingHeuristic::BottomLeftRule:   return rbp::MaxRectsBinPack::RectBottomLeftRule;
	case TexturePackingHeuristic::ContactPointRule: return rbp::MaxRectsBinPack::RectContactPointRule;
	default:
		assert(0 && "Unreachable");
	}
}

} // namespace

std::vector<util::Rectangle<PZInteger>> PackTexture(sf::Texture& texture, const std::vector<sf::Image*>& images,
													TexturePackingHeuristic heuristic, float* occupancy) {
	// Create texture packer:
	auto textureSize = texture.getSize();
	rbp::MaxRectsBinPack packer{static_cast<int>(textureSize.x), static_cast<int>(textureSize.y)};

	// Get sizes of images to pack:
	std::vector<rbp::RectSize> imageSizes;
	for (auto image : images) {
		auto imageSize = image->getSize();
		imageSizes.push_back(rbp::RectSize{static_cast<int>(imageSize.x), static_cast<int>(imageSize.y)});
	}

	// Run packing algorithm:
	std::vector<rbp::Rect> results = packer.Insert(imageSizes, false, ConvertHeuristic(heuristic));
	std::vector<util::Rectangle<PZInteger>> rv;
	rv.reserve(images.size());

	for (std::size_t i = 0; i < images.size(); i += 1) {
		auto& image = *images[i];
		auto& rect = results[i];

		texture.update(image, static_cast<unsigned>(rect.x), static_cast<unsigned>(rect.y));
		rv.emplace_back(rect.x, rect.y, rect.width, rect.height);
	}

	// Output values:
	if (occupancy != nullptr) {
		*occupancy = packer.Occupancy();
	}

	return rv;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>