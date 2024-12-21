# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Griddy.AutomaticTest")

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Active_area_test.cpp"
    "Model_conversions_test.cpp"
    "World_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    "Griddy"
    "gtest::gtest"
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    "../Common"
)
