
cmake_minimum_required(VERSION 3.14)

project(raynor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Collect all sources
set(RAYNOR_SOURCE_ALL
    source/Vector.hpp
    source/Main.cpp
)

# Compilation configuration
add_compile_definitions("SFML_STATIC")
add_compile_definitions("HOBGOBLIN_SHORT_NAMESPACE")

set(RAYNOR_TARGET_NAME
    ${PROJECT_NAME}
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(../Hobgoblin/build/Hobgoblin-x64-Debug-Lib/lib)
else()
    link_directories(../Hobgoblin/build/Hobgoblin-x64-Release-Lib/lib)
endif()

add_executable(${RAYNOR_TARGET_NAME}
    ${RAYNOR_SOURCE_ALL}
)

target_include_directories(${RAYNOR_TARGET_NAME} PUBLIC ../Hobgoblin/include)
target_include_directories(${RAYNOR_TARGET_NAME} PUBLIC source)
target_link_libraries(${RAYNOR_TARGET_NAME} PUBLIC 
    ${CONAN_LIBS}
    "hobgoblin"
)

if(WIN32)
    target_link_libraries(${RAYNOR_TARGET_NAME} PRIVATE legacy_stdio_definitions.lib)
endif()

# set_target_properties(${RAYNOR_TARGET_NAME} PROPERTIES LINK_FLAGS "/PROFILE")