
add_executable(${PROJECT_NAME} 
    Main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME}
PUBLIC
  "CONAN_PKG::hobgoblin"
)
