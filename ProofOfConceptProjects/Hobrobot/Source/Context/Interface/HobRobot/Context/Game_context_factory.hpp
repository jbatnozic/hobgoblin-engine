#ifndef HOBROBOT_CONTEXT_GAME_CONTEXT_FACTORY_HPP
#define HOBROBOT_CONTEXT_GAME_CONTEXT_FACTORY_HPP

#include "Engine.hpp"

#include <Hobgoblin/Config.hpp>

namespace hobrobot {

using hg::HGConfig;

std::unique_ptr<spe::GameContext> CreateHostContext(const HGConfig& aConfig);

std::unique_ptr<spe::GameContext> CreateClientContext(const HGConfig& aConfig);

} // namespace hobrobot

#endif // !HOBROBOT_CONTEXT_GAME_CONTEXT_FACTORY_HPP
