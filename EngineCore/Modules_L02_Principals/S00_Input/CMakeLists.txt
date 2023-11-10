
project("Hobgoblin.Input")

find_package(SFML CONFIG REQUIRED)

set(COMPONENT_SOURCES
)

add_library(${PROJECT_NAME}
STATIC
  "Source/SFML_conversions.hpp"

  "Source/Conversions.cpp"
  "Source/Mouse_button.cpp"
  "Source/Physical_keyboard_key.cpp"
  "Source/Universal_input_enumeration.cpp"
  "Source/Virtual_keyboard_key.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC "Include/")

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S02_HGExcept"

    # Utilities
    "Hobgoblin_L01_S00_Common"

    # External
    "sfml::sfml"
)

add_library("Hobgoblin_L02_S00_Input" ALIAS ${PROJECT_NAME})

add_subdirectory("Test")
