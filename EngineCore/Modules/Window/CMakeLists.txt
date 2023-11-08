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
  "Hobgoblin.Private"
  "Hobgoblin.Common"
  "Hobgoblin.Math"
  "Hobgoblin.Utility"
  "Hobgoblin.Input"
  "sfml::sfml"
)

add_subdirectory("Test")
