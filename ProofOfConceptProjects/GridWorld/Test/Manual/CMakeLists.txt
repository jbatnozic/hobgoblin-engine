project("GridWorld.ManualTest")

add_executable(${PROJECT_NAME}
    "GridWorld_manual_test.cpp"
    "Lighting_test.cpp"
    "Spooling_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    "GridWorld"
)

hg_copy_test_assets(${PROJECT_NAME} "Assets")
