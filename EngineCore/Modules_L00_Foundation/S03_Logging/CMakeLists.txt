# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.Logging")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Logging.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} SHARED ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S01_Format"
    "Hobgoblin_L00_S01_Preprocessor"
)

add_library("Hobgoblin_L00_S03_Logging" ALIAS ${COMPONENT_NAME})
