# ===== OVERLAY SETUP =====

set(OVERLAY_NAME "SPeMPE")

# ===== GATHER SOURCES ======

set(OVERLAY_SOURCES
  "Source/Authorization_manager_default.cpp"
  "Source/Context_components.cpp"
  "Source/Game_object_bases.cpp"
  "Source/Input_sync_manager_one.cpp"
  "Source/Keyboard_input.cpp"
  "Source/Lobby_manager_default.cpp"
  "Source/Mouse_input.cpp"
  "Source/Game_context.cpp"
  "Source/Networking_manager_one.cpp"
  "Source/Synced_varmap_manager_default.cpp"
  "Source/Synchronized_object_registry.cpp"
  "Source/Window_manager_one.cpp"
)

# ===== TARGET SETUP =====

add_library(${OVERLAY_NAME} STATIC ${OVERLAY_SOURCES})
target_include_directories(${OVERLAY_NAME} PUBLIC "Include/")

target_link_libraries(${OVERLAY_NAME}
PUBLIC
  "Hobgoblin.Common"
  "Hobgoblin.Graphics"
  "Hobgoblin.Logging"
  "Hobgoblin.QAO"
  "Hobgoblin.RigelNet"
  "Hobgoblin.RmlUi"
  "Hobgoblin.Utility"
  "CONAN_PKG::sfml"
  "CONAN_PKG::ztcpp"
)

# ===== BUILD TESTS =====

set(OVERLAY_TEST_NAME "${OVERLAY_NAME}.Test")

set(OVERLAY_TEST_SOURCES
  "Test/Default_lobby_manager_test.cpp"
  "Test/Default_synced_varmap_manager_test.cpp"
  "Test/Input_sync_manager_test.cpp"
  "Test/Keyboard_input_test.cpp"
  "Test/SPeMPE_test.cpp"
)

add_executable(${OVERLAY_TEST_NAME} ${OVERLAY_TEST_SOURCES})

target_link_libraries(${OVERLAY_TEST_NAME}
PUBLIC
  "${OVERLAY_NAME}"
  "CONAN_PKG::gtest"
)

# ===== BUILD EXAMPLES =====

add_subdirectory("Examples")
