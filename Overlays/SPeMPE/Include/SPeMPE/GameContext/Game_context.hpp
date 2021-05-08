#ifndef SPEMPE_GAME_CONTEXT_GAME_CONTEXT_HPP
#define SPEMPE_GAME_CONTEXT_GAME_CONTEXT_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/QAO.hpp>

#include <SPeMPE/GameContext/Context_components.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>

#include <memory>
#include <string>
#include <thread>

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

class GameContext {
public:
    ///////////////////////////////////////////////////////////////////////////
    // CONFIGURATION                                                         //
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // GAME OBJECT MANAGEMENT                                                //
    ///////////////////////////////////////////////////////////////////////////

    hg::QAO_Runtime& getQAORuntime();

    ///////////////////////////////////////////////////////////////////////////
    // COMPONENT MANAGEMENT                                                  //
    ///////////////////////////////////////////////////////////////////////////

    //! Throws hg::TracedLogicError in case of tagHash clash.
    template <class taComponent>
    void attachComponent(taComponent& aComponent);

    //! No-op if component isn't present.
    void detachComponent(ContextComponent& aComponent);

    //! Throws hg::TracedLogicError if the component isn't present.
    template <class taComponent>
    taComponent& getComponent() const;

    std::string getComponentTableString(char aSeparator = '\n') const;

    ///////////////////////////////////////////////////////////////////////////
    // OWN STATE                                                             //
    ///////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////
    // CHILD CONTEXT SUPPORT                                                 //
    ///////////////////////////////////////////////////////////////////////////

    void runChildContext(std::unique_ptr<GameContext> childContext);

    bool hasChildContext();

    GameContext* getChildContext() const;

    int stopChildContext();
    
private:
    // Configuration:
        // TODO

    // Game object management:
    hg::QAO_Runtime _runtime;
    SynchronizedObjectRegistry _syncObjReg;

    // Context components:
    detail::ComponentTable _components;

    // Own state:
        // TODO

    // Child context support:
    GameContext* _parentContext = nullptr;
    std::unique_ptr<GameContext> _childContext = nullptr;
    std::thread _childContextThread;
    int _childContextReturnValue = 0;
};

template <class taComponent>
void GameContext::attachComponent(taComponent& aComponent) {
    _components.attachComponent(aComponent);
}

//! Throws hg::TracedLogicError if the component isn't present.
template <class taComponent>
taComponent& GameContext::getComponent() const {
    _components.getComponent<taComponent>();
}

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_CONTEXT_GAME_CONTEXT_HPP