# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("SPeMPE.AutomaticTest")

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Autodiff_state_test.cpp"
    "Default_lobby_backend_manager_test.cpp"
    "Default_synced_varmap_manager_test.cpp"
    "Input_sync_manager_test.cpp"
    # "Keyboard_input_test.cpp"
    "SPeMPE_test.cpp"
    "Timing_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
  "SPeMPE"
  "gtest::gtest"
)
