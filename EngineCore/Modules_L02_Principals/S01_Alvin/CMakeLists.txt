# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.Alvin")

add_library(${PROJECT_NAME} STATIC
    # "Alvin.cpp"

    "Source/Alvin_dummy.cpp"
    "Source/Space.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC "Include/")
target_include_directories(${PROJECT_NAME} PUBLIC "Include/Hobgoblin/Private/")

target_link_libraries(${PROJECT_NAME} PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S01_Common"

    # Utilities
    "Hobgoblin_L01_S00_Math"

    # Principals
    "Hobgoblin_L02_S00_ChipmunkPhysics"
)

add_library("Hobgoblin_L02_S01_Alvin" ALIAS ${PROJECT_NAME})

add_subdirectory("Test")
