# ===== COMPONENT SETUP =====

find_package(SFML CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.Graphics")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/MaxRectsBinPack/Max_rects_bin_pack.cpp"
  "Source/MaxRectsBinPack/Rbp_rect.cpp"

  "Source/Draw_bridge.hpp"
  "Source/Multiview_rendertarget_adapter.cpp"
  "Source/Multiview_rendertarget_adapter.hpp"
  "Source/SFML_conversions.hpp"
  "Source/SFML_err.hpp"
  "Source/SFML_rendertarget_adapter.cpp"
  "Source/SFML_rendertarget_adapter.hpp"

  "Source/Builtin_fonts.cpp"
  "Source/Circle_shape.cpp"
  "Source/Draw_batcher.cpp"
  "Source/Image.cpp"
  "Source/Multisprite.cpp"
  "Source/Multisprite_blueprint.cpp"
  "Source/Rectangle_shape.cpp"
  "Source/Render_states.cpp"
  "Source/Render_texture.cpp"
  "Source/Render_window.cpp"
  "Source/Shader.cpp"
  "Source/Shape.cpp"
  "Source/Sprite.cpp"
  "Source/Sprite_blueprint.cpp"
  "Source/Sprite_loader.cpp"
  "Source/Texture.cpp"
  "Source/Texture_packing.cpp"
  "Source/Tga_export.cpp"
  "Source/Transform.cpp"
  "Source/Transformable_data.cpp"
  "Source/Vertex_array.cpp"
  "Source/Vertex_buffer.cpp"
  "Source/View.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S01_GSL"
    "Hobgoblin_L00_S02_HGExcept"
    "Hobgoblin_L00_S03_Logging"

    # Utilities
    "Hobgoblin_L01_S00_Common"
    "Hobgoblin_L01_S01_Math"
    "Hobgoblin_L01_S02_Utility"

    # Principals
    "Hobgoblin_L02_S00_Window"

    # External
    "sfml::sfml"
)

add_library("Hobgoblin_L02_S01_Graphics" ALIAS ${COMPONENT_NAME})

add_subdirectory("Test")
