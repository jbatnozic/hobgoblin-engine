#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "Game_object_framework.hpp"

class MainMenu : public GOF_NonstateObject {
public:
    MainMenu(QAO_RuntimeRef runtimeRef);

    void eventUpdate() override;

private:
};

#endif // !MAIN_MENU_HPP
