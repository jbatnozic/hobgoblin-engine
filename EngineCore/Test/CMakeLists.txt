# ===== COMPONENT SETUP =====

set(COMPONENT_NAME "Hobgoblin.Test")

# ===== GATHER SOURCES ======

set(COMPONENT_SOURCES
  # "Source/ColDetect_test.cpp" TODO
  "Source/Gsl_pointers_test.cpp"
  "Source/gtest_main.cpp"
  "Source/QAO_serialization_test.cpp"
  "Source/Qao_test.cpp"
  "Source/RigelNet_test.cpp"
  "Source/Util_test.cpp"
)

# ===== TARGET SETUP =====

add_executable(${COMPONENT_NAME} ${COMPONENT_SOURCES})

target_link_libraries(${COMPONENT_NAME}
PUBLIC
  "Hobgoblin.Common"
  "Hobgoblin.Math"
  "Hobgoblin.QAO"
  "Hobgoblin.RigelNet"
  "Hobgoblin.Utility"
  "CONAN_PKG::gtest"
)