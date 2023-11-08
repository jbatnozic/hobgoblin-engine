# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.Common")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/Gsl_pointers.cpp"
  "Source/Traced_exceptions.cpp"
)

# ===== TARGET SETUP =====

add_library(${COMPONENT_NAME} STATIC ${COMPONENT_SOURCES})
target_include_directories(${COMPONENT_NAME} PUBLIC "Include/")

if(WIN32)
    set(STACK_BACKTRACE_LIB "DbgHelp")
else()
    set(STACK_BACKTRACE_LIB "")
endif()

target_link_libraries(${COMPONENT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin_L00_S00_PDef"

    # External
    ${STACK_BACKTRACE_LIB}
)

add_library("Hobgoblin_L01_S00_Common" ALIAS ${COMPONENT_NAME})
