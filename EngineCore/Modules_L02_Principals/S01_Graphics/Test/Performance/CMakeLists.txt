# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.Graphics.PerformanceTest")

add_executable(${PROJECT_NAME}
    "Batching_performance_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
  "Hobgoblin.Common"
  "Hobgoblin.Graphics"
  "Hobgoblin.Math"
  "Hobgoblin.Utility"
  "sfml::sfml"
)

hg_copy_test_assets(${PROJECT_NAME} "Assets")
