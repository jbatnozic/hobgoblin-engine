# ===== COMPONENT SETUP =====

find_package(SFML CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.Window")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Event_conversion.hpp"
  "Source/SFML_conversions.hpp"

  "Source/Cursor.cpp"
  "Source/Video_mode.cpp"
  "Source/Window.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S01_Common"
    "Hobgoblin_L00_S02_HGExcept"

    # Utilities
    "Hobgoblin_L01_S00_Math"
    "Hobgoblin_L01_S02_Utility"

    # Principals
    "Hobgoblin_L02_S00_Input"

    # External
    "sfml::sfml"
)

add_library("Hobgoblin_L02_S00_Window" ALIAS ${COMPONENT_NAME})

add_subdirectory("Test")
