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
