#ifndef TYPHON_GAME_OBJECTS_CONTROL_GAMEPLAY_MANAGER_HPP
#define TYPHON_GAME_OBJECTS_CONTROL_GAMEPLAY_MANAGER_HPP

#include <Typhon/Framework.hpp>

class GameplayManager : public NonstateObject, private NetworkingManager::EventListener {
public:
    GameplayManager(QAO_RuntimeRef runtimeRef);

    ~GameplayManager();

    // TODO bool isGamePaused() const noexcept;

protected:
    void eventUpdate() override;
    void eventPostUpdate() override;
    void eventDrawGUI() override;

private:
    void onNetworkingEvent(const RN_Event& event) override;
};

#endif // !TYPHON_GAME_OBJECTS_CONTROL_GAMEPLAY_MANAGER_HPP

