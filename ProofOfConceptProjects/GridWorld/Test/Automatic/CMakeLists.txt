project("GridWorld.AutomaticTest")

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Conversions_test.cpp"
    "Spatial_info_test.cpp"
 )

target_link_libraries(${PROJECT_NAME}
PUBLIC
    "GridWorld"
    "gtest::gtest"
)
