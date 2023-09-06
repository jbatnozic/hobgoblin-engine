# ===== COMPONENT SETUP =====

find_package(SFML CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.Input")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/SFML_conversions.hpp"

  "Source/Conversions.cpp"
  "Source/Universal_input_enumeration.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
  "Hobgoblin.Private"
  "Hobgoblin.Common"
  "sfml::sfml"
)

add_subdirectory("Test")
