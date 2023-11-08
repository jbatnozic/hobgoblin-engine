# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.QAO")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/base.cpp"
  "Source/id.cpp"
  "Source/orderer.cpp"
  "Source/Priority_resolver.cpp"
  "Source/Priority_resolver2.cpp"
  "Source/registry.cpp"
  "Source/runtime.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"

    # Utilities
    "Hobgoblin_L01_S00_Common"
    "Hobgoblin_L01_S02_Utility"
)

add_library("Hobgoblin_L02_S00_QAO" ALIAS ${COMPONENT_NAME})
