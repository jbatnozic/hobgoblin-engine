find_package(hobgoblin CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    "Main.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    hobgoblin::hobgoblin
)
