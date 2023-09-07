# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.Config")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Hg_config.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
  "Hobgoblin.Private"
  "Hobgoblin.Common"
  "Hobgoblin.Format"
)
