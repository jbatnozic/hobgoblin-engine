#ifndef MAIN_GAME_CONTROLLER_HPP
#define MAIN_GAME_CONTROLLER_HPP

#include "GameObjects/Framework/Game_object_framework.hpp"
#include "GameObjects/Managers/Networking_manager.hpp"

class MainGameController : public GOF_NonstateObject, private NetworkingManager::EventListener {
public:
    MainGameController(QAO_RuntimeRef runtimeRef);

    ~MainGameController();

protected:
    void eventUpdate() override;

private:
    void onNetworkingEvent(const RN_Event& event) override;
};

#endif // !MAIN_GAME_CONTROLLER_HPP

