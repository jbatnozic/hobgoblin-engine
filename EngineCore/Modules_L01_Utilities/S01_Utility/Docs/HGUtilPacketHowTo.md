# Guide: hg::util::Packet

Hobgoblin's `Packet` class is very similar to [that from SFML](https://www.sfml-dev.org/tutorials/2.6/network-packet.php),
and almost everything that's true for that one still holds true for this one.

The main differences lie in error handling and in the method used to add support for more user-provided types.

## Error handling

### When appending

Appending is a simple case - neither the `append()` method nor `operator<<` should ever throw or report an error (to be
more precise, `std::bad_alloc` can be thrown in case of memory allocation failure, but that's an extreme case that's not
handled by this class). This assumes, of course, that any user-defined `operator<<` is implemented properly.

### When extracting

Given a `Packet` p and some type `T` where `p.getRemainingDataSize()` is 0, or less than is required to reconstruct `T`,
then attempting to extract T from p will throw `PacketExtractionError`. Similarly, calling `extractBytes` when the
packet contains no data will throw.

```cpp
Packet p;
assert(p.getRemainingDataSize() == 0);

T data;
p >> data; // throws
p.extract<T>(); // also throws
p.extractBytes(<some value greater than 0>); // also throws
```

### NoThrow variants

If you don't want to deal with exceptions, `Packet` provides `extractNoThrow<T>()` and `extractBytesNoThrow()` methods.
In case of extraction failure, the return value is undefined (for the former) or `nullptr` (for the latter). Because
the return values don't help in determining the operation's success, `Packet` provides a bool-conversion operator
which returns `false` if the packet has reached an invalid state - when this happens, throw away extracted values and
handle the error appropriately. Example:

```cpp
Packet p;
// fill packet with some data

// ...

const auto d = p.extractNoThrow<double>();
if (!p) {
    // ERROR - d is invalid
}

// If we have multiple values in the packet that together form a 'whole',
// we can check for errors only once after we attempt to extract all of them
const auto i1 = p.extractNoThrow<std::int16_t>();
const auto i2 = p.extractNoThrow<std::int32_t>();
const auto i3 = p.extractNoThrow<std::int64_t>();

if (!p) {
    // ERROR: at least i3 is invalid, i2 and i1 could be as well, but as these values
    // are not useful unless all of them are present, this doesn't matter
}
```

**Note:** After a packet reacher an invalid state, the only reasonable thing to do is either destroy it or `clear()` it.

As we've established, operators >> in the `Packet` class throw on failure (operator<< doesn't throw and isn't relevant
here). `Packet` has the `noThrow()` method which returs a proxy object which also defines templated operators >> but
which don't throw.

```cpp
// ALL OF THE FOLLOWING ARE EQUIVALENT
{
    Packet p;
    // fill packet with some data
    const auto i = p.extractNoThrow<std::int32_t>();
    const auto s = p.extractNoThrow<std::string>();
    if (!p) {
        // ERROR
    }
}
{
    Packet p;
    // fill packet with some data
    std::int32_t i;
    std::string  s;
    p.noThrow() >> i >> s;
    if (!p) {
        // ERROR
    }
}
{
    Packet p;
    // fill packet with some data
    std::int32_t i;
    std::string  s;
    if (!(p.noThrow() >> i >> s)) {
        // ERROR
    }
}
```

**Note:** The proxy object returned by `noThrow()` also supports operator<< for consistency, but, again, this one
doesn't throw either way.

## Extending hg::util::Packet to handle user types

- when writing operator << and >> overloads, use `PacketExtender` instead of `Packet` (it mimics a pointer to `Packet`
though it's never NULL)
- the overloaded operator shouldn't throw, so implement it in terms of `noThrow()` or `extractNoThrow()` (if,
by mistake, it does throw, this will be caught, but it could have a performance penalty and a warning will be logged)

```cpp
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Utility/Packet.hpp>

struct MyStruct {
    std::int32_t i;
    std::string  s;
};

hg::Packet& operator<<(hg::PacketExtender& aPacket, const MyStruct& aData) {
    *aPacket << aData.i << aData.s;
    return *aPacket;
    // OR
    return (*aPacket << aData.i << aData.s);
    // OR
    return (aPacket->noThrow() << aData.i << aData.s);
    // OR
    aPacket->append(aData.i);
    aPacket->append(aData.s);
    return *aPacket;
}

hg::Packet& operator>>(hg::PacketExtender& aPacket, MyStruct& aData) {
    aPacket->noThrow() >> aData.i >> aData.s;
    return *aPacket;
    // OR
    return (aPacket->noThrow() >> aData.i >> aData.s);
    // OR
    aData.i = aPacket->extractNoThrow<std::int32_t>();
    aData.s = aPacket->extractNoThrow<std::string>();
    return *aPacket;

    // WRONG (uses throwing variants)
    *aPacket >> aData.i >> aData.s;
    return *aPacket;
    // WRONG (uses throwing variants)
    aData.i = aPacket->extract<std::int32_t>();
    aData.s = aPacket->extract<std::string>();
    return *aPacket;
}
```
### Or use HG_ENABLE_AUTOPACK

TODO
