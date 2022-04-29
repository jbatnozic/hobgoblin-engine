add_subdirectory("Modules")
add_subdirectory("Test")
add_subdirectory("DebugExe")

# ===== BUILD AGGREGATE =====

# Build whole engine into one library/object file
# TODO

# TODO What with this?
# if(WIN32)
#   target_link_libraries(${HOBGOBLIN_TARGET_NAME} PRIVATE legacy_stdio_definitions.lib)
# endif()
