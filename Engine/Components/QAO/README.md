# QAO
`#include <Hobgoblin/QAO.hpp>`

QAO is a dynamic event-based framework for managing game objects. Its name is an acrobym for
"Quasi Active Objects", because, in a way, it mimics the [active objects](https://en.wikipedia.org/wiki/Active_object)
paradigm, though it does so in a greatly simplified manner (with no multithreading required). This isn't a bad
thing - in general, games have lots of objects doing stuff at once (players characters, enemies, interactive
scenery, various controllers etc) and having each run in its separate thread would be a timing nightmare.

**Note:** QAO uses an older naming scheme where all symbols are prefixed with `QAO_`.

## Concept
The framework is based on two main parts: The runtime (`QAO_Runtime`) and the active objects themselves (any class
which inherits from `QAO_Base`). The objects have one or more `_event*` methods inherited from `QAO_Base` overriden,
and are then registered with a runtime object, which then "runs" then, that is, calls their events at appropriate
times.

### Events
There are a number of predefined events in QAO (in order in which they run):

| # | Event         | Method                  |
|---|---------------|-------------------------|
| 1 | StartFrame    | `_eventStartFrame()`    |
| 2 | PreUpdate     | `_eventPreUpdate()`     |
| 3 | Update        | `_eventUpdate()`        |
| 4 | PostUpdate    | `_eventPostUpdate()`    |
| 5 | Draw1         | `_eventDraw1()`         |
| 6 | Draw2         | `_eventDraw2()`         |
| 7 | DrawGUI       | `_eventDrawGUI()`       |
| 8 | FinalizeFrame | `_eventFinalizeFrame()` |

While you can write any code for any given event, each has a specific intended purpose, and you should stick to 
these guidelines for best results:
- **StartFrame:** For code that absolutely must execute before anything else. This is the least used event.
- **PreUpdate:** Objects prepare for the upcoming Update - actions like gathering user input, reading network messages
and similar are appropriate for this event. Objects should interact as little as possible here.
- **Update:** This is the "main" event where most of the work happens. Ideally, all objects prepared themselves during
`PreUpdate`, so they are free to interact with each other (including creating new objects or deleting existing ones).
- **PostUpdate:** In this event, we process the results of the preceding `Update` event. In a multiplayer game, this
would be where the server uploads updates to clients. You can also view it as a preparation for the `Draw*` events.
Like in `PreUpdate`, interactions between objects should be minimal.
- **Draw1:** The game world is drawn. You can have each object resposible for drawing its own state, or have one
object that draws everything,
- **Draw2:** Same as `Draw1` basically. But, because things drawn later appear on top of those drawn before, it can
sometimes be useful to have 2 separate Draw events to it's easier to order everything properly.'
- **DrawGUI:** A GUI always goes on top of everything else, so it's natural to draw it last. (Also, it's worth noting
that a GUI is usually drawn relative to the 'window' coordinate system and not the 'game world' coordinate system.)
- **FinalizeFrame:** Code that absolutely must run after everything else. Usually you'll only have 2 actions here: 
display everything that was drawn onto the screen, and then wait some time so the next frame doesn't start too quickly
(because we want a constant framerate).

Typically, over the course of a single frame, all the events are run, in their order of appearance above. So, first
`_eventStartFrame()` will be called for all objects in the runtime, then `_eventPreUpdate()` will be called for all
objects in the runtime, and so on, until all events are finished. Then we start over from `StartFrame`, and repeat
the cycle until the game ends and the program exits.

#### A Note on Draw* events
You should program your objects as if the 3 `Draw*` methods arent't guaranteed to run - in other words, don't put any
critical logic in there. In some setups, `Draw*` events can sometimes skipped (for example, if the machine is slow
and can't keep up, or the game simulates 120 frames per second but the monitor can only display 60, and so on). In
others (windowless game servers), they may not be run at all, ever.

### Game objects
Apart from their specific implementations of event methods, different game objects have a few more important values:
- **Current runtime:** While the most common pattern is that an object is attached to a runtime and remains there 
until it is destroyed, it's not mandatory. You can create it 'outside' of a runtime and attach it later, or detach it
and attach it to a different one.
- **Type information:** This is just a standard object of type `type_info` (from the standard header `<typeinfo>`) 
that identifies the actual type of the object.
- **Execution priority:** When there multiple objects in the runtime (which is almost always the case), their event
methods are called in order of descending execution priority.
- **Name:** This is a string that can identify the class, identify a specific instance, or mean something else. The
QAO framework doesn't do anything with this information, so it's up to the user to assign it and use it as they see
fit (or leave it empty if it's not needed).

## Usage
Various examples with real usable code.

###Instantiating a runtime
```cpp
...
QAO_Runtime runtime; // It's that simple ;)
...
```

###Creating a simple game object class

```cpp
class MyGameObject : public QAO_Base {
public:
    MyGameObject(QAO_RuntimeRef aRuntimeRef)
        : QAO_Base(aRuntimeRef           /* Identifies a runtime to which to attach the object (if any) */
                  , typeid(MyGameObject) /* Type information */
                  , 0                    /* Execution priority */
                  , "MyGameObject"       /* Name string */
                  )
    {
    }

private:
    void _eventUpdate() override {
        _moveAround();
    }

    void _eventDraw1() override {
        _drawSelf();
    }

    ...
};

```