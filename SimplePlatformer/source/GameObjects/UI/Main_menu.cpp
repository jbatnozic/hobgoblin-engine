
#include <string>

#include "GameContext/Game_context.hpp"
#include "GameObjects/UI/Main_menu.hpp"

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
#define HOST 1
#define CLIENT 2
#define SOLO 3
#define GAME_MASTER 4

	int mode = InputPrompt<int>("mode - 1 = host; 2 = client, 3 = solo, 4 = GM", 2);
	if (mode == HOST) {
		// Start a local server in the background:
		auto serverCtx = std::make_unique<GameContext>();
		serverCtx->networkConfig.clientCount = InputPrompt<hg::PZInteger>("client count", 1);
		serverCtx->networkConfig.localPort = InputPrompt<std::uint16_t>("local port - 0 for any", 8888);
		serverCtx->configure(GameContext::Mode::Server);

		const std::uint16_t serverPort = serverCtx->netMgr.getServer().getLocalPort();

		ctx().runChildContext(std::move(serverCtx));

		// Connext to the server:
		ctx().networkConfig.localPort = 0; // 0 = any
		ctx().networkConfig.serverIp = "localhost";
		ctx().networkConfig.serverPort = serverPort;
		ctx().configure(GameContext::Mode::Client);
	}
	else if (mode == CLIENT) {
		ctx().networkConfig.localPort = InputPrompt<std::uint16_t>("local port", 0);
		ctx().networkConfig.serverIp = InputPrompt<std::string>("server IP", "127.0.0.1");
		ctx().networkConfig.serverPort = InputPrompt<std::uint16_t>("server port", 8888);
		ctx().configure(GameContext::Mode::Client);
	}
	else if (mode == SOLO) {
		ctx().configure(GameContext::Mode::Solo);
	}
	else if (mode == GAME_MASTER) {
		ctx().networkConfig.clientCount = InputPrompt<hg::PZInteger>("client count", 1);
		ctx().networkConfig.localPort = InputPrompt<std::uint16_t>("local port - 0 for any", 8888);
		ctx().configure(GameContext::Mode::GameMaster);
	}

	QAO_PDestroy(this);

#undef HOST 
#undef CLIENT 
#undef SOLO 
#undef GAME_MASTER 
}