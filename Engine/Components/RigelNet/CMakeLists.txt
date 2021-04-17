# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.RigelNet")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Events.cpp"
  "Source/Factories.cpp"
  "Source/Handlermgmt.cpp"
  "Source/Node_interface.cpp"
  "Source/Packet.cpp"
  "Source/Socket_adapter.cpp"
  "Source/Udp_client_impl.cpp"
  "Source/Udp_connector_impl.cpp"
  "Source/Udp_server_impl.cpp"
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