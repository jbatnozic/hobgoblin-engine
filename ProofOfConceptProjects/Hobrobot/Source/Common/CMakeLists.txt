find_package(SFML CONFIG REQUIRED)
find_package(ztcpp CONFIG REQUIRED)

project("HobRobot.Common")

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE "./Interface/")

target_link_libraries(${PROJECT_NAME}
INTERFACE
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
