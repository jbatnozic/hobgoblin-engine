
#include <iostream>

#include "State_scheduler.hpp"

void PRINT(StateScheduler2<int>& obj) {
    for (auto& item : obj) {
        std::cout << item << ' ';
    }
    std::cout << std::endl;
}

int main() {
    StateScheduler2<int> ssch{5, 0, false, false};
    for (auto& item : ssch) {
        item = 0;
    }

    PRINT(ssch);

    ssch.putNewState(1, 0);
    ssch.putNewState(2, 0);
    ssch.putNewState(3, 0);
    ssch.scheduleNewStates();

    PRINT(ssch);

    ssch.advance();

    PRINT(ssch);


}