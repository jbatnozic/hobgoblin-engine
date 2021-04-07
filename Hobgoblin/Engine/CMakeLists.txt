# ===== ADD COMPONENTS =====

# Add Private, Common, Preprocessor and Utility first, in that order,
# because other components may depend on them
add_subdirectory("Private")
add_subdirectory("Common")
add_subdirectory("Preprocessor")
add_subdirectory("Utility")

add_subdirectory("ChipmunkPhysics")
add_subdirectory("Graphics")
add_subdirectory("Math")
add_subdirectory("QAO")
add_subdirectory("RigelNet")

# ===== COMPILE WHOLE ENGINE =====

if(False)
  set(HOBGOBLIN_ENGINE_TARGET_NAME "hobgoblin")

  add_library(${HOBGOBLIN_ENGINE_TARGET_NAME} STATIC
    "Dummy.cpp"
  )

  target_link_libraries(${HOBGOBLIN_ENGINE_TARGET_NAME}
  PUBLIC
    "Hobgoblin.Private"
    "Hobgoblin.Common"
    "Hobgoblin.Preprocessor"
    "Hobgoblin.Utility"

    "Hobgoblin.ChipmunkPhysics"
    "Hobgoblin.Graphics"
    "Hobgoblin.Math"
    "Hobgoblin.QAO"
    "Hobgoblin.RigelNet"
  )
endif()