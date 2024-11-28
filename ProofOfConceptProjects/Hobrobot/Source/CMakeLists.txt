project("Hobrobot")

find_package(SFML CONFIG REQUIRED)
find_package(ztcpp CONFIG REQUIRED)

include_directories(.) # For Engine.hpp

add_subdirectory("Common")
add_subdirectory("Context")
# add_subdirectory("Physics")
add_subdirectory("GameObjects")

add_executable(${PROJECT_NAME}
    "_Main.cpp"
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    HobRobot.Common
    HobRobot.Context
    HobRobot.GameObjects

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

    # Principal
    "Hobgoblin.ChipmunkPhysics"
    "Hobgoblin.HGConfig"
    "Hobgoblin.Input"
    "Hobgoblin.QAO"
    "Hobgoblin.RigelNet"
    "Hobgoblin.Window"
    "Hobgoblin.Alvin"
    "Hobgoblin.Graphics"
    "Hobgoblin.RmlUi"
    
    # Overlay
    "SPeMPE"

    # External
    "sfml::sfml"
    "ztcpp::ztcpp"
)
