project("GridWorld.AutomaticTest")

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Active_area_test.cpp"
    "Model_conversions_test.cpp"
    "Spatial_info_test.cpp"
    "World_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    "GridWorld"
    "gtest::gtest"
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    "../Common"
)
