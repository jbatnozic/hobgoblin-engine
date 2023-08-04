@ECHO OFF

REM SFML implementation used std::auto_ptr (which was removed in C++17)
REM so we have to build it as C++14 unlike the rest of the project.
REM Luckily, it's ABI-compatible with C++17.

@ECHO ON

conan install . -of _Build/Hobgoblin-x64/ ^
    --profile=hobgoblin ^
    -s build_type=Debug ^
    -s compiler.cppstd=17 ^
    -s sfml:compiler.cppstd=14 ^
    --build=missing ^
    --build=outdated ^
    --build=cascade
    
conan install . -of _Build/Hobgoblin-x64/ ^
    --profile=hobgoblin ^
    -s build_type=Release ^
    -s compiler.cppstd=17 ^
    -s sfml:compiler.cppstd=14 ^
    --build=missing ^
    --build=outdated ^
    --build=cascade
