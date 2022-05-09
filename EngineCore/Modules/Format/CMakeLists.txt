# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.Format")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} INTERFACE ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} INTERFACE "Include/")

target_link_libraries(${COMPONENT_NAME}
INTERFACE
  "CONAN_PKG::fmt"
)
