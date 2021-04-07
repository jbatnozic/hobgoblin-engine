# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.Graphics")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Private/Max_rects_bin_pack.cpp"
  "Source/Private/Rbp_rect.cpp"

  "Source/Brush.cpp"
  "Source/Builtin_fonts.cpp"
  "Source/Color.cpp"
  "Source/Multisprite.cpp"
  "Source/Sprite_loader.cpp"
  "Source/Texture_packing.cpp"
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
  "CONAN_PKG::sfml"
)