
#include <Hobgoblin/RigelNet.hpp>

#include <Typhon/GameObjects/UI/Main_menu.hpp>
#include <Typhon/Graphics/Sprites.hpp>
#include <Typhon/Terrain/Terrain.hpp>

#include "Main_impl.hpp"

// TODO catch exceptions in main

int MainImpl::run(int argc, char* argv[]) {
	// Setup:
	ResolveExecutionPriorities();
	hg::RN_IndexHandlers();

	// To avoid the risk of initializing later from multiple threads
	// at the same time.
	Terrain::initializeSingleton();

	auto spriteLoader = LoadAllSprites();
	GameContext::ResourceConfig resConfig;
	resConfig.spriteLoader = &spriteLoader;
	GameContext::RuntimeConfig rtConfig;
	_gameContext = std::make_unique<GameContext>(resConfig, rtConfig);
	ExtendGameContext(*_gameContext);
	QAO_PCreate<MainMenu>(&_gameContext->getQaoRuntime());

	// Run:
	const int rv = _gameContext->run();

	// Teardown:
	_gameContext.reset();

	// Return run's return value:
	return rv;
}