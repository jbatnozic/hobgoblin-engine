# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

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
    # External
    "fmt::fmt"
)

add_library("Hobgoblin_L00_S01_Format" ALIAS ${PROJECT_NAME})
