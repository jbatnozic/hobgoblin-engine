
#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "Global_program_state.hpp"
#include "Object_framework.hpp"

std::unique_ptr<GlobalProgramState> ProgramSetup();
void ProgramTeardown(std::unique_ptr<GlobalProgramState> globalState);
int  MainProgramLoop(GlobalProgramState& globalState);

int main() {
	std::unique_ptr<GlobalProgramState> globalState = ProgramSetup();

	int rv = MainProgramLoop(*globalState);

	ProgramTeardown(std::move(globalState));

	return rv;
}

///////////////////////////////////////////////////////////////////////////////

std::unique_ptr<GlobalProgramState> ProgramSetup() {
	auto globalState = std::unique_ptr<GlobalProgramState>{
		// Explicit NEW call is safe here, and it's used to enable autocomplete
		// for constructor arguments (using std::make_unique prevents that).
		new GlobalProgramState()
	};
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

			if (i == 1) {
				std::cout << "Double update " << cnt << '\n';
				cnt += 1;
			}

			// All steps except render:
			//std::cout << "Update\n";
			int rv = SingleQAOIteration(globalState, ~(1 << static_cast<std::int32_t>(QAO_Event::Render)));
			if (rv != 0) {
				return rv;
			}
			accumulatorTime -= deltaTime;
		} // End for

		// Render step:
		//std::cout << "Render; " << accumulatorTime.count() << "us in accumulator\n";
		int rv = SingleQAOIteration(globalState, (1 << static_cast<std::int32_t>(QAO_Event::Render)));
		if (rv != 0) {
			return rv;
		}
	} // End while

	return EXIT_SUCCESS;
}

int SingleQAOIteration(GlobalProgramState& globalState, std::int32_t eventFlags) {
	QAO_Runtime& runtime = globalState.qaoRuntime;

	runtime.startStep();
	bool done = false;
	do {
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
	} while (!done);

	return 0;
}

void ProgramTeardown(std::unique_ptr<GlobalProgramState> globalState) {
	globalState.reset();
}