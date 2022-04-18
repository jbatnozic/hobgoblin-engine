# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.Utility")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Dynamic_bitset.cpp"
  "Source/Packet.cpp"
  "Source/Serialization.cpp"
  "Source/Time_utils.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
  "Hobgoblin.Private"
  "Hobgoblin.Common"
  "CONAN_PKG::sfml"
)
