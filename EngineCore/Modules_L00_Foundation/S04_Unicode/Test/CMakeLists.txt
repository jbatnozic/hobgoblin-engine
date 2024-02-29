project("Hobgoblin.Unicode.Test")

find_package(ICU CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(SFML CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Unicode_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin.Unicode"

    # External
    "gtest::gtest"
    "icu::icu"
    "sfml::sfml"
)

hg_copy_test_assets(${PROJECT_NAME} "Assets")
