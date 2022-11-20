#! /bin/bash

# SFML implementation used std::auto_ptr (which was removed in C++17)
# so we have to build it as C++14 unlike the rest of the project.
# Luckily, it's ABI-compatible with C++17.

conan install . -if _Build/ \
    --profile=default \
    --build=outdated \
    -s build_type=$1 \
    -s compiler.cppstd=17 \
    -s sfml:compiler.cppstd=14
