# ===== COMPONENT SETUP =====

find_package(GLEW CONFIG REQUIRED)
find_package(rmlui CONFIG REQUIRED)
find_package(sfml CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.RmlUi")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
    "Source/Context_driver.cpp"
    "Source/Hobgoblin_backend.cpp"
    "Source/RmlUi_preprocessing.cpp"
    "Source/RmlUi_SFML_renderer.cpp"
    "Source/RmlUi_SFML_system.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
  "Hobgoblin.Private"
  "Hobgoblin.Common"
  "Hobgoblin.Logging"
  "Hobgoblin.Graphics"
  "GLEW::GLEW"
  "rmlui::rmlui"
  "sfml::sfml"
)
