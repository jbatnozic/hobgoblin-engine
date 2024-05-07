# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

# ===== OVERLAY SETUP =====

find_package(SFML CONFIG REQUIRED)
find_package(ztcpp CONFIG REQUIRED)

set(OVERLAY_NAME "SPeMPE")

# ===== GATHER SOURCES ======

set(OVERLAY_SOURCES
  "Source/Authorization_manager_default.cpp"
  "Source/Context_components.cpp"
  "Source/Event_loop_timing_reporter.cpp"
  "Source/Game_context.cpp"
  "Source/Game_context_flag_validation.cpp"
  "Source/Game_object_bases.cpp"
  "Source/Input_sync_manager_default.cpp"
  "Source/Lobby_backend_manager_default.cpp"
  "Source/Networking_manager_default.cpp"
  "Source/Synced_varmap_manager_default.cpp"
  "Source/Synchronized_object_registry.cpp"
  "Source/Networking_telemetry_reporter.cpp"
  "Source/Sync_flags.cpp"
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
    "Hobgoblin.Common"
    "Hobgoblin.HGExcept"
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

add_subdirectory("Test")

# ===== BUILD EXAMPLES =====

add_subdirectory("Examples")
