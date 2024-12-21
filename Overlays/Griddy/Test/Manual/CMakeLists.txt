# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Griddy.ManualTest")

add_executable(${PROJECT_NAME}
    "Default_disk_io_test.cpp"
    "Griddy_manual_test.cpp"
    "Lighting_test.cpp"
    "Openness_test.cpp"
    "Spooling_test.cpp"
    "Storage_handler_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    "Griddy"
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    "../Common"
)

hg_copy_test_assets(${PROJECT_NAME} "Assets")
