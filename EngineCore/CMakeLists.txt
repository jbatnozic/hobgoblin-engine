# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

add_subdirectory("Modules_L00_Foundation")
add_subdirectory("Modules_L01_Utilities")
add_subdirectory("Modules_L02_Principals")

add_subdirectory("Test")
add_subdirectory("DebugExe")

# ===== BUILD AGGREGATE =====

# Build whole engine into one library/object file
# TODO
