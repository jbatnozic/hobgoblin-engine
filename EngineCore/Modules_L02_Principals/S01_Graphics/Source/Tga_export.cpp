// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Graphics/Tga_export.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

void ExportTextureToTgaFile(const Texture& aTexture, const std::string& aFilePath) {
	const auto image = aTexture.copyToImage();
	image.saveToFile(aFilePath);
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

// clang-format on
