# ===== PROJECT SETUP =====

cmake_minimum_required(VERSION 3.14)

project("Hobgoblin" VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_DEBUG_EXECUTABLE 
  "Build a runnable executable for debugging purposes instead of a static library"
  OFF
)

# ===== CONAN INTEGRATION =====

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

# ===== ADD MODULES =====

add_subdirectory("Engine")
add_subdirectory("Test")








# old build sysyem
if(False)
  set(HOBGOBLIN_SOURCE_ALL
      ${HOBGOBLIN_SOURCE_CHIPMUNK}
      source/Hobgoblin/ChipmunkPhysics/prime.h
      ${HOBGOBLIN_SOURCE_COMMON}
      ${HOBGOBLIN_SOURCE_COLDETECT}
      ${HOBGOBLIN_SOURCE_GRAPHICS}
      ${HOBGOBLIN_SOURCE_PRIVATE}
      ${HOBGOBLIN_SOURCE_QAO}
      ${HOBGOBLIN_SOURCE_RIGELNET}
      ${HOBGOBLIN_SOURCE_UTILITY}
  )

  if(BUILD_DEBUG_EXECUTABLE)
      set(HOBGOBLIN_SOURCE_ALL
          ${HOBGOBLIN_SOURCE_ALL}
          ${HOBGOBLIN_SOURCE_MAIN}
      )
  endif()

  # Compilation configuration
  add_compile_definitions("SFML_STATIC")

  if(BUILD_DEBUG_EXECUTABLE)
      set(HOBGOBLIN_TARGET_NAME
          ${PROJECT_NAME}-debug-executable
      )
      add_executable(${HOBGOBLIN_TARGET_NAME}
          ${HOBGOBLIN_SOURCE_ALL}
      )
  else()
      set(HOBGOBLIN_TARGET_NAME
          ${PROJECT_NAME}
      )
      add_library(${HOBGOBLIN_TARGET_NAME}
          STATIC
          ${HOBGOBLIN_SOURCE_ALL}
      )
  endif()

  target_include_directories(${HOBGOBLIN_TARGET_NAME} PUBLIC include)
  target_include_directories(${HOBGOBLIN_TARGET_NAME} PUBLIC include/Hobgoblin/private)
  target_link_libraries(${HOBGOBLIN_TARGET_NAME} PUBLIC ${CONAN_LIBS})

  if(WIN32)
      target_link_libraries(${HOBGOBLIN_TARGET_NAME} PRIVATE legacy_stdio_definitions.lib)
  endif()

  unset(BUILD_DEBUG_EXECUTABLE CACHE)
endif()