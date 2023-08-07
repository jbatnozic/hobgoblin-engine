#! /bin/bash

# 1st argument: compiler (gcc|clang)
# 2nd argument: profile  (Debug|Release)

conan install . -of _Build/ \
    --profile=hobgoblin-$1 \
    -s build_type=$2 \
    --build=missing \
    --build=outdated \
    --build=cascade
