# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.Utility.AutomaticTest")

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Packet_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Utilities
    "Hobgoblin.Utility"

    # External
    "gtest::gtest"
)
