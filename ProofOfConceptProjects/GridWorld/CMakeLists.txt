
project(GridWorld)

find_package(GLEW CONFIG REQUIRED)

add_library(${PROJECT_NAME}
    "Source/Model/Cell.cpp"

    "Source/Rendering/Dimetric_renderer.cpp"
    "Source/Rendering/Lighting_renderer_2d.cpp"

    "Source/World/World.cpp"

    "Source/Coord_conversion.cpp"
    "Source/Dimetric_transform.cpp"
    "Source/Main.cpp"
    "Source/Spatial_info.cpp"
  )

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin.Common"
    "Hobgoblin.Format"
    "Hobgoblin.GSL"
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
