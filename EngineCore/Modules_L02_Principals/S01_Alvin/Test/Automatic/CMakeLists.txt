# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.Alvin.AutomaticTest")

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Alvin_automatic_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
  "Hobgoblin.Common"
  "Hobgoblin.ChipmunkPhysics"
  "Hobgoblin.Alvin"

  # External
  "gtest::gtest"
)
