# Rigel Net
`#include <Hobgoblin/RigelNet.hpp>`

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
To define a Message, first you must put `#include <Hobgoblin/RigelNet_macros.hpp>` in your source file. It's recommended to include this only in .cpp files and not headers, because it will pollute the global namespace with a huge amount of macro definitions.

The simplest form of a Message definition is as follows:

```cpp
RN_DEFINE_RPC(MyCoolRpc) {
    // This code will be executed on the remote end
    std::cout << "I have received MyCoolRpc!\n";
}
```

The `RN_DEFINE_RPC` macro will also generate the appropriate `Compose_<name>` function, in this case `Compose_MyCoolRpc`, which we can use to instruct the local node to send the message to a remote node and execute the appropriate code. For example:

```cpp
RN_NodeInterface& node = ...;
Compose_MyCoolRpc(node, RN_COMPOSE_FOR_ALL);
...
node.update(RN_UpdateMode::Send);
```

The first argument of any `Compose_*` function is the node which is supposed to send the message, and the second argument tells it to which remote to send to. In the case of a Client node, it really doesn't matter, as it can only send to its Server, so it disregards this argument (so you can put 0, or RN_COMPOSE_FOR_ALL, or any other integer). In the case of a Server, however, there are a few different ways to handle this argument:
- Put RN_COMPOSE_FOR_ALL - this will compose the message to all currently connected clients.
- Put an integer - 0 will compose for client with index 0, 1 for client with index 1 etc (up to server size - 1).
- Put any forward iterable object (such as a vector) whose element type is an integer or is implicitly convertible to an integer. Each number provided by this object will compose for a client with that index.

**Important:** Note that calling `Compose_*` does NOT immediately send a Message/RPC. It only adds it to the sending queue, and all queued Messages are sent only when you call `.update(RN_UpdateMode::Send)` on the node. On the remote side, Messages are not received and handled asychronously, but instead, only when you call `.update(RN_UpdateMode::Receive)` on the receiving node. When you do, Messages are interpreted and their bodies are executed in the order in which they were composed and sent (in the case of a Server node, first all Messages from client 0 are processed, then all Messages from client 1, and so on...). An example is given below:

```cpp
RN_NodeInterface& server = ...;
RN_NodeInterface& client = ...;

ConnectClientToServer(client, server); // Made up function for demonstration purposes

Compose_MyCoolRpc(server, RN_COMPOSE_FOR_ALL); // Puts the Message in the queue of node 'server'

server.update(RN_UpdateMode::Send); // Send all queued messages to appropriate recepients

client.update(RN_UpdateMode::Receive); // Unpacks and executes all received messages in order of sending
                                       // In this case, only the body of MyCoolRpc is executed, which
                                       // results in printing `"I have received MyCoolRpc!\n"` to `cout`.

```

### Handling Messages differently on the Server and Client sides
TODO

### Accessing program state from Message bodies
TODO

### Message arguments
TODO
