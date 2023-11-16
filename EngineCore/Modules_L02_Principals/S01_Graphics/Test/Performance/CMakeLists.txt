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
