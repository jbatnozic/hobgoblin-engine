
find_package(sfml CONFIG REQUIRED)

add_executable("Hobgoblin.DebugExe"
  "Source/RcssPreprocessorMain.cpp"
)

target_link_libraries("Hobgoblin.DebugExe"
PUBLIC
  "Hobgoblin.Common"
  "Hobgoblin.Graphics"
  "Hobgoblin.Logging"
  # "Hobgoblin.QAO"
  # "Hobgoblin.RigelNet"
  "Hobgoblin.Math"
  "Hobgoblin.Utility"
  "Hobgoblin.RmlUi"
  "sfml::sfml"
  # "ztcpp::ztcpp"
)
