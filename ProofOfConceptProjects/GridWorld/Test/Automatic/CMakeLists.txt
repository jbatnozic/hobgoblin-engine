project("GridWorld.UnitTest")

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Spatial_info_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    "GridWorld"
    "gtest::gtest"
)
