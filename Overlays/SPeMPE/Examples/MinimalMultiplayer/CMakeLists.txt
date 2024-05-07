# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence


add_executable("minimal-multiplayer"
    "Minimal_multiplayer.cpp"
)

target_link_libraries("minimal-multiplayer"
PRIVATE
    "${OVERLAY_NAME}"
)
