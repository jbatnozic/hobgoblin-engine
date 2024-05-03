project("Alvin")

add_executable(${PROJECT_NAME}
    "Alvin.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC "Include/")

target_link_libraries(${PROJECT_NAME}
    "Hobgoblin.Common"
    "Hobgoblin.ChipmunkPhysics"
    "Hobgoblin.Graphics"
    "Hobgoblin.Window"
)
