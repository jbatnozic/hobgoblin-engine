project("Hobgoblin.Window.ManualTest")

add_executable(${PROJECT_NAME}
    "Window_manual_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
  "Hobgoblin.Private"
  "Hobgoblin.Common"
  "Hobgoblin.Window"
  "Hobgoblin.Math"
  "Hobgoblin.Utility"
  "sfml::sfml"
)
