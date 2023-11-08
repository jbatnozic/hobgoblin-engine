# ===== OVERLAY SETUP =====

find_package(SFML CONFIG REQUIRED)
find_package(ztcpp CONFIG REQUIRED)

set(OVERLAY_NAME "SPeMPE")

# ===== GATHER SOURCES ======

set(OVERLAY_SOURCES
  "Source/Authorization_manager_default.cpp"
  "Source/Context_components.cpp"
  "Source/Game_context.cpp"
  "Source/Game_context_verification.cpp"
  "Source/Game_object_bases.cpp"
  "Source/Input_sync_manager_default.cpp"
  "Source/Lobby_backend_manager_default.cpp"
  "Source/Networking_manager_default.cpp"
  "Source/Synced_varmap_manager_default.cpp"
  "Source/Synchronized_object_registry.cpp"
  "Source/Sync_details.cpp"
  "Source/Window_frame_input_view.cpp"
  "Source/Window_input_tracker.cpp"
  "Source/Window_manager_default.cpp"
)

# ===== TARGET SETUP =====

add_library(${OVERLAY_NAME} STATIC ${OVERLAY_SOURCES})
target_include_directories(${OVERLAY_NAME} PUBLIC "Include/")

target_link_libraries(${OVERLAY_NAME}
PUBLIC
    # Foundation
    "Hobgoblin.GSL"
    "Hobgoblin.Logging"

    # Utilities
    "Hobgoblin.Common"
    "Hobgoblin.Math"
    "Hobgoblin.Utility"

    # Principal
    "Hobgoblin.Input"
    "Hobgoblin.QAO"
    "Hobgoblin.RigelNet"
    "Hobgoblin.RmlUi"
    "Hobgoblin.Graphics"
    
    # External
    "sfml::sfml"
    "ztcpp::ztcpp"
)

# ===== BUILD TESTS =====

find_package(GTest CONFIG REQUIRED)

set(OVERLAY_TEST_NAME "${OVERLAY_NAME}.Test")

set(OVERLAY_TEST_SOURCES
  "Test/Autodiff_state_test.cpp"
  "Test/Default_lobby_backend_manager_test.cpp"
  "Test/Default_synced_varmap_manager_test.cpp"
  "Test/Input_sync_manager_test.cpp"
  # "Test/Keyboard_input_test.cpp"
  "Test/SPeMPE_test.cpp"
)

add_executable(${OVERLAY_TEST_NAME} ${OVERLAY_TEST_SOURCES})

target_link_libraries(${OVERLAY_TEST_NAME}
PUBLIC
  "${OVERLAY_NAME}"
  "gtest::gtest"
)

# ===== BUILD EXAMPLES =====

add_subdirectory("Examples")
