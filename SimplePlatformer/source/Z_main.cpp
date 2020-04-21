
#include <Hobgoblin/Utility/stopwatch.hpp>
#include <SFML/System.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "Global_program_state.hpp"
#include "Game_object_framework.hpp"

std::unique_ptr<GlobalProgramState> ProgramSetup();
void ProgramTeardown(std::unique_ptr<GlobalProgramState> globalState);
int  MainProgramLoop(GlobalProgramState& globalState);

int main() {
	std::unique_ptr<GlobalProgramState> globalState = ProgramSetup();

    hg::util::Stopwatch sw;
	int rv = MainProgramLoop(*globalState);
    std::cout << "Program ran for " << sw.getElapsedTime<std::chrono::milliseconds>().count() << "ms.\n";

	ProgramTeardown(std::move(globalState));

	return rv;
}

///////////////////////////////////////////////////////////////////////////////

template <class T>
T InputPrompt(const std::string& name, T defaultValue) {
	std::cout << "Input " << name << " (press Enter for deault - " << defaultValue << "): ";
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

std::unique_ptr<GlobalProgramState> ProgramSetup() {
	RN_IndexHandlers();

	bool isHost;
	hg::PZInteger size;
	std::uint16_t localPort;
	std::uint16_t serverPort;
	std::string serverIp;

	isHost = InputPrompt<int>("mode - 1 = host; 2 = client", 2) == 1;
	if (isHost) {
		size = InputPrompt<hg::PZInteger>("player count", 2);
		localPort = InputPrompt<std::uint16_t>("local port - 0 for any", 8888);
	}
	else {
		localPort = InputPrompt<std::uint16_t>("local port", 0);
		serverIp = InputPrompt<std::string>("server IP", "127.0.0.1");
		serverPort = InputPrompt<std::uint16_t>("server port", 8888);
	}

	auto globalState = std::unique_ptr<GlobalProgramState>{
		// Explicit NEW call is safe here, and it's used to enable autocomplete
		// for constructor arguments (using std::make_unique prevents that).
		new GlobalProgramState(isHost)
	};

	globalState->netMgr.getNode().visit(
		[=](NetworkingManager::ServerType& server) {
			server.start(localPort, "beetlejuice");
			server.setTimeoutLimit(std::chrono::seconds{2});
			std::cout << "Server started on port " << server.getLocalPort() << "\n";
		},
		[=](NetworkingManager::ClientType& client) {
			client.connect(localPort, serverIp, serverPort, "beetlejuice");
			client.setTimeoutLimit(std::chrono::seconds{2});
		}
	);

	return globalState;
}

using Duration = std::chrono::duration<double, std::micro>;
using std::chrono::milliseconds;
using std::chrono::steady_clock;
using std::chrono::duration_cast;

int SingleQAOIteration(GlobalProgramState& globalState, std::int32_t eventFlags);

int MainProgramLoop(GlobalProgramState& globalState) {
	const Duration deltaTime = Duration{1'000'000} / 60;
	Duration accumulatorTime = deltaTime;
	auto currentTime = steady_clock::now();

    int itercnt = 0;
	int cnt = 0;
	while (!globalState.quit) {
		//std::cout << "==================\n";
		auto now = steady_clock::now();
		accumulatorTime += duration_cast<Duration>(now - currentTime);
		currentTime = now;

		for (int i = 0; i < /*FACTOR*/ 2; i += 1) { // TODO
			if (accumulatorTime < deltaTime) {
				break;
			}

            itercnt += 1;

			if (i == 1) {
				//std::cout << "Double update " << cnt << '\n';
				cnt += 1;
			}

			// All steps except render:
			hg::util::Stopwatch stopwatch{};
			int rv = SingleQAOIteration(globalState, ~(1 << static_cast<std::int32_t>(QAO_Event::Render)));
			if (rv != 0) {
				return rv;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::T)) {
				auto time = stopwatch.getElapsedTime<std::chrono::microseconds>().count();
				std::cout << "Time for step: " << time << "us\n";
			}

			accumulatorTime -= deltaTime;
		} // End for

		if (accumulatorTime >= deltaTime / 2) {
			accumulatorTime = deltaTime / 2;
		}

		// Render step:
		//std::cout << "Render; " << accumulatorTime.count() << "us in accumulator\n";
		int rv = SingleQAOIteration(globalState, (1 << static_cast<std::int32_t>(QAO_Event::Render)));
		if (rv != 0) {
			return rv;
		}
	} // End while

    std::cout << "double updates = " << cnt << '\n';
    std::cout << "itercnt = " << itercnt << '\n';

	return EXIT_SUCCESS;
}

#define CATCH_EXCEPTIONS
int SingleQAOIteration(GlobalProgramState& globalState, std::int32_t eventFlags) {
	QAO_Runtime& runtime = globalState.qaoRuntime;

	runtime.startStep();
	bool done = false;
	do {
#ifdef CATCH_EXCEPTIONS
		try {
			runtime.advanceStep(done, eventFlags);
		}
		catch (std::exception& ex) {
			std::cout << "Exception caught: " << ex.what() << '\n';
			return 1;
		}
		catch (...) {
			std::cout << "Unknown exception caught!\n";
			return 2;
		}
#else
		runtime.advanceStep(done, eventFlags);
#endif
	} while (!done);

	return 0;
}

void ProgramTeardown(std::unique_ptr<GlobalProgramState> globalState) {
	globalState.reset();
}