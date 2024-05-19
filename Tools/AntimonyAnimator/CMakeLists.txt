# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Antimony")

add_executable(${PROJECT_NAME}
    "AAMain.cpp"
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    # Foundation
    "Hobgoblin.Common"
    "Hobgoblin.HGExcept"
    "Hobgoblin.Logging"

    # Utilities
    "Hobgoblin.Math"

    # Principals
    "Hobgoblin.Graphics"

    # Overlay
    "SPeMPE"
)
