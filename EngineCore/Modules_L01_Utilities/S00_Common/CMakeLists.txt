project("Hobgoblin.Common")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE "Include/")

target_link_libraries(${PROJECT_NAME}
INTERFACE
    # Foundation
    "Hobgoblin_L00_S00_PDef"
)

add_library("Hobgoblin_L01_S00_Common" ALIAS ${PROJECT_NAME})
