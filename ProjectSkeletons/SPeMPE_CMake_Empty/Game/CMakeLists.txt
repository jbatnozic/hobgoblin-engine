# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence


add_executable(${PROJECT_NAME} 
    Main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME}
PUBLIC
  "CONAN_PKG::hobgoblin"
)
