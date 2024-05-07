# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("SPeMPE.ManualTest")

add_executable(${PROJECT_NAME}
    "_Main.cpp"
    "Actors.cpp"
    "Main_gameplay_manager.cpp"
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
  "SPeMPE"
)
