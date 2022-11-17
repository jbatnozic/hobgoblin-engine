# SPeMPE Example: Multiplayer Foundation

This example program lays a foundation for a full-featured multiplayer game using SPeMPE. You can reference or even
just reuse this example program to build your own game.

## Required reading

For the most part, SPeMPE is a combination of [QAO](https://github.com/jbatnozic/Hobgoblin/tree/master/EngineCore/Modules/QAO)
and [RigelNet](https://github.com/jbatnozic/Hobgoblin/tree/master/EngineCore/Modules/RigelNet) with some additional code
on top, so familiarize yourself with both of those before proceeding.

## Importing

To use SPeMPE in your game, simply do `#include <SPeMPE/SPeMPE.h>`.

All the symbols are in the namespace `jbatnozic::spempe`. In this guide and in the example code I will use the alias
`namespace spe = jbatnozic::spempe;` to shorten it.

**Note:** Any names in `jbatnozic::spempe::detail` as well as those starting with `USPEMPE_` or `__spempe` (no matter
in which namespace they are) represent types and functions that are the internal implementation of the engine. Don't use
those, they won't be documented and they can be changed at any time.

## SPeMPE Contexts

The starting point both for this guide and for making a SPeMPE game in general should be the game context - 
`spe::GameContext`. You can think of this context as a container that holds all of your game's state, objects, and 
everything else needed to run the game. The most important feature of the game context is that it contains:
- A QAO runtime (`QAO_Runtime`) which will hold all of your active objects and run the game in a structured way;
- Any number of Context Components (I will explain those in more detail later, but in short, they are user-made
extensions to the context which bolster its functionality and they are always available through the context);
- Up to 1 nested context (a child context). There is a reason why you may want this.

By design, any active object ((digression: here, and from now on, when I say "active object", I mean any object
inheriting from `QAO_Base` and currently attached to a runtime)) within a `spe::GameContext` (attached to that context's
`QAO_Runtime`) has access to the context object at all times, and should access all needed game data (and other objects)
through the context.

**Note:** This works simply by using the `setUserData` method of `QAO_Runtime` to set the user data to be the pointer
to the context. Therefore it's **VERY IMPORTANT** that you **DO NOT** change the user data of a QAO runtime that's
embedded in a SPeMPE context!

So with that out of the way let me reiterate my previous point: objects in a context should be able to get all the data
they require through their context. This data should be held _by the Context_ (directly or indirectly). This also means
**not to use global variables** (constants and reading globals are fine if you're sure they won't change). Avoiding
globals is good advice in general but here it's even more important. For maximum capabilities, you should be able to 
have 2 separate contexts in 2 separate threads in **the same process** without breaking anything. Everything that's a
part of SPeMPE already supports this, but when making your custom objects that you're going to attach to a context you
should keep this in mind.

So... Why go to all this trouble? Why make contexts perfectly isolated? Maybe you've guessed by now it has to do with
nested contexts mentioned earlier. Remember, SPeMPE's primary porpose is to provide an easy platform for _multiplayer_
games. Which means you need a server of some sort and clients to connect to it. However, for user convenience, it's
simpler if they don't need to run a separate dedicated server if they don't want to -- with most games that support LAN
multiplayer, one player enters the game and clicks "Host Game" or "Create Game" and the other players join him - no
setup needed. Does this mean we need to create our game in 3 variants (dedicated server, client, and a
server-that's-also-a-client-hybrid)? Well, it's certainly possible, but adds unnecessary complication. What we can do
_instead_ is keep only the server and client modes and forget about the weird hybrid. The server mode then has only 1
purpose: run the game for the connected clients/players. And the client mode as well: connect to a server and display
game updates provided by the server, while constantly uploading the player's input. This setup makes programming both of
these relatively simple. With nested contexts, it doesn't even take away anything - you can simply run a server context
in a child context, in a different thread, within the same process:
- **Singleplayer experience:** Player enters the game, one context (the one which the player sees) runs in client mode
and connects to a "hidden" context (that's created automatically in the background) running in server mode. Remember
that RigelNet supports "local connections", that is, if both the server and the client are in the same process, they
can send messages to each other directly, bypassing the sockets completely, and this is very fast and efficient.
- **Hosting a game experience:** Basically the same as singleplayer. The only change we need to make is configure the
server to support more than 1 connection at a time.
- **Dedicated server experience:** Strictly speaking, there is no "player" here. Somebody just runs the program which
starts a single context in server mode and waits for players to connect.
- **Connecting to server experience:** Player enters the game, now there is only one context that the player sees and
it's in client mode. So they connect to a server and play the game. They don't care if the server is dedicated or not.

By the way, the above is how some popular games out in the real world, including Minecraft, function. As long as it's
implemented correctly it's a _good idea_.

Now that we've covered the philosophy behind contexts and context design, let's move on to how to actually achieve it...

### Context Components

TODO

### Child Contexts

TODO

### Miscellaneous Context Information

TODO
