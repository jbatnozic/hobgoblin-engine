#ifndef TYPHON_MAIN_MAIN_IMPL_HPP
#define TYPHON_MAIN_MAIN_IMPL_HPP

#include <Typhon/Framework.hpp>

#include <memory>

class MainImpl {
public:
    int run(int argc, char* argv[]);

private:
    std::unique_ptr<GameContext> _gameContext;
};

#endif // !TYPHON_MAIN_MAIN_IMPL_HPP
