# Hobgoblin
Hobgoblin is a small game engine, intended to serve as a basis for small and games. It provides often-needed 
facilities such as object and timing measurement, graphics, physics, networking and various other utilities.

It is written in C++20 and uses [SFML](https://www.sfml-dev.org/) as the core for many of its features.

It is tested most thoroughly on Windows (built with MSVC), and semi-regularly on Linux and Mac
(built with Clang and GCC). Basically: It should work without issue on major platforms and major
compilers.

## Concept
The idea is that when you're using Hobgoblin, you use it in two parts: The 
[Core](https://github.com/jbatnozic/Hobgoblin/tree/master/EngineCore#engine-core) and an 
[Engine Overlay](https://github.com/jbatnozic/Hobgoblin/tree/master/Overlays#engine-overlays). The core provides a
collection of generic and modular tools for various things needed in game development, and then an Overlay is put on
top to tie them together into a cohesive whole and something that looks more like what you'd expect from a game engine.

## Directories
Here you will find:
 - **EngineCore:** The core of the engine.
 - **Overlays:** Various ready-to-use Overlays that can be combined with the core.
 - **ProjectSkeletons:** Standalone projects that you can copy to quickly get started with making a game with
 Hobgoblin.
 - **ProofOfConceptProjects:** Projects which are used either to develop future Hobgoblin features, or depend on
Hobgoblin and serve as showcases and examples of its usage. They also test whether the API is user-friendly, so
they are placed in the same repository for faster development in case something needs to be changed.
 - **Scripts:** Various helper scripts for building and exporting the engine.

Most of the directories have their own Readme file which describes relevant contents in more detail.

## Build instructions
If you don't already have it, you will need to install the package manager [Conan](https://conan.io/) (version 2.0.0 or
higher) in order to be able to fetch all of Hobgoblin's dependencies and build it successfully. Other than that, you
will need [CMake](https://cmake.org/) (version 3.23 or higher) and a C++ compiler that supports C++20.

### Windows

Before anything else, copy the appropriate Conan profile (from `Conan\Profiles\Windows`) into your `.conan2\profiles`
folder.

#### To work on Hobgoblin

1. Run `Scripts\Get_conan_dependencies.bat`.
2. To build and install all the dependencies, run `Conan_install_dependencies.bat`.
3. To edit or build the engine, open the root folder in Visual Studio.

#### To make a game with Hobgoblin

1. Run `Scripts\Get_conan_dependencies.bat`.
2. Run `Scripts\Conan_export_engine.bat`.
3. Copy one of the skeletons from `ProjectSkeletons` and adapt its `conanfile.py` and `CMakeLists.txt` files as needed,
   then run its `Conan_install_dependencies.bat`.
4. Open the project in Visual Studio.

### Linux/Mac

Before anything else, copy the appropriate Conan profile (from `Conan/Profiles/Linux` or `Conan/Profiles/Max`) into your
`~/.conan2/profiles` directory.

Then, if on Linux, you'll need to install some system dependencies. This wasn't tested very thoroughly, so
depending on your specific system, the concrete dependencies could be different. My setup on Ubuntu
required the following list:

```
sudo apt install -y libudev-dev libgl-dev libgl1-mesa-dev libx11-xcb-dev libfontenc-dev libice-dev libsm-dev \
		              libxaw7-dev libxcomposite-dev libxcursor-dev libxdamage-dev libxfixes-dev libxi-dev \
		              libxinerama-dev libxkbfile-dev libxmu-dev libxmuu-dev libxpm-dev libxrandr-dev libxres-dev \
		              libxt-dev libxtst-dev libxv-dev libxxf86vm-dev libxcb-glx0-dev libxcb-render0-dev \
		              libxcb-render-util0-dev libxcb-xkb-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev \
		              libxcb-randr0-dev libxcb-shape0-dev libxcb-sync-dev libxcb-xfixes0-dev libxcb-xinerama0-dev \
		              libxcb-dri3-dev libxcb-cursor-dev libxcb-dri2-0-dev libxcb-dri3-dev libxcb-present-dev \
		              libxcb-composite0-dev libxcb-ewmh-dev libxcb-res0-dev libxcb-util-dev libxcb0-util-dev 
```

Another option is to skip this step and just install whatever is needed whenever something in the build
process complains that a library is missing.

#### To work on Hobgoblin

1. Run `Scripts/Get_conan_dependencies.bash`.
2. To build and install all the dependencies, run `conanw.bash install <profile> <type>`. The profile can be `gcc` or 
   `clang` for Linux, or `apple-clang` for Mac. The type can be `Debug` or `Release` for either platform.
3. To edit the code, open the root folder in your favourite editor or IDE. To build it, run
   `conanw.bash build <profile> <type>`, using the same parameters as you did for `install`.

#### To make a game with Hobgoblin

1. Run `Scripts/Get_conan_dependencies.bash`.
2. Run `Scripts/Conan_export_engine.bash`.
3. Copy one of the skeletons from `ProjectSkeletons` and adapt its `conanfile.py` and `CMakeLists.txt` files as needed.
4. To be continued... (TODO)

## Licence
The engine is licensed under the [MS-PL](https://opensource.org/license/ms-pl-html) licence.

This EXCLUDES certain files which are modified versions of SFML code and Chipmunk Physics code - these files retain
their original copyright notices.
