#ifndef GAME_OBJECT_FRAMEWORK_HPP
#define GAME_OBJECT_FRAMEWORK_HPP

#include <Hobgoblin/QAO.hpp>
using namespace hg::qao;

#include <Hobgoblin/RigelNet.hpp>
using namespace hg::rn;

#include <typeinfo>
#define TYPEID_SELF typeid(decltype(*this))

#include <unordered_map>
#include <unordered_set>

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

using SyncId = std::int64_t;
class GOF_SynchronizedObject;

class SynchronizedObjectManager {
    // TODO Cover edge case when an object is created and then immediately destoryed (in the same step)
public:
    SynchronizedObjectManager(RN_Node& node);

    SyncId createMasterObject(GOF_SynchronizedObject* object);
    void createDummyObject(GOF_SynchronizedObject* object, SyncId masterSyncId);
    void destroyObject(GOF_SynchronizedObject* object);

    GOF_SynchronizedObject* getMapping(SyncId syncId) const;
    void syncAll();
    void syncAllToNewClient(hg::PZInteger clientIndex);

    void syncObjectCreate(const GOF_SynchronizedObject* object);
    void syncObjectUpdate(const GOF_SynchronizedObject* object);
    void syncObjectDestroy(const GOF_SynchronizedObject* object);

private:
    std::unordered_map<SyncId, GOF_SynchronizedObject*> _mappings;
    std::unordered_set<const GOF_SynchronizedObject*> _newlyCreatedObjects;
    std::unordered_set<const GOF_SynchronizedObject*> _alreadyUpdatedObjects;
    std::unordered_set<const GOF_SynchronizedObject*> _alreadyDestroyedObjects;
    std::vector<hg::PZInteger> _recepientVec;
    SyncId _syncIdCounter = 0;
    RN_Node& _node;
};

// Objects which are essential to the game's state, so they are both saved when
// writing game state, and synchronized with clients in multiplayer sessions.
// For example, units, terrain, interactible items (and, basically, most other 
// game objects).
class GOF_SynchronizedObject : public GOF_StateObject {
public:
    // TODO: Implement methods in .cpp file

    // Constructor for Master object
    GOF_SynchronizedObject(QAO_Runtime* runtime, const std::type_info& typeInfo, 
                           int executionPriority, std::string name, 
                           SynchronizedObjectManager& syncObjMapper)
        : GOF_StateObject{runtime, typeInfo, executionPriority, std::move(name)}
        , _syncObjMgr{syncObjMapper}
        , _syncId{syncObjMapper.createMasterObject(this)}
    {
    }

    // Constructor for Dummy object (has preassigned syncId)
    GOF_SynchronizedObject(QAO_Runtime* runtime, const std::type_info& typeInfo,
                           int executionPriority, std::string name,
                           SynchronizedObjectManager& syncObjMapper, SyncId syncId)
        : GOF_StateObject{runtime, typeInfo, executionPriority, std::move(name)}
        , _syncObjMgr{syncObjMapper}
        , _syncId{syncId}
    {
        syncObjMapper.createDummyObject(this, _syncId);
    }

    virtual ~GOF_SynchronizedObject() {
        _syncObjMgr.destroyObject(this);
    }

    SyncId getSyncId() const noexcept {
        return _syncId & ~std::int64_t{1};
    }

    bool isMasterObject() const noexcept {
        return (_syncId & 1) != 0;
    }

    virtual void syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const = 0;
    virtual void syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const = 0;
    virtual void syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const = 0;

    void syncCreate() const;
    void syncUpdate() const;
    void syncDestroy() const;

private:
    SynchronizedObjectManager& _syncObjMgr;
    const SyncId _syncId;
};

#endif // !GAME_OBJECT_FRAMEWORK_HPP
