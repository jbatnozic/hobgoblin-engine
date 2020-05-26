
cmake_minimum_required(VERSION 3.14)

project(hobgoblin-test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Collect all sources
set(HOBGOBLIN_TEST_SOURCE_ALL
    source/ColDetect_test.cpp
	source/gtest_main.cpp
	source/QAO_serialization_test.cpp
	source/Qao_test.cpp
	source/RigelNet_test.cpp
	source/Util_test.cpp
)

# Compilation configuration
add_compile_definitions("SFML_STATIC")
add_compile_definitions("HOBGOBLIN_SHORT_NAMESPACE")

set(HOBGOBLIN_TEST_TARGET_NAME
    ${PROJECT_NAME}.exe
)

link_directories(../Hobgoblin/build/Hobgoblin-x64-Debug-Lib/lib)

add_executable(${HOBGOBLIN_TEST_TARGET_NAME}
    ${HOBGOBLIN_TEST_SOURCE_ALL}
)

target_include_directories(${HOBGOBLIN_TEST_TARGET_NAME} PUBLIC ../Hobgoblin/include)
target_include_directories(${HOBGOBLIN_TEST_TARGET_NAME} PUBLIC source)
target_link_libraries(${HOBGOBLIN_TEST_TARGET_NAME} PUBLIC 
    ${CONAN_LIBS}
    "hobgoblin"
)