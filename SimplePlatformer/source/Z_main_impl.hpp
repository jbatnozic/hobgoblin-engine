#ifndef MAIN_IMPL_HPP
#define MAIN_IMPL_HPP

#include <memory>

#include "Game_context.hpp"

class MainImpl {
public:
    int run(int argc, char* argv[]);

private:
    std::unique_ptr<GameContext> _gameContext;
};

#endif // !MAIN_IMPL_HPP
