# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.GSL")

find_package(Microsoft.GSL CONFIG REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE "Include/")

target_link_libraries(${PROJECT_NAME}
INTERFACE
    # Foundation
    "Hobgoblin_L00_S00_PDef"

    # External
    "Microsoft.GSL::GSL"
)

add_library("Hobgoblin_L00_S01_GSL" ALIAS ${PROJECT_NAME})

add_subdirectory("Test")
