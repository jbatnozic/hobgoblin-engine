project("FrameTimingTester")

add_executable(${PROJECT_NAME}
    "FTTMain.cpp"
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
