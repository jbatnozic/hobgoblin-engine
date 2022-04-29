# ===== OVERLAY SETUP =====

set(OVERLAY_NAME "SPeMPE")

# ===== GATHER SOURCES ======

set(OVERLAY_SOURCES
  "Source/Context_components.cpp"
  "Source/Game_object_bases.cpp"
  "Source/Input_sync_manager_one.cpp"
  "Source/Keyboard_input.cpp"
  "Source/Game_context.cpp"
  "Source/Networking_manager_one.cpp"
  "Source/Synchronized_object_registry.cpp"
  "Source/Window_manager_one.cpp"

  # "Source/Temp_demo.cpp"
)

# ===== TARGET SETUP =====

add_library(${OVERLAY_NAME} STATIC ${OVERLAY_SOURCES})
target_include_directories(${OVERLAY_NAME} PUBLIC "Include/")

target_link_libraries(${OVERLAY_NAME}
PUBLIC
  "Hobgoblin.Common"
  "Hobgoblin.Graphics"
  "Hobgoblin.QAO"
  "Hobgoblin.RigelNet"
  "Hobgoblin.Utility"
  "CONAN_PKG::sfml"
  "CONAN_PKG::ztcpp"
)

# ===== BUILD TESTS =====

set(OVERLAY_TEST_NAME "${OVERLAY_NAME}.Test")

set(OVERLAY_TEST_SOURCES
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

add_executable("minimal-multiplayer"
  "Examples/Minimal_multiplayer.cpp"
)

target_link_libraries("minimal-multiplayer"
PUBLIC
  "${OVERLAY_NAME}"
)