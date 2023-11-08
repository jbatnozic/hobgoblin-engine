# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.HGConfig")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Hg_config.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S01_Format"

    # Utilities
    "Hobgoblin_L01_S00_Common"
)

add_library("Hobgoblin_L02_S00_HGConfig" ALIAS ${COMPONENT_NAME})
