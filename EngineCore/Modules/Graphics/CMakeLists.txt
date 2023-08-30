# ===== COMPONENT SETUP =====

find_package(SFML CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.Graphics")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/MaxRectsBinPack/Max_rects_bin_pack.cpp"
  "Source/MaxRectsBinPack/Rbp_rect.cpp"

  "Source/Multiview_rendertarget_adapter.hpp"
  "Source/SFML_conversions.hpp"

  "Source/Builtin_fonts.cpp"
  "Source/Color.cpp"
  "Source/Cursor.cpp"
  "Source/Draw_batcher.cpp"
  "Source/Drawable.cpp"
  "Source/Image.cpp"
  # "Source/Multisprite.cpp"
  "Source/Multiview_rendertarget_adapter.cpp"
  "Source/Render_target.cpp"
  "Source/Shader.cpp"
  "Source/Shape.cpp"
  "Source/Sprite.cpp"
  # "Source/Sprite_loader.cpp"
  "Source/Texture.cpp"
  "Source/Texture_packing.cpp"
  "Source/Tga_export.cpp"
  "Source/Transform.cpp"
  "Source/Vertex_array.cpp"
  "Source/Vertex_buffer.cpp"
  "Source/Video_mode.cpp"
  "Source/View.cpp"
  "Source/Views.cpp"
  "Source/Window.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
  "Hobgoblin.Private"
  "Hobgoblin.Common"
  "Hobgoblin.Math"
  "Hobgoblin.Utility"
  "sfml::sfml"
)

add_subdirectory("Test")
