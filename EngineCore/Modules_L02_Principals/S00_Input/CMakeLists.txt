# ===== COMPONENT SETUP =====

find_package(SFML CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.Input")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/SFML_conversions.hpp"

  "Source/Conversions.cpp"
  "Source/Mouse_button.cpp"
  "Source/Physical_keyboard_key.cpp"
  "Source/Universal_input_enumeration.cpp"
  "Source/Virtual_keyboard_key.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"

    # Utilities
    "Hobgoblin_L01_S00_Common"

    # External
    "sfml::sfml"
)

add_library("Hobgoblin_L02_S00_Input" ALIAS ${COMPONENT_NAME})

add_subdirectory("Test")
