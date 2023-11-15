# ===== COMPONENT SETUP =====

find_package(SFML CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.Utility")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Dynamic_bitset.cpp"
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
    # Foudnation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S02_HGExcept"

    # Utility
    "Hobgoblin_L01_S00_Common"

    # External
    "sfml::sfml"
)

add_library("Hobgoblin_L01_S02_Utility" ALIAS ${COMPONENT_NAME})
