# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.Math")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} INTERFACE ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} INTERFACE "Include/")

add_library("Hobgoblin_L01_S01_Math" ALIAS ${COMPONENT_NAME})
