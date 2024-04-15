// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef TYPHON_GAME_OBJECTS_UI_MAIN_MENU_HPP
#define TYPHON_GAME_OBJECTS_UI_MAIN_MENU_HPP

#include <Typhon/Framework.hpp>

class MainMenu : public NonstateObject {
public:
    MainMenu(QAO_RuntimeRef runtimeRef);

    void _eventUpdate() override;

private:
    static void generateTerrain(GameContext& ctx);
};

#endif // !TYPHON_GAME_OBJECTS_UI_MAIN_MENU_HPP

// clang-format on
