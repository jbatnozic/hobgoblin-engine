#ifndef GAME_OBJECT_FRAMEWORK_HPP
#define GAME_OBJECT_FRAMEWORK_HPP

#include <Hobgoblin/QAO.hpp>
using namespace hg::qao;

#include <Hobgoblin/RigelNet.hpp>
using namespace hg::rn;

#include <Hobgoblin/Utility/NoCopyNoMove.hpp>

#include <typeinfo>
#include <unordered_map>
#include <unordered_set>

#include "Graphics/Sprites.hpp"
#include "Utility/Keyboard_input.hpp"

#define TYPEID_SELF typeid(decltype(*this))

class GameContext;

class GOF_Base : public hg::QAO_Base {
public:
    using hg::QAO_Base::QAO_Base;

    GameContext& ctx() const;
    KbInputTracker& kbi() const;
    const hg::gr::Multisprite& getspr(SpriteId spriteId) const;
    // TODO: Add more utility methods
};

// I:
// Objects which are not essential to the game's state and thus not saved (when
// writing game state) nor synchronized with clients (in multiplayer sessions).
// For example, particle effects and such.
// II:
// Controllers which are always created when the game starts and thus always
// implicitly present, so we don't need to save them.
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
constexpr SyncId SYNC_ID_CREATE_MASTER = 0;

class GOF_SynchronizedObject;

class SynchronizedObjectManager : public hg::util::NonCopyable, public hg::util::NonMoveable {
    // TODO Cover edge case when an object is created and then immediately destroyed (in the same step)
public:
    SynchronizedObjectManager(RN_Node& node);

    void setNode(RN_Node& node);

    SyncId registerMasterObject(GOF_SynchronizedObject* object);
    void registerDummyObject(GOF_SynchronizedObject* object, SyncId masterSyncId);
    void unregisterObject(GOF_SynchronizedObject* object);

    GOF_SynchronizedObject* getMapping(SyncId syncId) const;

    void syncObjectCreate(const GOF_SynchronizedObject* object);
    void syncObjectUpdate(const GOF_SynchronizedObject* object);
    void syncObjectDestroy(const GOF_SynchronizedObject* object);

    void syncStateUpdates();
    void syncCompleteState(hg::PZInteger clientIndex);

private:
    std::unordered_map<SyncId, GOF_SynchronizedObject*> _mappings;
    std::unordered_set<const GOF_SynchronizedObject*> _newlyCreatedObjects;
    std::unordered_set<const GOF_SynchronizedObject*> _alreadyUpdatedObjects;
    std::unordered_set<const GOF_SynchronizedObject*> _alreadyDestroyedObjects;
    std::vector<hg::PZInteger> _recepientVec;
    SyncId _syncIdCounter = 2;
    RN_Node* _node;
};

// Objects which are essential to the game's state, so they are both saved when
// writing game state, and synchronized with clients in multiplayer sessions.
// For example, units, terrain, interactible items (and, basically, most other 
// game objects).
class GOF_SynchronizedObject : public GOF_StateObject {
public:
    // TODO: Implement methods in .cpp file

    // if syncId.has_value() == true, create dummy object. Otherwise, create
    // master object.
    GOF_SynchronizedObject(QAO_RuntimeRef runtimeRef, const std::type_info& typeInfo,
                           int executionPriority, std::string name,
                           SynchronizedObjectManager& syncObjMgr, SyncId syncId = SYNC_ID_CREATE_MASTER)
        : GOF_StateObject{runtimeRef, typeInfo, executionPriority, std::move(name)}
        , _syncObjMgr{syncObjMgr}
        , _syncId{(syncId == SYNC_ID_CREATE_MASTER) ? _syncObjMgr.registerMasterObject(this) : syncId}
    {
        _syncObjMgr.registerDummyObject(this, _syncId);
    }

    virtual ~GOF_SynchronizedObject() {
        _syncObjMgr.unregisterObject(this);
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
