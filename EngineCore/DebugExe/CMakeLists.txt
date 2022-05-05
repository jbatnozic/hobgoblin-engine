
add_executable("Hobgoblin.DebugExe"
    "Source/SpriteBatchMain.cpp"
)

target_link_libraries("Hobgoblin.DebugExe"
PUBLIC
  "Hobgoblin.Common"
  "Hobgoblin.Graphics"
  # "Hobgoblin.QAO"
  # "Hobgoblin.RigelNet"
  "Hobgoblin.Math"
  "Hobgoblin.Utility"
  "CONAN_PKG::sfml"
  # "CONAN_PKG::ztcpp"
)