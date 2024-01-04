
project(GridWorld)

add_library(${PROJECT_NAME}
    "Source/Isometric_renderer.cpp"
    "Source/Main.cpp"
    "Source/Renderer.cpp"
    "Source/Spatial_info.cpp"
    "Source/Grid_world.cpp" 
    "Source/Coord_conversion.cpp"
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
)

target_include_directories(${PROJECT_NAME} PUBLIC "Include/")

add_subdirectory("Test")
