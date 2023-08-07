# ===== COMPONENT SETUP =====

find_package(SFML CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.Graphics")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/MaxRectsBinPack/Max_rects_bin_pack.cpp"
  "Source/MaxRectsBinPack/Rbp_rect.cpp"

  "Source/Builtin_fonts.cpp"
  "Source/Color.cpp"
  "Source/Draw_batcher.cpp"
  "Source/Drawable.cpp"
  "Source/Multisprite.cpp"
  "Source/Sprite_loader.cpp"
  "Source/Sprite.cpp"
  "Source/Texture_packing.cpp"
  "Source/Tga_export.cpp"
  "Source/Views.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
  "Hobgoblin.Private"
  "Hobgoblin.Common"
  "Hobgoblin.Utility"
  "sfml::sfml"
)
