project("SPeMPE.ManualTest")

add_executable(${PROJECT_NAME}
    "_Main.cpp"
    "Actors.cpp"
    "Main_gameplay_manager.cpp"
    "Simple_zerotier.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
  "SPeMPE"
)
