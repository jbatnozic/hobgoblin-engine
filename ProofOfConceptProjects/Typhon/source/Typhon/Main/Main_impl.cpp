
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/Utility/Exceptions.hpp>

#include <Typhon/Framework.hpp>
#include <Typhon/GameObjects/UI/Main_menu.hpp>
#include <Typhon/Graphics/Sprites.hpp>
#include <Typhon/Terrain/Terrain.hpp>

#include <cstdlib> // TODO Temp. for srand
#include <stdexcept>
#include <iostream>

#include "Main_impl.hpp"

#define MAIN_SHOULD_CATCH_EXCEPTIONS

int MainImpl::run(int argc, char* argv[]) 
#ifdef MAIN_SHOULD_CATCH_EXCEPTIONS
try
#endif // MAIN_SHOULD_CATCH_EXCEPTIONS
{
	std::cout << GameConfig::GAME_NAME << ' ' << GameConfig::GAME_VERSION << '\n';

	// Setup:
	ResolveExecutionPriorities();
	hg::RN_IndexHandlers();
	std::srand(static_cast<unsigned>(std::time(NULL)));

	// To avoid the risk of initializing later from multiple threads
	// at the same time.
	Terrain::initializeSingleton();

	auto spriteLoader = LoadAllSprites();
	GameContext::ResourceConfig resConfig;
	resConfig.spriteLoader = &spriteLoader;
	GameContext::RuntimeConfig rtConfig{GameConfig::TARGET_FRAMERATE};
	_gameContext = std::make_unique<GameContext>(resConfig, rtConfig);
	ExtendGameContext(*_gameContext);
	QAO_PCreate<MainMenu>(&_gameContext->getQaoRuntime());

	// Run:
	const int retVal = _gameContext->run();

	// Teardown:
	_gameContext.reset();

	// Return run's return value:
	return retVal;
}
#ifdef MAIN_SHOULD_CATCH_EXCEPTIONS
catch (hg::util::TracedException& ex) {
	std::cerr << "Unrecoverable traced exception caught in main: " << ex.what();
	ex.printStackTrace(std::cerr);
	return EXIT_FAILURE;
}
catch (std::exception& ex) {
	std::cerr << "Unrecoverable exception caught in main: " << ex.what();
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Unrecoverable exception caught in main: <unknown exception type>";
	return EXIT_FAILURE;
}
#endif // MAIN_SHOULD_CATCH_EXCEPTIONS