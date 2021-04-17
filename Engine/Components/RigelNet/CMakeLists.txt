# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.RigelNet")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Event.cpp"
  "Source/Handlermgmt.cpp"
  "Source/Node.cpp"
  "Source/Packet.cpp"
  "Source/Udp_client.cpp"
  "Source/Udp_connector.cpp"
  "Source/Udp_server.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")
target_include_directories(${COMPONENT_NAME} PUBLIC "../../ZTCpp/Include")
target_link_directories(${COMPONENT_NAME} PUBLIC "../../ZTCpp/Lib")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
  "Hobgoblin.Private"
  "Hobgoblin.Common"
  "Hobgoblin.Preprocessor"
  "Hobgoblin.Utility"
  "CONAN_PKG::sfml"
  "ztcpp"
)