# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.GSL.Test")

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME} 
  "GSL_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin.GSL"
  
    # External
    "gtest::gtest"
)
