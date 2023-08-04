# ===== COMPONENT SETUP =====

project("Hobgoblin.Format")

find_package(fmt CONFIG REQUIRED)

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
)

# ===== TARGET SETUP =====

add_library(${PROJECT_NAME} INTERFACE ${COMPONENT_SOURCES})
target_include_directories(${PROJECT_NAME} INTERFACE "Include/")

target_link_libraries(${PROJECT_NAME}
INTERFACE
  "fmt::fmt"
)
