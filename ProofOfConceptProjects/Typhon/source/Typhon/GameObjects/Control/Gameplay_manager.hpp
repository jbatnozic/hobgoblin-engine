// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef TYPHON_GAME_OBJECTS_CONTROL_GAMEPLAY_MANAGER_HPP
#define TYPHON_GAME_OBJECTS_CONTROL_GAMEPLAY_MANAGER_HPP

#include <Typhon/Framework.hpp>

class GameplayManager : public NonstateObject, private NetworkingManager::EventListener {
public:
    GameplayManager(QAO_RuntimeRef runtimeRef);

    ~GameplayManager();

    // TODO bool isGamePaused() const noexcept;

    void restartGame();

protected:
    void _eventUpdate() override;
    void _eventEndUpdate() override;
    void _eventDrawGUI() override;

private:
    void onNetworkingEvent(const RN_Event& event) override;

    void _setPlayerPosition(hg::PZInteger playerIndex, float* x, float* y) const;
};

#endif // !TYPHON_GAME_OBJECTS_CONTROL_GAMEPLAY_MANAGER_HPP

// clang-format on
