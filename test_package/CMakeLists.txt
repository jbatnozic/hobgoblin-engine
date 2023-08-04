cmake_minimum_required(VERSION 3.23)
project("Hobgoblin.Test.Package" CXX)

find_package(hobgoblin CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Hobgoblin_test_package.cpp"
)
target_link_libraries(${PROJECT_NAME} 
    "hobgoblin::hobgoblin"
)
