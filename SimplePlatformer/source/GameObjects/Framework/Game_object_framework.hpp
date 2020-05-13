#ifndef GAME_OBJECT_FRAMEWORK_HPP
#define GAME_OBJECT_FRAMEWORK_HPP

#include <Hobgoblin/Utility/NoCopyNoMove.hpp>

#include "GameObjects/Framework/Common.hpp"
#include "GameObjects/Framework/Synchronized_object_registry.hpp"

#include "Graphics/Sprites.hpp"
#include "Utility/Keyboard_input.hpp"

// ///////////////////////////////////////////////////////////////////////////////////////////// //
// TYPE HIERARCHY

class GameContext;

class GOF_Base : public hg::QAO_Base {
public:
    using hg::QAO_Base::QAO_Base;

    GameContext& ctx() const;
    KbInputTracker& kbi() const;
    const hg::gr::Multisprite& getspr(SpriteId spriteId) const;
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

// Objects which are essential to the game's state, so they are both saved when
// writing game state, and synchronized with clients in multiplayer sessions.
// For example, units, terrain, interactible items (and, basically, most other 
// game objects).
class GOF_SynchronizedObject : public GOF_StateObject {
public:
    // TODO: Implement methods in .cpp file

    GOF_SynchronizedObject(QAO_RuntimeRef runtimeRef, const std::type_info& typeInfo,
                           int executionPriority, std::string name,
                           GOF_SynchronizedObjectRegistry& syncObjReg, GOF_SyncId syncId = GOF_SYNC_ID_CREATE_MASTER);

    virtual ~GOF_SynchronizedObject();

    GOF_SyncId getSyncId() const noexcept;

    bool isMasterObject() const noexcept;

protected:
    void syncCreate() const;
    void syncUpdate() const;
    void syncDestroy() const;

private:
    GOF_SynchronizedObjectRegistry& _syncObjReg;
    const GOF_SyncId _syncId;

    virtual void syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const = 0;
    virtual void syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const = 0;
    virtual void syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const = 0;

    friend class GOF_SynchronizedObjectRegistry;
};

// ///////////////////////////////////////////////////////////////////////////////////////////// //
// CANNONICAL FORMS

// A Synchronized object has the cannonical forms if it fulfils the following criteria:
// + Inherits from GOF_SynchronizedObject
// + Publicly defines a nested struct named "VisibleState" which has defined operators << and >> for insertion and
//   extraction to and from a hg::util::Packet
// + Its entire visible (observable by the user/player) state can be described with an instance of the VisibleState
//   struct
// + It defines a constructor with the following form:
//   ClassName(QAO_RuntimeRef rtRef, GOF_SynchronizedObjectRegistry& syncObjReg,
//             GOF_SyncId syncId, const VisibleState& initialState);
//   (NOTE: to overcome the limitation of the fixed constructor form, we can utilize the two-step initialization
//   pattern using init- methods)
// + Is always created on the heap using QAO_PCreate or QAO_ICreate
// + Publicly defnes a method void cannonicalSyncApplyUpdate(const VisibleState& state, int delay) which will be
//   called when a new state description is received from the server
// + Defines a method (with any access modifier) with the following signature:
//   const VisibleState& getCurrentState() const;
//
// If a Synchronized object has the cannonical form, the following macros can be used to generate appropriate
// network handlers for that type

#define GOF_GENERATE_CANNONICAL_HANDLERS(_class_name_) \
    RN_DEFINE_HANDLER(Create##_class_name_, RN_ARGS(GOF_SyncId, syncId, _class_name_::VisibleState&, state)) { \
        GOF_CannonicalCreateImpl<_class_name_, GameContext>(RN_NODE_IN_HANDLER(), syncId, state); \
    } \
    RN_DEFINE_HANDLER(Update##_class_name_, RN_ARGS(GOF_SyncId, syncId, _class_name_::VisibleState&, state)) { \
        GOF_CannonicalUpdateImpl<_class_name_, GameContext>(RN_NODE_IN_HANDLER(), syncId, state); \
    } \
    RN_DEFINE_HANDLER(Destroy##_class_name_, RN_ARGS(GOF_SyncId, syncId)) { \
        GOF_CannonicalDestroyImpl<_class_name_, GameContext>(RN_NODE_IN_HANDLER(), syncId); \
    }

#define GOF_GENERATE_CANNONICAL_SYNC_DECLARATIONS \
    void syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override; \
    void syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override; \
    void syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;

#define GOF_GENERATE_CANNONICAL_SYNC_IMPLEMENTATIONS(_class_name_) \
    void _class_name_::syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const { \
        Compose_Create##_class_name_(node, rec, getSyncId(), getCurrentState()); \
    } \
    void _class_name_::syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const { \
        Compose_Update##_class_name_(node, rec, getSyncId(), getCurrentState()); \
    } \
    void _class_name_::syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const { \
        Compose_Destroy##_class_name_(node, rec, getSyncId()); \
    }

template <class T, class TCtx>
void GOF_CannonicalCreateImpl(RN_Node& node, GOF_SyncId syncId, typename T::VisibleState& state) {
    node.visit(
        [&](RN_UdpClient& client) {
            auto& ctx = *client.getUserData<TCtx>();
            auto& runtime = ctx.qaoRuntime;
            auto& syncObjReg = ctx.syncObjReg;
            QAO_PCreate<T>(&runtime, syncObjReg, syncId, state);
        },
        [](RN_UdpServer& server) {
            // TODO ERROR
        }
    );
}

template <class T, class TCtx>
void GOF_CannonicalUpdateImpl(RN_Node& node, GOF_SyncId syncId, typename T::VisibleState& state) {
    node.visit(
        [&](RN_UdpClient& client) {
            auto& ctx = *client.getUserData<TCtx>();
            auto& runtime = ctx.qaoRuntime;
            auto& syncObjReg = ctx.syncObjReg;
            auto& object = *static_cast<T*>(syncObjReg.getMapping(syncId));

            const std::chrono::microseconds delay = client.getServer().getRemoteInfo().latency / 2LL;
            object.cannonicalSyncApplyUpdate(state, ctx.calcDelay(delay));
        },
        [](RN_UdpServer& server) {
            // TODO ERROR
        }
    );
}

template <class T, class TCtx>
void GOF_CannonicalDestroyImpl(RN_Node& node, GOF_SyncId syncId) {
    node.visit(
        [&](RN_UdpClient& client) {
            auto& ctx = *client.getUserData<TCtx>();
            auto& runtime = ctx.qaoRuntime;
            auto& syncObjReg = ctx.syncObjReg;
            auto* object = static_cast<T*>(syncObjReg.getMapping(syncId));

            QAO_PDestroy(object);
        },
        [](RN_UdpServer& server) {
            // TODO ERROR
        }
    );
}

#endif // !GAME_OBJECT_FRAMEWORK_HPP
