
#include <Hobgoblin/RigelNet.hpp>

#include "Game_object_framework.hpp"
#include "Main_menu.hpp"
#include "Z_main_impl.hpp"

int MainImpl::run(int argc, char* argv[]) {
	// Setup:
	hg::RN_IndexHandlers();
	_gameContext = std::make_unique<GameContext>();
	QAO_PCreate<MainMenu>(_gameContext->qaoRuntime);

	// Run:
	int rv;
	GameContext::run(_gameContext.get(), &rv);

	// Teardown:
	_gameContext.reset();

	// Return run's return value:
	return rv;
}