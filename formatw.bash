#! /bin/bash

echo "Formatting code..."

./Scripts/Run_clang_format.py -r . -i

echo "Done!"
