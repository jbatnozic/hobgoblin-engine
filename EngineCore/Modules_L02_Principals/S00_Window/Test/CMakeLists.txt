# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.Window.ManualTest")

add_executable(${PROJECT_NAME}
    "Window_manual_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
  "Hobgoblin.Common"
  "Hobgoblin.Window"
  "Hobgoblin.Math"
  "Hobgoblin.Utility"
  "sfml::sfml"
)
