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
  "Hobgoblin.Format"
  "Hobgoblin.Preprocessor"
  "Hobgoblin.Private"
)
