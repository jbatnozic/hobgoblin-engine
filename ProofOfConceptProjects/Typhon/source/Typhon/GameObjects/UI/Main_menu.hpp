#ifndef TYPHON_GAME_OBJECTS_UI_MAIN_MENU_HPP
#define TYPHON_GAME_OBJECTS_UI_MAIN_MENU_HPP

#include <Typhon/Framework.hpp>

class MainMenu : public NonstateObject {
public:
    MainMenu(QAO_RuntimeRef runtimeRef);

    void eventUpdate() override;

private:
    static void generateTerrain(GameContext& ctx);
};

#endif // !TYPHON_GAME_OBJECTS_UI_MAIN_MENU_HPP
