# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

# ===== COMPONENT SETUP =====

find_package(SFML CONFIG REQUIRED)
find_package(ztcpp CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.RigelNet")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
    "Source/Events.cpp"
    "Source/Factories.cpp"
    "Source/Handlermgmt.cpp"
    "Source/Node_interface.cpp"
    "Source/Retransmit_predicate.cpp"
    "Source/Socket_adapter.cpp"
    "Source/Udp_client_impl.cpp"
    "Source/Udp_connector_impl.cpp"
    "Source/Udp_receive_buffer.cpp"
    "Source/Udp_send_buffer.cpp"
    "Source/Udp_server_impl.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

# ZTCpp Integration (TODO: Make it configurable from outside. For now it is mandatory)
target_compile_definitions(${COMPONENT_NAME} PRIVATE "HOBGOBLIN_RN_ZEROTIER_SUPPORT")

target_link_libraries(${COMPONENT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S01_Preprocessor"
    "Hobgoblin_L00_S02_HGExcept"

    # Utilities
    "Hobgoblin_L00_S01_Common"
    "Hobgoblin_L01_S02_Utility"

    # External
    "sfml::sfml"
    "ztcpp::ztcpp"
)

add_library("Hobgoblin_L02_S00_RigelNet" ALIAS ${COMPONENT_NAME})
