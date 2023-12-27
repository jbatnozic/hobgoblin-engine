# ===== COMPONENT SETUP =====

find_package(SFML CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.ColDetect")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Qtree_collision_domain.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"

    # Utilities
    "Hobgoblin_L00_S01_Common"
    "Hobgoblin_L01_S02_Utility"

    # External
    "sfml::sfml"
)

add_library("Hobgoblin_L02_S01_ColDetect" ALIAS ${COMPONENT_NAME})
