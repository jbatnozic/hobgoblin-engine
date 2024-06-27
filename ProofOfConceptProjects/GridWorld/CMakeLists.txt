
project(GridWorld)

find_package(GLEW CONFIG REQUIRED)

add_library(${PROJECT_NAME}
    "Source/Model/Cell.cpp"
    "Source/Model/Chunk_spooler_default.cpp"

    "Source/Rendering/Dimetric_renderer.cpp"
    "Source/Rendering/Lighting_renderer_2d.cpp"
    "Source/Rendering/Line_of_sight_renderer_2d.cpp"
    "Source/Rendering/OpenGL_helpers.cpp"
    "Source/Rendering/OpenGL_helpers.hpp"

    "Source/World/World.cpp"

    "Source/Coord_conversion.cpp"
    "Source/Detail_access.hpp"
    "Source/Dimetric_transform.cpp"
    "Source/Main.cpp"
    "Source/Spatial_info.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin.Common"
    "Hobgoblin.Format"
    "Hobgoblin.Preprocessor"
    "Hobgoblin.HGExcept"
    "Hobgoblin.Logging"
    "Hobgoblin.Unicode"

    # Utilities
    "Hobgoblin.Math"
    "Hobgoblin.Utility"

    # Principals
    "Hobgoblin.Window"
    "Hobgoblin.Graphics"

    # External
    "GLEW::GLEW"
)

target_include_directories(${PROJECT_NAME} PUBLIC "Include/")

add_subdirectory("Test")
