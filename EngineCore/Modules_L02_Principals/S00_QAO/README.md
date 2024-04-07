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

|  # | Event         | Method                  | Step    |
|----|---------------|-------------------------|---------|
|  1 | PRE_UPDATE    | `_eventPreUpdate()`     | Update  |
|  2 | BEGIN_UPDATE  | `_eventBeginUpdate()`   | Update  |
|  3 | UPDATE_1      | `_eventUpdate1()`       | Update  |
|  4 | UPDATE_2      | `_eventUpdate2()`       | Update  |
|  5 | END_UPDATE    | `_eventEndUpdate()`     | Update  |
|  6 | POST_UPDATE   | `_eventPostUpdate()`    | Update  |
|  7 | PRE_DRAW      | `_eventPreDraw()`       | Draw    |
|  8 | DRAW_1        | `_eventDraw1()`         | Draw    |
|  9 | DRAW_2        | `_eventDraw2()`         | Draw    |
| 10 | DRAW_GUI      | `_eventDrawGUI()`       | Draw    |
| 11 | POST_DRAW     | `_eventPostDraw()`      | Draw    |
| 12 | DISPLAY       | `_eventDisplay()`       | Display |

**Note:** All of these methods are private. But don't worry, in C++ it is possible to override private methods
(furthermore, it's a good practice).

While you can write any code for any given event, each has a specific intended purpose, and you should stick to 
these guidelines for best results:
- **PRE_UPDATE:** Event for code that absolutely must execute before anything else. You should rarely need this event.
- **BEGIN_UPDATE:** Objects prepare for the upcoming Update(s) - actions like gathering user input, reading network
messages and similar are appropriate for this event. Objects should interact as little as possible here.
- **UPDATE_1/2:** These are the "main" events where most of the work happens. Ideally, all objects prepared themselves
during `PRE_UPDATE` and `BEGIN_UPDATE`, so they are free to interact with each other (including creating new objects or
deleting existing ones).
- **END_UPDATE:** In this event, we process the results of the preceding `Update1/2` events. In a multiplayer game,
this would be where the server uploads updates to clients. You can also view it as a preparation for the `Draw*`
events. Like in `BEGIN_UPDATE`, interactions between objects should be minimal.
- **POST_UDPATE:** This is another rarely needed event, sometimes used to 'clean up' the results of the whole
Update step. Again, no object interactions are expected here.
- **PRE_DRAW:** This is the analogue to `PRE_UPDATE` but for the drawing step; this event is used to prepare the game
state for the drawing step, if needed (for example: clear the target window). No actual drawing should be performed
in this event.
- **DRAW_1/2:** In this event the game world is drawn. You can have each object resposible for drawing its own state,
or have one object that draws everything. Both (`DRAW_1` and `DRAW_2`) events are functionally the same, but because
things drawn later appear on top of those drawn before, it can sometimes be useful to have 2 separate drawing events
so it's easier to order everything properly.
- **DRAW_GUI:** A GUI always goes on top of everything else, so it's natural to draw it last. (Also, it's worth noting
that a GUI is usually drawn relative to the 'window' coordinate system and not the 'game world' coordinate system.)
- **POST_DRAW:** No more actual drawing should be performed after `DRAW_GUI`, but `POST_DRAW` can be used to clean up
after the drawing step if needed.
- **DISPLAY:** Usually you'll only have two predefined actions in this event: 1) display everything that was drawn onto
the screen; 2) if there is still time until the expected start of the next iteration, the program can sleep or wait for
V-Sync. (Point 2 can be skipped in games with a variable timestep, though QAO was really made with a constant timestep
in mind.)

Typically, over the course of a single iteration, all the events are run, in their order of appearance above. So, first
`_eventPreUpdate()` will be called for all objects in the runtime, then `_eventBeginUpdate()` will be called for all
objects in the runtime, and so on, until all events are finished. Then we start over from `_eventPreUpdate()`, and 
repeat the cycle until the game ends and the program exits.

#### Logical Steps

As shown in the table above, the events are split into three logical groups: **Update**, **Draw** and **Display**.
Now, the QAO library itself doesn't implement any event loop, but there are recommendations as to how it should be
used for best results (and that's the way it's implemented in
[SPeMPE](https://github.com/jbatnozic/Hobgoblin/tree/master/Overlays/SPeMPE)).
- The **Update** step is the "meat" of your program - all critical logic happens in here. In a fixed timestep loop
(such as what SPeMPE implements), the program should prioritize invoking the step method a constant number of times
per second (60 or 120 for example) and with as even cadence as possible (ideally you want exactly 1/60th of a
second to pass between every start of an Update step in a 60fps game). A case which is the same for both fixed and
variable timestep is when the host machine isn't fast enough to produce the required framerate - in this case the
Update step should just be called as fast as possible.
- The **Draw** step should be used exclusively for drawing stuff onto the screen, _and_ what's drawn should be
dependent only on the results produced in the Update step. The reason for this is that the Draw step isn't
guaranteed to run every time an Update step finishes. For example, if a game with V-Sync that's designed to run at
120fps is running on a monitor that can only display 60fps, it makes sense to do 2 Update steps per one Draw/Display
step. On the other hand, on a slow computer that's struggling to produce the correct framerate, the game engine may
decide to occasionally skip the Draw step in an effort to keep up. Finally, in some deployments, such as headless
game servers, the Draw step could be just not run at all, ever. This is why any code in the Draw step should only
read, but not change the game state - otherwise some of the usecases described above may be broken.
- The main purpose of the **Display** step (hence the name) is to just display whatever was drawn in the Draw step
onto the screen, and that's almost all there is to it. On the other hand, despite the name, the Display step should
run even on deployments (again, such as headless game servers) that don't draw/display anything (and maybe don't even
open a window at all). A game running on a computer that's much faster than the game required will end up going
through the Display step multiple times between two consecutive Update/Draw steps. A way to avoid this is to wait
for V-Sync or otherwise make the program sleep in the Display step. This is why the Display step should run even
on headless deployments - in this way we can have the timing code universally in the same place.ß

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
implement vSync or some other timing mechanism.
[SPeMPE](https://github.com/jbatnozic/Hobgoblin/tree/master/Overlays/SPeMPE)'s `WindowManager` can also help with
this.

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