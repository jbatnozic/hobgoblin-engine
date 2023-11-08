project("Hobgoblin.GSL.Test")

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME} 
  "GSL_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin.GSL"
  
    # External
    "gtest::gtest"
)
