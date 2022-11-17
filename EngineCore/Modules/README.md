# Modules
Hobgoblin provides its features across several modules.

**Note:** For more detailed information about each specific module, documentation and examples of usage, see that
module's respective `README.md` file. Also check the comments in public headers.

## Base modules
Basic types and definitions needed by all other modules:
- **Common:** Definitions of common numeric and pointer types, exceptions and similar.
- **Private:** Internal implementation details.

## General modules
- **ChipmunkPhysics:** [A C-based physics engine](https://chipmunk-physics.net/) not written by me but only embedded
within Hobgoblin, with some type-safety extensions included.
- **ColDetect:** Facilities for efficient broad-phase and narrow-phase 2D collision detection. (NOTE: Narrow-phase not
yet implemented)
- **Graphics:** Various utilities for rendering stuff on-screen, loading sprites, packing them onto textures etc.
- **Math:** Math stuff - geometry, trigonometry, vectors, quadratic equations etc.
- **QAO:** Event-driven framework for managing game objects.
- **Preprocessor:** A bit of macro magic! (Rarely needed in modern C++, but occasionally useful.).
- **RigelNet:** A networking library based on the 
[remote procedure calling](https://en.wikipedia.org/wiki/Remote_procedure_call) paradigm made specifically for 
peer-to-peer connections - where both the clients and the server will be executing code compiled from the same source.
It uses either TCP (not yet implemented) or a custom-built reliable UDP protocol.
- **RmlUi:** https://github.com/mikke89/RmlUi
- **Utility:** Miscellaneous generic utilities, such as custom containers, commonly used functions, multithreading
utilities and other things.

With the exception of `Preprocessor` and `Utility`, which are occasionally used by other modules, these Generic 
modules are mutually independent.