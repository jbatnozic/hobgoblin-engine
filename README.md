# Hobgoblin Engine

Small and simple game engine, intended to serve as a basis for small and simple games.
It is written in C++17 and uses [SFML](https://www.sfml-dev.org/) as its core.

### Modules:
Hobgoblin provides its features across several mutually independent (with one exception) modules:

 - **ColDetect:** Facilities for efficient broad-phase and narrow-phase 2D collision detection. (NOTE: Narrow-phase not
 yet implemented)
 
 - **Graphics:** Various utilities for rendering stuff on-screen, loading sprites, packing them onto textures etc.
 
 - **QAO:** Event-driven framework for managing [active game objects](https://en.wikipedia.org/wiki/Active_object). 
 However, the traditional threaded approach is greatly simplified  in that it's designed to be single-threaded - 
 objects can implement certain "event" methods and the single-threaded QAO runtime calls them periodically in a well 
 defined order. It's left up to the implementer to ensure that these methods are mutually cooperative (that they always
 perform a small enough chunk of their jobs so that they return quickly and leave enough time for others). 
 
 - **RigelNet:** A networking library based on the 
 [remote procedure calling](https://en.wikipedia.org/wiki/Remote_procedure_call) paradigm made specifically for 
 peer-to-peer connections - where both the clients and the server will be executing code compiled from the same source.
 It uses either TCP (not yet implemented) or a custom-built reliable UDP protocol.
 
 - **Utility:** Miscellaneous generic utilities, such as custom containers, commonly used functions, multithreading
 utilities and other things. This package is the exception mentioned in the above rule, in that other modules may (and
 do) depend on this one.