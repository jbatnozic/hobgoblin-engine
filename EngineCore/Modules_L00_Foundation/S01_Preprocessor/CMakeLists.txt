# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.Preprocessor")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} INTERFACE ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} INTERFACE "Include/")

add_library("Hobgoblin_L00_S01_Preprocessor" ALIAS ${COMPONENT_NAME})
