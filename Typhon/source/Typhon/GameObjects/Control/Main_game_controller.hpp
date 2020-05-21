#ifndef TYPHON_GAME_OBJECTS_CONTROL_MAIN_GAME_CONTROLLER_HPP
#define TYPHON_GAME_OBJECTS_CONTROL_MAIN_GAME_CONTROLLER_HPP

#include <Typhon/Framework.hpp>

class MainGameController : public NonstateObject, private NetworkingManager::EventListener {
public:
    MainGameController(QAO_RuntimeRef runtimeRef);

    ~MainGameController();

protected:
    void eventUpdate() override;
    void eventPostUpdate() override;
    void eventDrawGUI() override;

private:
    void onNetworkingEvent(const RN_Event& event) override;
};

#endif // !TYPHON_GAME_OBJECTS_CONTROL_MAIN_GAME_CONTROLLER_HPP

