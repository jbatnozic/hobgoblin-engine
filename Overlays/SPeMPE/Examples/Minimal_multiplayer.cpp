
#include <Hobgoblin/Utility/Autopack.hpp>
#include <SPeMPE/SPeMPE.hpp>

#include <memory>

namespace hg  = ::jbatnozic::hobgoblin;
namespace spe = ::jbatnozic::spempe;

using namespace hg::qao; // All names from QAO are prefixed with QAO_
using namespace hg::rn;  // All names from RigelNet are prefixed with RN_

using MNetworking = spe::NetworkingManagerInterface;
// using MWindow     = spe::WindowManagerInterface; TODO

///////////////////////////////////////////////////////////////////////////
// PLAYER CONTROLS                                                       //
///////////////////////////////////////////////////////////////////////////

struct PlayerControls {
    bool left, right, up, down;
    HG_ENABLE_AUTOPACK(PlayerControls, left, right, up, down);
};

///////////////////////////////////////////////////////////////////////////
// MAIN GAMEPLAY CONTROLLER INTERFACE                                    //
///////////////////////////////////////////////////////////////////////////

class GameplayManagerInterface : public spe::ContextComponent {
public:
    virtual const PlayerControls& getPlayerControls(hg::PZInteger aForPlayerIndex) const = 0;

private:
    SPEMPE_CTXCOMP_TAG("GameplayManagerInterface");
};

using MGameplay = GameplayManagerInterface;

///////////////////////////////////////////////////////////////////////////
// PLAYER "AVATARS"                                                      //
///////////////////////////////////////////////////////////////////////////

struct PlayerAvatar_VisibleState {
    float x, y;
    int owningPlayerIndex;
    HG_ENABLE_AUTOPACK(PlayerAvatar_VisibleState, x, y, owningPlayerIndex);
};

class PlayerAvatar : public spe::SynchronizedObject<PlayerAvatar_VisibleState> {
public:
    PlayerAvatar(QAO_RuntimeRef aRuntimeRef,
                 spe::SynchronizedObjectRegistry& aSyncObjReg,
                 spe::SyncId aSyncId)
        : SyncObjSuper{aRuntimeRef, SPEMPE_TYPEID_SELF, 0 /* exepr */, "PlayerAvatar", aSyncObjReg, aSyncId}
    {
    }

    void eventUpdate(spe::IfMaster) override {
        auto& self     = _getCurrentState();
        if (self.owningPlayerIndex < 0) {
            return;
        }

        auto& controls = ccomp<MGameplay>().getPlayerControls(self.owningPlayerIndex);

        self.x += (5.f * ((float)controls.right - (float)controls.left));
        self.y += (5.f * ((float)controls.down  - (float)controls.up));
    }

    void eventDraw1() override {
        auto& self = _getCurrentState();
        // TODO
    }

private:
    void _syncCreateImpl(RN_NodeInterface& aNode,
                         std::vector<hg::PZInteger>& aRecepients) const override;

    void _syncUpdateImpl(RN_NodeInterface& aNode,
                         std::vector<hg::PZInteger>& aRecepients) const override;

    void _syncDestroyImpl(RN_NodeInterface& aNode,
                          std::vector<hg::PZInteger>& aRecepients) const override;
};

SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(PlayerAvatar, (CREATE, UPDATE, DESTROY));

void PlayerAvatar::_syncCreateImpl(RN_NodeInterface& aNode,
                                   std::vector<hg::PZInteger>& aRecepients) const {
    SPEMPE_SYNC_CREATE_DEFAULT_IMPL(PlayerAvatar, aNode, aRecepients);
}

void PlayerAvatar::_syncUpdateImpl(RN_NodeInterface& aNode,
                                   std::vector<hg::PZInteger>& aRecepients) const {
    SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(PlayerAvatar, aNode, aRecepients);
}

void PlayerAvatar::_syncDestroyImpl(RN_NodeInterface& aNode,
                                    std::vector<hg::PZInteger>& aRecepients) const {
    SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(PlayerAvatar, aNode, aRecepients);
}

///////////////////////////////////////////////////////////////////////////
// MAIN GAMEPLAY CONTROLLER IMPLEMENTATION                               //
///////////////////////////////////////////////////////////////////////////

class GameplayManager : public GameplayManagerInterface, spe::NonstateObject {
public:
    GameplayManager(QAO_RuntimeRef aRuntimeRef)
        : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, 0 /*exepr*/, "GameplayManager"}
    {
    }

    const PlayerControls& getPlayerControls(hg::PZInteger aForPlayerIndex) const override {
        return _pc;
    }

private:
    PlayerControls _pc;
};

///////////////////////////////////////////////////////////////////////////
// GAME CONFIG                                                           //
///////////////////////////////////////////////////////////////////////////

std::unique_ptr<spe::GameContext> MakeGameContext() {
    auto context = std::make_unique<spe::GameContext>(spe::GameContext::RuntimeConfig{});

    auto gpMgr = std::make_unique<GameplayManager>(context->getQAORuntime().nonOwning());
    context->attachAndOwnComponent(std::move(gpMgr));

    return context;
}

int main(int argc, char* argv[]) {
    RN_IndexHandlers();

    // Parse args (mode, IP, port etc)

    // Start the game:
    auto context = MakeGameContext();
    const int status = context->runFor(-1);
    return status;
}