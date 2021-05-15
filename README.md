# Hobgoblin
Hobgoblin is a small game engine, intended to serve as a basis for small and games. It provides often-needed 
facilities such as object and timing measurement, graphics, physics, networking and various other utilities.

It is written in C++17 and uses [SFML](https://www.sfml-dev.org/) as the core for many of its features.

It is tested most thoroughly on Windows (built with MSVC), but as it uses CMake as its build system and only
standard language features, it should work with little to no modifications on other platforms where its 
dependencies work (Linux and MacOS at least).

## Concept
The idea is that when you're using Hobgoblin, you use it in two parts: The 
[Core](https://github.com/jbatnozic/Hobgoblin/tree/master/EngineCore#enginecore) and an 
[Engine Overlay](https://github.com/jbatnozic/Hobgoblin/tree/master/Overlays#engine-overlays). The core provides a
collection of generic and modular tools for various things needed in game development, and then an Overlay is put on
top to tie them together into a cohesive whole and something that looks more like what you'd expect from a game engine.

## Directories
Here you will find:
 - **EngineCore:** The core of the engine.
 - **Overlays:** Various ready-to-use Overlays that can be combined with the core.
 - **ProofOfConceptProjects:** Projects which are used either to develop future Hobgoblin features, or depend on
Hobgoblin and serve as showcases and examples of its usage. They also test whether the API is user-friendly, so
they are placed in the same repository for faster development in case something needs to be changed.
 - **Tools:** TODO

Most of the directories have their own Readme file which describes relevant contents in more detail.

## Build instructions