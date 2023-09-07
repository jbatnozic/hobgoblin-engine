# ===== COMPONENT SETUP =====

find_package(GTest CONFIG REQUIRED)

set(COMPONENT_NAME "Hobgoblin.Test")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  "Source/gtest_main.cpp"

  # "Source/ColDetect_test.cpp" TODO
  "Source/Format_test.cpp"
  "Source/Gsl_pointers_test.cpp"
  "Source/Hg_config_test.cpp"
  "Source/Math_test.cpp"
  "Source/gtest_main.cpp"
  "Source/Preprocessor_test.cpp"
  "Source/QAO_serialization_test.cpp"
  "Source/QAO_test.cpp"
  "Source/RigelNet_test.cpp"
  "Source/Util_test.cpp"
)

# ===== TARGET SETUP =====

add_executable(${COMPONENT_NAME} ${COMPONENT_SOURCES})

target_link_libraries(${COMPONENT_NAME}
PUBLIC
  "Hobgoblin.Common"
  "Hobgoblin.Format"
  "Hobgoblin.Config"
  "Hobgoblin.Math"
  "Hobgoblin.QAO"
  "Hobgoblin.RigelNet"
  "Hobgoblin.Utility"
  "gtest::gtest"
)
