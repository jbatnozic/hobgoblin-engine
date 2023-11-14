project("Hobgoblin.Graphics.ManualTest")

add_executable(${PROJECT_NAME}
    "Graphics_manual_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
  "Hobgoblin.Common"
  "Hobgoblin.Graphics"
  "Hobgoblin.Math"
  "Hobgoblin.Utility"
  "sfml::sfml"
)
