# Project Skeletons

This folder contains projects that are already set up to work with Hobgoblin, so you can copy/paste
them, rename them and edit accordingly to begin making your game in no time!

## First-Time Build

If you're just starting out, you're going to need to export the Hobgoblin Engine to your local
Conan Cache before you can use it as a dependency in your game. To do that, follow these
instructions:

### On Windows:

- Call the script `Get_conan_dependencies.bat` found in `Hobgoblin/Scripts/`.
- Call the script `Conan_export_engine.bat` found in `Hobgoblin/Scripts/`.

### On Other Systems:

- Call the script `Get_conan_dependencies.bash` found in `Hobgoblin/Scripts/`.
- Call the script `Conan_export_engine.bash` found in `Hobgoblin/Scripts/`.

## Directories

- **SPeMPE_CMake_Empty:** A bare-bones CMake project that only includes Hobgoblin and its overlay
SPeMPE and doesn't have more than an empty `main` function.
