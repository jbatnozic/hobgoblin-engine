# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

project("Hobgoblin.HGExcept")

add_library(${PROJECT_NAME}
STATIC
  "Source/Traced_exception.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC "Include/")

if(WIN32)
    set(STACK_BACKTRACE_LIB "DbgHelp")
else()
    set(STACK_BACKTRACE_LIB "")
endif()

target_link_libraries(${PROJECT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"
    "Hobgoblin_L00_S01_Format"
    "Hobgoblin_L00_S01_Preprocessor"

    # External
    ${STACK_BACKTRACE_LIB}
)

add_library("Hobgoblin_L00_S02_HGExcept" ALIAS ${PROJECT_NAME})

add_subdirectory("Test")
