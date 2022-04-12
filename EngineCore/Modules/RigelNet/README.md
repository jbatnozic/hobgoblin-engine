# Rigel Net
`#include <Hobgoblin/RigelNet.hpp>`

`#include <Hobgoblin/RigelNet_macros.hpp> // Optional`

RigelNet is a RPC (Remote Procedure Calling) framework for games.

## Concept
RigelNet is centred around 2 main concepts: Nodes and Messages. A Node can be either a Server or a Client, and it's
intended for one or more Client nodes to connect to a central Server node (and Clients never communicate amongst
each other). Messages, or RPCs, on the other hand, are basically pieces of code that you want to execute on the
remote end with some provided parameters.

You will notice that there are no blocking calls anywhere in RigelNet, and instead everything relies on polling and
manual updates. That's because RigelNet is specifically suited to working together with QAO, in a fixed timestep
environment.

## The RigelNet Registry
TODO

Always call `RN_IndexHandlers();` as soon as the program starts, before doing anything else with RigelNet.

## Nodes
To start things off, we need to instantiate and connect some nodes.

### Server side
To instantiate a server node, we use the `createServer` static method of the `RN_ServerFactory` class.

```cpp
static std::unique_ptr<RN_ServerInterface> createServer(
    RN_Protocol aProtocol,
    std::string aPassphrase,
    PZInteger aServerSize,
    PZInteger aMaxPacketSize,
    RN_NetworkingStack aNetworkingStack);
```

The argumets are:
- **aProtocol** - For the underlying protocol, you can choose either TCP or UDP (as of the time of writing this, only 
  UDP is supported). No matter which you choose, RigelNet guarantees that all sent messages will be delivered and in the
  correct order too.
- **aPassphrase** - A passphrase that clients will have to match in order to successfully connect. This is NOT a 
  security feature! It's only there to prevent mismatched versions of the same program from connecting, or even
  different programs altogether from connecting. Because of that, the passphrase should be something like
  `"<developer>::<project>::<version>"`.
- **aServerSize** - How many clients can connect to the server at any given time. Must be at least 1.
- **aMaxPacketSize** - Maximum size (in bytes) of packets which the server will try to send (larger ones will be split
  and reassembled upon delivery automatically). This setting only really makes sense if you choose UDP as the protocol, 
  and doesn't affect a TCP server. As for the value, the theoretical limit is 64kB, but you shouldn't go for more than
  a few kB because different operating systems and network configurations support wildly varying sizes.
- **aNetworkingStack** - The networking stack used to send and receive data. This guide will focus on the `Default` one,
  which is the one provided by the host operating system, and the one you're most likely to be using. There is also an
  experimental `ZeroTier` one, based on [libzt](https://github.com/zerotier/libzt).

```cpp
// Example call
auto server = RN_ServerFactory::createServer(
    RN_Protocol::UDP,
    "jbatnozic::hobgoblin::v.1.2.3",
    4,
    1024,
    RN_NetworkingStack::Default
);
```

Once we've instantiated a server instance, we need to configure it and start it:

```cpp
auto server = RN_ServerFactory::createServer(...);

// If a client isn't responding for longer than this time,
// it will be kicked by the server automatically
server->setTimeoutLimit(std::chrono::microseconds{5'000'000L});

// TODO (this part is optional anyway)
server->setRetransmitPredicate(...);

// Start listening for connections on the given port (use 0 to use "any available port")
server->start(0);
```

### Client side
To instantiate a client node, we use the `createClient` static method of the `RN_ClientFactory` class.

```cpp
static std::unique_ptr<RN_ClientInterface> createClient(
    RN_Protocol aProtocol,
    std::string aPassphrase,
    PZInteger aMaxPacketSize,
    RN_NetworkingStack aNetworkingStack);
```

The argumets are:
- **aProtocol** - (see above)
- **aPassphrase** - (see above)
- **aMaxPacketSize** - (see above)
- **aNetworkingStack** - (see above)

```cpp
// Example call
auto server = RN_ClientFactory::createClient(
    RN_Protocol::UDP,
    "jbatnozic::hobgoblin::v.1.2.3",
    1024,
    RN_NetworkingStack::Default
);
```

Once we've instantiated a client instance, we need to configure it and connect it to a server:

```cpp
auto client = RN_ClientFactory::createClient(...);

// If the server isn't responding for longer than this time,
// the client will act as if it was disconnected
client->setTimeoutLimit(std::chrono::microseconds{5'000'000L});

// TODO (this part is optional anyway)
client->setRetransmitPredicate(...);

// Start connecting to a server. The arguments are, in order:
// - local port (can use 0 as "any available port")
// - server's IP address
// - server's port
client->connect(0, "127.0.0.1", 8888);
```

## Updating the Nodes
Once you've got your node, no matter if it's a Server or a Client, you need to periodically update it if it's to do
anything. To do this, use the `update` method. For example:

```cpp
auto node = ...;
node->update(RN_UpdateMode::Receive);
node->update(RN_UpdateMode::Send);
```

It's recommended to call `update` two times per frame: Once near the beginning of the frame to receive, and once near
the end of the frame to send. If you're also using QAO, receiving should happen during the `PreUpdate` event, and
sending should happen during the `PostUpdate` event (and obviously your program should update its state inbetween).

### Polling for networking events
After each call to a node's `update` method, you should poll the node for any eventual networking events which might
have happened during the updating process. These events include mostly stuff like remote nodes connecting and
disconnecting.

```cpp
auto node = ...;
RN_Event ev;
while (node->pollEvent(ev)) {
    // handle event
    // TODO (how?)
}
```

## Defining and sending Messages
\<TODO\>