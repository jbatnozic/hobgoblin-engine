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
    # Foundation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S01_Common"
    "Hobgoblin_L00_S01_GSL"
    "Hobgoblin_L00_S02_HGExcept"
    "Hobgoblin_L00_S03_Logging"

    # Principals
    "Hobgoblin_L02_S00_Input"
    "Hobgoblin_L02_S00_Window"
    "Hobgoblin_L02_S01_Graphics"

    # External
    "GLEW::GLEW"
    "rmlui::rmlui"
    "sfml::sfml"
)

add_library("Hobgoblin_L02_S02_RmlUi" ALIAS ${COMPONENT_NAME})
