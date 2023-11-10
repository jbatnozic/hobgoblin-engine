project("Hobgoblin.HGExcept.ManualTest")

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME} 
  "HGExcept_manual_test.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin.HGExcept"
)
