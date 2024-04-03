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
| 2 | PreUpdate     | `_eventBeginUpdate()`     |
| 3 | Update        | `_eventUpdate()`        |
| 4 | PostUpdate    | `_eventEndUpdate()`    |
| 5 | Draw1         | `_eventDraw1()`         |
| 6 | Draw2         | `_eventDraw2()`         |
| 7 | DrawGUI       | `_eventDrawGUI()`       |
| 8 | Display       | `_eventDisplay()` |

**Note:** All of these methods are private. But don't worry, in C++ it is possible to override private methods
(furthermore, it's a good practice).

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
sometimes be useful to have 2 separate Draw events to it's easier to order everything properly.
- **DrawGUI:** A GUI always goes on top of everything else, so it's natural to draw it last. (Also, it's worth noting
that a GUI is usually drawn relative to the 'window' coordinate system and not the 'game world' coordinate system.)
- **Display:** Code that absolutely must run after everything else. Usually you'll only have 2 actions here: 
display everything that was drawn onto the screen, and then wait some time so the next frame doesn't start too quickly
(if we want a constant framerate).

Typically, over the course of a single frame, all the events are run, in their order of appearance above. So, first
`_eventStartFrame()` will be called for all objects in the runtime, then `_eventBeginUpdate()` will be called for all
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

### Instantiating a runtime
```cpp
...
QAO_Runtime runtime; // It's that simple ;)
...
```

### Getting a QAO_RuntimeRef
`QAO_RuntimeRef` is a small object that identifies (refers to) a `QAO_Runtime` object. We need one in order to
instantiate `QAO_Base` (and anything derived from it).
```cpp
// Given a runtime 'rt'...
QAO_Runtime rt;

// We can construct a runtime ref in the following ways:
QAO_RuntimeRef ref1{rt};  // (passing a reference to a runtime)

QAO_RuntimeRef ref2{&rt}; // We can also pass a pointer (which can be null, and makes
                          // a valid QAO_RuntimeRef that simply refers to no runtime).

QAO_RuntimeRef ref3{nullptr}; // (See above.) When a null runtime ref is passed to a QAO_Base,
                              // it doesn't get attached to a runtime.

QAO_RuntimeRef ref4{rt.nonOwning()}; // This one is diferent from the other examples above, because
                                     // it makes a NON-OWNING runtime ref, meaning that when it is
                                     // passed to a QAO_Base constructor, it will get attached to a
                                     // runtime, but that runtime will know that it doesn't own the
                                     // registered instance, and so won't try to delete it if it is
                                     // deleted itself.
```

### Creating a simple game object class

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

### Creating instances of a game object class & interactions with a runtime

```cpp
QAO_Runtime rt;

// Creates an instance of MyGameObject (all arguments passed to QAO_PCreate are passed directly to
// MyGameObject constructor, same as std::make_unique) and attaches it to the runtime. Returns a
// non-owning pointer to the created instance. 
MyGameObject* obj1 = QAO_PCreate<MyGameObject>(&rt);

// TODO(description pending)
QAO_Id<MyGameObject> obj2 = QAO_ICreate<MyGameObject(&rt);

// Creates an instance of MyGameObject and attaches it to the runtime. The runtime will not own
// the instance, so the function returns an owning (unique) pointer to the created instance. 
std::unique_ptr<MyGameObject> obj3 = QAO_PCreate<MyGameObject>(rt.nonOwning());

// This will throw an exception (we pass a non-owning ref, but QAO_PCreate returns a non-owning
// pointer - so who owns the object?).
MyGameObject* obj4 = QAO_PCreate<MyGameObject>(rt.nonOwning());

// (same as above) 
QAO_Id<MyGameObject> obj5 = QAO_ICreate<MyGameObject(rt.nonOwning());

// This will throw an exception (we pass an owning ref, but QAO_UPCreate returns an owning
// pointer - but there can be only one owner).
std::unique_ptr<MyGameObject> obj6 = QAO_PCreate<MyGameObject>(rt);

// We can also instantiate the object manually (yes, even on the stack). Unless the object was
// allocated on the heap using 'new' (which you shouldn't do), we must pass a non-owning
// reference - otherwise the runtime will try to call 'delete' on the object if it gets
// destroyed, and you will have a bad time.
// Note that QAO_Base is not copyable nor moveable, so the practicality of this is moot. But it
// is definitely possible.
MyGameObject myGameObject{rt.nonOwning()};

// Objects that were created using 'QAO_*Create' functions can be destroyed with 'QAO_Destroy()'.
// We pass whatever was returned by 'QAO_*Create' (in case of `QAO_ICreate` we must also pass
// a runtime where the object currently is).
QAO_Destroy(obj1);            // equivalent to 'delete obj';
QAO_Destroy(obj2);            // equivalent to 'delete runtime.find(obj2)';
QAO_Destroy(std::move(obj3)); // equivalent to 'obj3.reset()';
```

### Working with IDs
**(TODO)**

### Running the runtime
```cpp
QAO_Runtime rt;

AddSomeObjectsToRuntime(rt); // You implement this part yourself

// Run a single frame
rt.startStep();
bool done = false;
rt.advanceStep(done); // Done will be true if the step finished successfully, and false
                      // if advanceStep() was exited due to an exception. If you catch and
                      // handle exceptions, you can keep calling advanceStep(), and it will
                      // continue where it left off (retrying the same event on the object
                      // that threw an exception, if it still exists, or the first one after
                      // it), until done becomes true.

// Run 10 frames and continue on exception
for (int i = 0; i < 10; i += 1) {
    runtime.startStep();
    bool done = false;
    do {
        try {
            rt.advanceStep(done);
        }
        catch (...) {
            // swallow all exceptions (in general, a very bad idea!)
        }
    } while (!done);
}

// Event flags
// TODO

```

Most commonly, you'll be running the `startStep()`/`advanceStep()` combo in an infinite loop, and break only when
some external condition is met (user clicked X, pressed Escape, etc). However, QAO itself doesn't keep track of time
at all. If you want a consistent framerate (for example, if your game doesn't use delta time), it is up to you to 
implement vSync or some other timing mechanism. [SPeMPE](https://github.com/jbatnozic/Hobgoblin#hobgoblin)'s
`WindowManager` can also help with this.

### Inspecting objects within a runtime
**(TODO)**

### Priority resolvers
Since it can be very important to set up execution priorities for all objects inside of a runtime correctly, QAO
includes two classes that can help with that: `QAO_PriorityResolver` and (very imaginatively) `QAO_PriorityResolver2`.

**(TODO)**

## Miscellaneous information

### Namespaces
By default, all QAO symbols are present in both `::jbatnozic::hobgoblin` and `::jbatnozic::hobgoblin::qao`
namespaces. Like all Hobgoblin modules, if `HOBGOBLIN_SHORT_NAMESPACE` is defined before including the headers,
you will get an alias to `hg`. So there are a few ways to go about this:

```cpp
#include <Hobgoblin/QAO.hpp>
...
{
  jbatnozic::hobgoblin::QAO_Runtime rt1;
  jbatnozic::hobgoblin::qao::QAO_Runtime rt2;
}
```

```cpp
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/QAO.hpp>
...
{
  hg::QAO_Runtime rt1;
  hg::qao::QAO_Runtime rt2;
}
```


```cpp
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/QAO.hpp>
// All the names are prefixed with QAO_ so this isn't so bad
using namespace hg::qao; 
...
{
  QAO_Runtime rt;
}
```