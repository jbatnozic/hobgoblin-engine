# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.RigelNet.AutomaticTest")

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "RigelNet_automatic_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S01_Preprocessor"
    "Hobgoblin_L00_S02_HGExcept"

    # Utilities
    "Hobgoblin_L00_S01_Common"
    "Hobgoblin_L01_S02_Utility"

    # Principals
    "Hobgoblin_L02_S00_RigelNet"

    # External
    "gtest::gtest"
)
