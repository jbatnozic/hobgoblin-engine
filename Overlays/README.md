# Engine Overlays
While the [core of the engine](https://github.com/jbatnozic/Hobgoblin/tree/master/EngineCore#engine-core) is modular
and quite generic, it can feel less like a game engine and more like a disjoint collection of independent parts.
This is done as to make it as flexible as possible, but it does have a negative impact to user-friendliness.

The concept of an Engine Overlay is then split in two parts: 
- There is an **assumption** about what kind of game will be made with it, 
- And there is a **collection of additional tools** which are specialized to help with making such games.

Overlay is then combined with the engine core (basically, it goes "on top" of it) to provide something more akin to
what you would expect from a fully-fledged game engine.

For example, SPeMPE assumes that you want to make a peer-to-peer multiplayer game with graphics displayed in a
single window, so it provides a convenient framework which alleviates much of the pain in writing such games.

## Directories
Here you will find:
- **SPeMPE:** An Overlay for peer-to-peer multiplayer games with graphics displayed in a single window.
