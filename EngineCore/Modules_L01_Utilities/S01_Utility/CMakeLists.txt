# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

# ===== COMPONENT SETUP =====

find_package(base64 CONFIG REQUIRED)
find_package(SFML CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.Utility")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Base64.cpp"
  "Source/Dynamic_bitset.cpp"
  "Source/File_io.cpp"
  "Source/Flow_field_spooler_impl.cpp"
  "Source/Packet.cpp"
  "Source/Randomization.cpp"
  "Source/Serialization.cpp"
  "Source/Time_utils.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S01_Common"
    "Hobgoblin_L00_S02_HGExcept"
    "Hobgoblin_L00_S04_Unicode"

    # Utilities
    "Hobgoblin_L01_S00_Math"

    # External
    "aklomp::base64"
    "sfml::sfml"
)

add_library("Hobgoblin_L01_S02_Utility" ALIAS ${COMPONENT_NAME})

add_subdirectory("Test")
