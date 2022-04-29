cmake_minimum_required(VERSION 3.1)
project("Hobgoblin.Test.Package" CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(${PROJECT_NAME}
    "Hobgoblin_test_package.cpp"
)
target_link_libraries(${PROJECT_NAME} 
    ${CONAN_LIBS}
)
