# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project(GridGoblin)

find_package(GLEW CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)

add_library(${PROJECT_NAME}
    "Source/Model/Cell.cpp"
    "Source/Model/Chunk.cpp"

    "Source/Private/Chunk_disk_io_handler_default.cpp"
    "Source/Private/Chunk_spooler_default.cpp"
    "Source/Private/Chunk_storage_handler.cpp"
    "Source/Private/Model_conversions.cpp"

    "Source/World/Active_area.cpp"
    "Source/World/World.cpp"

    "Source/Detail_access.hpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin.Common"
    "Hobgoblin.Format"
    "Hobgoblin.Preprocessor"
    "Hobgoblin.HGExcept"
    "Hobgoblin.Logging"
    "Hobgoblin.Unicode"

    # Utilities
    "Hobgoblin.Math"
    "Hobgoblin.Utility"

    # Principals
    "Hobgoblin.Window"
    "Hobgoblin.Graphics"

    # External
    "GLEW::GLEW"
    "rapidjson"
)

# TODO: Chunk conversions to and from json strings could be even faster if std::string is not used
target_compile_definitions(${PROJECT_NAME}
PRIVATE
    "RAPIDJSON_HAS_STDSTRING=1"
)

target_include_directories(${PROJECT_NAME} PUBLIC "Include/")

add_subdirectory("Test")
