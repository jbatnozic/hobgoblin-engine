find_package(SFML CONFIG REQUIRED)
find_package(ztcpp CONFIG REQUIRED)

project("HobRobot.GameObjects")

add_library(${PROJECT_NAME} STATIC
    "Source/Limb.cpp"
    "Source/Player_robot.cpp"
    "Source/Single_shape_body.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC "./Interface/")

target_link_libraries(${PROJECT_NAME}
PUBLIC
    HobRobot.Context
    
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
