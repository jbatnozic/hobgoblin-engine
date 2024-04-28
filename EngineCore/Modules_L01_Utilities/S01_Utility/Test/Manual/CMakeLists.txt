# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.Utility.ManualTest")

find_package(SFML CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Flow_field_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Utilities
    "Hobgoblin.Math"
    "Hobgoblin.Utility"

    # External
    "sfml::sfml"
)
