# ===== PROJECT SETUP =====

cmake_minimum_required(VERSION 3.23)

project("Hobgoblin" VERSION 0.1 LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ===== IMPORT CUSTOM FUNCTIONS =====

include("CMake/hg_copy_test_assets.cmake")

# ===== ADD ALL THE PARTS =====

# In Debug builds, define "UHOBGOBLIN_DEBUG" macro for all targets.
add_compile_definitions($<$<CONFIG:Debug>:UHOBGOBLIN_DEBUG>)

# This will make the modules which compile to Windows DLLs
# export their symbols properly.
add_compile_definitions("UHOBGOBLIN_EXPORT")

add_subdirectory("EngineCore")
add_subdirectory("Overlays")

add_subdirectory("ProofOfConceptProjects")
