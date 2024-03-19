# ===== COMPONENT SETUP =====

find_package(GTest CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.Test")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/gtest_main.cpp"

  # "Source/ColDetect_test.cpp" TODO
  "Source/Format_test.cpp"
  "Source/Hg_config_test.cpp"
  "Source/Math_test.cpp"
  "Source/gtest_main.cpp"
  "Source/Preprocessor_test.cpp"
  # "Source/QAO_serialization_test.cpp"
  "Source/QAO_test.cpp"
  "Source/RigelNet_test.cpp"
  "Source/Util_test.cpp"
)

# ===== TARGET SETUP =====

add_executable(${COMPONENT_NAME} ${COMPONENT_SOURCES})

target_link_libraries(${COMPONENT_NAME}
PUBLIC
    # Foundation
    "Hobgoblin.Format"

    # Utilities
    "Hobgoblin.Common"
    "Hobgoblin.Math"
    "Hobgoblin.Utility"

    # Principals
    "Hobgoblin.HGConfig"
    "Hobgoblin.QAO"
    "Hobgoblin.RigelNet"
  
    # External
    "gtest::gtest"
)
