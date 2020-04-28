
#include <Hobgoblin/RigelNet.hpp>

#include "GameObjects/Framework/Game_object_framework.hpp"
#include "GameObjects/UI/Main_menu.hpp"
#include "Graphics/Sprites.hpp"
#include "Main/Main_impl.hpp"

// TODO catch exceptions in main

int MainImpl::run(int argc, char* argv[]) {
	// Setup:
	hg::RN_IndexHandlers();

	auto spriteLoader = LoadAllSprites();
	GameContext::ResourceConfig resConfig;
	resConfig.spriteLoader = &spriteLoader;
	_gameContext = std::make_unique<GameContext>(resConfig);
	QAO_PCreate<MainMenu>(_gameContext->qaoRuntime);

	// Run:
	const int rv = _gameContext->run();

	// Teardown:
	_gameContext.reset();

	// Return run's return value:
	return rv;
}