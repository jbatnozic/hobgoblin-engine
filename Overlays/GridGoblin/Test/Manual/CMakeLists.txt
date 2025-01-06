# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("GridGoblin.ManualTest")

add_executable(${PROJECT_NAME}
    "Default_disk_io_test.cpp"
    "Dimetric_rendering_test.cpp"
    "GridGoblin_manual_test.cpp"
    "Openness_test.cpp"
    "Spooling_test.cpp"
    "Storage_handler_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    "GridGoblin"
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    "../Common"
)

hg_copy_test_assets(${PROJECT_NAME} "Assets")
