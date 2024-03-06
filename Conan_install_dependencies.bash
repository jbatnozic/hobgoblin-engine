#! /bin/bash

# 1st argument: compiler (gcc|clang)
# 2nd argument: profile  (Debug|Release)

conan install . -of build_/ \
    -pr:h=hobgoblin-$1 \
    -pr:b=hobgoblin-$1 \
    -s build_type=$2 \
    --build=missing \
    --build=outdated \
    --build=cascade
