# ===== PROJECT SETUP =====

cmake_minimum_required(VERSION 3.14)

project("Hobgoblin" VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ===== CONAN INTEGRATION =====

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

# ===== ADD MODULES =====

add_subdirectory("Components")
add_subdirectory("Test")
add_subdirectory("DebugExe")

# ===== BUILD AGGREGATE =====

# Build whole engine into one library/object file
# TODO

# TODO What with this?
# if(WIN32)
#   target_link_libraries(${HOBGOBLIN_TARGET_NAME} PRIVATE legacy_stdio_definitions.lib)
# endif()