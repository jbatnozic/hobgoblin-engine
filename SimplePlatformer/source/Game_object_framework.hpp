#ifndef GAME_OBJECT_FRAMEWORK_HPP
#define GAME_OBJECT_FRAMEWORK_HPP

#include <Hobgoblin/QAO.hpp>
using namespace hg::qao;

#include <Hobgoblin/RigelNet.hpp>
using namespace hg::rn;

#include <typeinfo>
#define TYPEID_SELF typeid(decltype(*this))

#include <unordered_map>

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

class SynchronizedObjectMapper {
public:
    SyncId mapMasterObject(GOF_SynchronizedObject& object);
    void mapDummyObject(GOF_SynchronizedObject& object, SyncId masterSyncId);
    GOF_SynchronizedObject* getMapping(SyncId syncId) const;
    const std::unordered_map<SyncId, GOF_SynchronizedObject*>& getAllMappings() const;
    void unmap(SyncId syncId);

private:
    std::unordered_map<SyncId, GOF_SynchronizedObject*> _mappings;
    SyncId _syncIdCounter = 0;
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
                           SynchronizedObjectMapper& syncObjMapper)
        : GOF_StateObject{runtime, typeInfo, executionPriority, std::move(name)}
        , _syncObjMapper{syncObjMapper}
        , _syncId{syncObjMapper.mapMasterObject(*this)}
    {
    }

    // Constructor for Dummy object (has preassigned syncId)
    GOF_SynchronizedObject(QAO_Runtime* runtime, const std::type_info& typeInfo,
                           int executionPriority, std::string name,
                           SynchronizedObjectMapper& syncObjMapper, SyncId syncId)
        : GOF_StateObject{runtime, typeInfo, executionPriority, std::move(name)}
        , _syncObjMapper{syncObjMapper}
        , _syncId{syncId}
    {
        syncObjMapper.mapDummyObject(*this, _syncId);
    }

    virtual ~GOF_SynchronizedObject() {
        _syncObjMapper.unmap(_syncId);
    }

    SyncId getSyncId() const noexcept {
        return _syncId;
    }

    virtual void syncCreate(RN_Node& node, const std::vector<hg::PZInteger>& rec) = 0;
    virtual void syncUpdate(RN_Node& node, const std::vector<hg::PZInteger>& rec) = 0;
    virtual void syncDestroy(RN_Node& node, const std::vector<hg::PZInteger>& rec) = 0; 
    // TODO Remove this ^ (destruction sync should be automatic and without parameters)

private:
    SynchronizedObjectMapper& _syncObjMapper;
    const SyncId _syncId;
};

#endif // !GAME_OBJECT_FRAMEWORK_HPP
