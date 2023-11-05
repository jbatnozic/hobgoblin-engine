# ===== COMPONENT SETUP =====

find_package(GLEW CONFIG REQUIRED)
find_package(rmlui CONFIG REQUIRED)
find_package(SFML CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.RmlUi")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
    "Source/Context_driver.cpp"
    "Source/Hobgoblin_backend.cpp"
    "Source/RmlUi_Hobgoblin_renderer.cpp"
    "Source/RmlUi_Hobgoblin_system.cpp"
    "Source/RmlUi_preprocessing.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
  "Hobgoblin.Private"
  "Hobgoblin.Common"
  "Hobgoblin.Input"
  "Hobgoblin.Logging"
  "Hobgoblin.Graphics"
  "Hobgoblin.Window"
  "GLEW::GLEW"
  "rmlui::rmlui"
  "sfml::sfml"
)
