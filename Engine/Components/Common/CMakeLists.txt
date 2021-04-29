# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.Common")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Gsl_pointers.cpp"
  "Source/Traced_exceptions.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
  "Hobgoblin.Private"
)