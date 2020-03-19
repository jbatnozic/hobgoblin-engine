#ifndef GAME_OBJECT_FRAMEWORK_HPP
#define GAME_OBJECT_FRAMEWORK_HPP

// TODO: Rename to Game_object_framework.hpp

#include <Hobgoblin/QAO.hpp>
using namespace hg::qao;

struct GlobalProgramState;

class GOF_Base : public hg::QAO_Base {
public:
    using hg::QAO_Base::QAO_Base;

    GlobalProgramState& global() const;
    QAO_Runtime& rt() const;
    // TODO: Add more utility methods
};

// Objects which are not essential to the game's state and thus not saved (when
// writing game state) nor synchronized with clients (in multiplayer sessions).
// For example, particle effects and such.
class GOF_NonstateObject : public GOF_Base {
public:
    using GOF_Base::GOF_Base;
};

// Objects which are essential for the game's state, but will not be synchronized
// with clients. For use with singleplayer games, or for server-side controller
// objects in multiplayer games.
class GOF_StateObject : public GOF_Base {
public:
    using GOF_Base::GOF_Base;
};



// remoteId -> universalId / globalId / distributedId, [syncId] ?

class RemoteObjectMapper {
    // holds references to its runtime and rn_node;
    // assigns unique remote identifiers (RemoteId);
    // maps remote identifiers to local objects;
};

// Objects which are essential to the game's state, so they are both saved when
// writing game state, and synchronized with clients in multiplayer sessions.
// For example, units, terrain, interactible items (and, basically, most other 
// game objects).
class GOF_SynchronizedObject : public GOF_StateObject {
public:
    GOF_SynchronizedObject(RemoteObjectMapper& rom);

    virtual void onCreate();
    virtual void onUpdate();
    virtual void onDestroy();

private:
    // RemoteObjectMapper& _remoteObjectMapper;
    // RemoteID _remoteId;
};

#endif // !GAME_OBJECT_FRAMEWORK_HPP
