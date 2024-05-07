# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.Common")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE "Include/")

target_link_libraries(${PROJECT_NAME}
INTERFACE
    # Foundation
    "Hobgoblin_L00_S00_PDef"
)

add_library("Hobgoblin_L00_S01_Common" ALIAS ${PROJECT_NAME})

add_subdirectory("Test")
