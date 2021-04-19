conan install . -if Build/Hobgoblin-x64-Debug/ ^
    --profile=default ^
    --build=outdated ^
    -s build_type=Debug ^
    -s ztcpp:compiler.cppstd=17
    
conan install . -if Build/Hobgoblin-x64-Release/ ^
    --profile=default ^
    --build=outdated ^
    -s build_type=Release ^
    -s ztcpp:compiler.cppstd=17