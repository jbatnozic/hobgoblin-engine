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

### Instantiating a Context

TODO

### Context Components

Context Components are objects that are, informally speaking, global in the scope of the context they are attached to.
They provide some service, resource, etc. to objects within the context.

For example: Imagine that you wanted to have an object that will load the game's graphics resources (sprites for
example) when the game loads and then hold and provide them when needed. An object like this would be a perfect
candidate to be a context component because it's something that's universally needed by many objects in the game,
but at the same time we don't want to have to pass a reference to it manually all the time. Other examples of objects
that would make good context components are: user input controllers, window controllers networking controllers etc.

A Context Component can _also_ be an active object (be a member of the QAO runtime and respond to events same as any
other), in which case the SPeMPE convention is to call it a "Manager". More word on this later.

For multiple reasons, including: avoiding circular includes, reducing coupling, increased testability and
maintainability, and others, it's **strongly** recommended to separate all context components into abstract
interface and concrete implementation.

Example in code (based on the previous example with sprite loading):

```cpp
// File: Sprite_provider_interface.hpp
#include <SPeMPE/SPeMPE.h>

class SpriteProviderInterface: public spe::ContextComponent {
public:
    virtual void loadAllSprites() = 0;
    virtual Sprite getSprite(SpriteID aSpriteId) const = 0;
    
private:
    SPEMPE_CTXCOMP_TAG("my::namespace::SpriteProviderInterface");
};

// File: Sprite_provider_default.hpp
#include "Sprite_provider_interface.hpp"

class DefaultSpriteProvider: public SpriteProviderInterface {
public:
  DefaultSpriteProvider(...);
  
  void loadAllSprites() override;
  Sprite getSprite(SpriteID aSpriteId) const override;
  
private:
  /* data... */
};

// File: Sprite_provider_default.cpp
#include "Sprite_provider_default.hpp"

/* some implementation... */
```

As you see creating a context component is very much like creating a regular ol' C++ class, with a few extra steps:
- The interface of the component needs to inherit from `spe::ContextComponent`.
- Somewhere in the body of the interface, preferrably in its private section, you need to provide a tag using the
`SPEMPE_CTXCOMP_TAG("<tag>")` macro. It's recommended that the tag be in `company::project::ClassName` format, because
you can't have multiple context components with the same tag.
- Multiple inheritance from `spe::ContextComponent` is not supported. In other words, the implementation of your
component can inherit from only 1 class/interface that itself inherits from `spe::ContextComponent` and also from only
one class that has a tag (provided using the macro above) - preferrably these two should be the same class.

The following code snippet demonstrates how to attach a component to a context:

```cpp
auto context = std::make_unique<spe::GameContext>(...);
auto spriteProvider = std::make_unique<DefaultSpriteProvider>(...);

context->attachComponent(*spriteProvider);

// later:
context->detachComponent(*spriteProvider);
```

In the example above, the context does not assume ownership of the attached component - the component is only made
accessible through the context. Whatever code attached the component is also responsible to later detach and destroy
it.

However, we can also let the context itself manage the lifetime of its components (recommended):

```cpp
auto context = std::make_unique<spe::GameContext>(...);
auto spriteProvider = std::make_unique<DefaultSpriteProvider>(...);

// In this case attachAndOwnComponent accepts components ONLY through a unique_ptr by value
context->attachAndOwnComponent(std::move(spriteProvider));

// later:
context.reset(); // at this point, all attached components are destroyed in the reverse order of attaching, AFTER
                 // the embedded QAO runtime has been cleared.
```

Accessing context components:

```cpp
spe::GameContext& context = ...; // get a reference to the context somehow

auto& spriteProvider = context.getComponent<SpriteProviderInterface>();
// ^ notice that we're using the type of the interface and NOT the concrete class above.
// Also note that this call will throw an exception if no matching component is attached.
spriteProvider.loadAllSprites();
// do other stuff with the component...

// OR

auto* spriteProvider = context.getComponentPtr<SpriteProviderInterface>();
// ^ the above will simple return nullptr if no matching component is attached.
if (spriteProvider) {
  spriteProvider->loadAllSprites();
  // do other stuff with the component...
}
```

Using `getComponent` and `getComponentPtr` are the most basic way to access your components, but for some of your
objects there is an even shorter notation -- see the "Game Object Framework" section for more info on this.

### Child Contexts

TODO

### Miscellaneous Context Information

TODO

## Game Object Framework

Though SPeMPE uses QAO, it provides 3 subclasses of `QAO_Base` to be used _instead_ of `QAO_Base` depending on how you
plan to use the object. Those are:
- `spe::NonstateObject` - You should inherit from this one in two cases: 1) For objects, such as particle effects,
which are not essential to the game's state and thus not saved (when writing game state) nor synchronized with clients
(in multiplayer sessions); 2) Controllers which are always created when the game starts and thus always implicitly
present, so we don't need to save or synchronize them.
- `spe::StateObject` - You should inherit from this class for objects which are essential for the game's state, but
will not be synchronized with clients. This is intended for use with singleplayer games, as it's not particularly
useful for multiplayer games, though it could possibly be utilized for some hidden server-side-only objects.
//! objects in multiplayer games.
- `spe::SynchronizedObject` - You should inherit from this one when making objects which are essential to the game's
state, so they are both saved when writing game state, and synchronized (with SPeMPE, automatically!) with clients in
multiplayer sessions. For example, units, terrain, interactible items (and, basically, most other game objects).

There are a few elements which are common to all 3 of the above and will be available to your classes no matter which
one they inherit from:

TODO
=======================================================================================================================
