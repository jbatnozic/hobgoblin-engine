project("Hobgoblin.Input.ManualTest")

add_executable(${PROJECT_NAME}
    "Input_manual_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
  "Hobgoblin.Input"
  "sfml::sfml"
)
