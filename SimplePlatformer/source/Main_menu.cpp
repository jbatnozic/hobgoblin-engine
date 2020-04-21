
#include <string>

#include "Game_context.hpp"
#include "Main_menu.hpp"

namespace {

template <class T>
T InputPrompt(const std::string& name, T defaultValue) {
	std::cout << "Input " << name << " (press Enter for default - " << defaultValue << "): ";
	std::string temp;
	std::getline(std::cin, temp);
	if (temp.empty()) {
		return defaultValue;
	}
	else {
		std::stringstream ss;
		ss << temp;
		T t;
		ss >> t;
		return t;
	}
}

} // namespace

MainMenu::MainMenu(QAO_RuntimeRef runtimeRef)
	: GOF_NonstateObject{runtimeRef, TYPEID_SELF, 0, "MainMenu"}
{
}

void MainMenu::eventUpdate() {
	int mode;

	mode = InputPrompt<int>("mode - 1 = host; 2 = client, 3 = solo, 4 = GM", 2);
	if (mode == 1) {
		std::uint16_t serverPort;
		
		// Start a local server in the background:
		auto serverCtx = std::make_unique<GameContext>(GameContext::Mode::Server);
		serverCtx->networkingServerSize = InputPrompt<hg::PZInteger>("player count", 2);
		serverCtx->networkingLocalPort  = InputPrompt<std::uint16_t>("local port - 0 for any", 8888);
		serverCtx->fullStart();

		serverPort = serverCtx->netMgr.getServer().getLocalPort();

		ctx().runChildContext(std::move(serverCtx));

		// Connext to the server:
		ctx().networkingLocalPort = 0;
		ctx().networkingServerIp = "localhost";
		ctx().networkingServerPort = serverPort;
		ctx().changeMode(GameContext::Mode::Client);
		ctx().fullStart();
	}
	else if (mode == 2) {
		ctx().networkingLocalPort = InputPrompt<std::uint16_t>("local port", 0);
		ctx().networkingServerIp = InputPrompt<std::string>("server IP", "127.0.0.1");
		ctx().networkingServerPort = InputPrompt<std::uint16_t>("server port", 8888);
		ctx().changeMode(GameContext::Mode::Client);
		ctx().fullStart();
	}
	else if (mode == 3) {
		ctx().changeMode(GameContext::Mode::Solo);
		ctx().fullStart();
	}
	else if (mode == 4) {
		ctx().networkingServerSize = InputPrompt<hg::PZInteger>("player count", 2);
		ctx().networkingLocalPort = InputPrompt<std::uint16_t>("local port - 0 for any", 8888);
		ctx().changeMode(GameContext::Mode::GameMaster);
		ctx().fullStart();
	}

	QAO_PDestroy(this);
}