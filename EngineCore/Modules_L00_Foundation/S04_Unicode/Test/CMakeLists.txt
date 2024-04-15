# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.Unicode.Test")

find_package(ICU CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Unicode_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin.Unicode"

    # External
    "gtest::gtest"
    "icu::icu"
)

hg_copy_test_assets(${PROJECT_NAME} "Assets")
