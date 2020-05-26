conan install . -if build-cmake/x64-Debug-Lib/ -s build_type=Debug
conan install . -if build-cmake/x64-Debug-Exe/ -s build_type=Debug
conan install . -if build-cmake/x64-Release-Lib/
conan install . -if build-cmake/x64-Release-Exe/