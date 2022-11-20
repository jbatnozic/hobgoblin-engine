
add_executable("minimal-multiplayer"
    "Minimal_multiplayer.cpp"
)

target_link_libraries("minimal-multiplayer"
PRIVATE
    "${OVERLAY_NAME}"
)