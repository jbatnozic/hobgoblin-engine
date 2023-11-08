#ifndef UHOBGOBLIN_GR_TEXTURE_PACKING_HPP
#define UHOBGOBLIN_GR_TEXTURE_PACKING_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Image.hpp>
#include <Hobgoblin/Graphics/Texture.hpp>
#include <Hobgoblin/Graphics/Texture_rect.hpp>

#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

enum class TexturePackingHeuristic {
	BestShortSideFit,
	BestLongSideFit,
	BestAreaFit,
	BottomLeftRule,
	ContactPointRule
};

class TexturePackingError : public TracedRuntimeError {
public:
	using TracedRuntimeError::TracedRuntimeError;
};

std::vector<TextureRect> PackTexture(Texture& texture, 
									 const std::vector<Image*>& images,
                                     TexturePackingHeuristic heuristic, 
									 float* occupancy = nullptr);

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_TEXTURE_PACKING_HPP