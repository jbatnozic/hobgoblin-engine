#ifndef MAIN_GAME_CONTROLLER_HPP
#define MAIN_GAME_CONTROLLER_HPP

#include "Game_object_framework.hpp"
#include "Networking_manager.hpp"

class MainGameController : public GOF_NonstateObject, private NetworkingManager::EventListener {
public:
    MainGameController(QAO_RuntimeRef runtimeRef);

    ~MainGameController();

    void onNetworkingEvent(const RN_Event& event) override;

private:

};

#endif // !MAIN_GAME_CONTROLLER_HPP

