#ifndef STATE_SCHEDULER_HPP
#define STATE_SCHEDULER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Math.hpp>

#include <algorithm>
#include <deque>
#include <vector>

//struct TState {
//    void integrate(const TState& other);
//    void unstuck(const std::deque<TState>& history);
//    void debounce(const std::deque<TState>& q, std::size_t currentPos);
//};

template <class TState>
class StateScheduler {
public:
    StateScheduler(hg::PZInteger bufferLength, hg::PZInteger historyLength, bool doDebounce = true, bool doUnstuck = true);

    void reset(hg::PZInteger bufferLength, hg::PZInteger historyLength, bool doDebounce = true, bool doUnstuck = true);

    void putNewState(const TState& newState, hg::PZInteger delay = 0);
    
    void scheduleNewStates();

    void advance();

    TState& getCurrentState();
    TState& getLatestState();

    const TState& getCurrentState() const;
    const TState& getLatestState() const;

    typename std::deque<TState>::iterator begin();
    typename std::deque<TState>::iterator end();

    typename std::deque<TState>::const_iterator cbegin() const;
    typename std::deque<TState>::const_iterator cend() const;

private:
    std::deque<TState> _stateBuffer;
    std::deque<TState> _rawStateHistory;
    std::vector<TState> _newStates;
    hg::PZInteger _historyLength;
    hg::PZInteger _newStatesDelay;
    bool _doDebounce, _doUnstuck;
};

template <class TState>
StateScheduler<TState>::StateScheduler(hg::PZInteger bufferLength, hg::PZInteger historyLength, 
                                       bool doDebounce, bool doUnstuck) {
    reset(bufferLength, historyLength, doDebounce, doUnstuck);
}

template <class TState>
void StateScheduler<TState>::reset(hg::PZInteger bufferLength, hg::PZInteger historyLength, 
                                   bool doDebounce, bool doUnstuck) {
    _stateBuffer.clear();
    _stateBuffer.resize(hg::ToSz(bufferLength + 1 + historyLength));

    _historyLength = historyLength; // Non-raw (!) history length

    _rawStateHistory.clear();
    _rawStateHistory.resize(hg::ToSz(bufferLength + 1));

    _doDebounce = doDebounce;
    _doUnstuck = doUnstuck;
}

template <class TState>
void StateScheduler<TState>::putNewState(const TState& newState, hg::PZInteger delay) {
    _newStates.push_back(newState);
    _newStatesDelay = delay;

    _rawStateHistory.pop_front();
    _rawStateHistory.push_back(newState);
}

template <class TState>
void StateScheduler<TState>::scheduleNewStates() {
    // Integrate new states:
    int place = (static_cast<int>(_stateBuffer.size()) - 1) - _newStatesDelay;

    for (auto iter = _newStates.rbegin(); iter != _newStates.rend(); iter = std::next(iter)) {
        place = std::max(static_cast<int>(_historyLength), place);
        if (place == static_cast<int>(_stateBuffer.size()) - 1) {
            _stateBuffer[hg::ToSz(place)] = *iter;
        }
        else {
            _stateBuffer[hg::ToSz(place)].integrate(*iter);
        }
        place -= 1;
    }

    _newStates.clear();

    // Unstuck:
    if (_doUnstuck) {
        for (auto& state : _stateBuffer) {
            state.unstuck(_rawStateHistory);
        }
    }
}

template <class TState>
void StateScheduler<TState>::advance() {
    if (!_rawStateHistory.empty()) {
        _stateBuffer.push_back(_rawStateHistory.back());
    }
    else {
        _stateBuffer.push_back(_stateBuffer.back());
    }
    _stateBuffer.pop_front();

    // Debounce:
    if (_doDebounce) {
        _stateBuffer[_historyLength].debounce(_stateBuffer, hg::ToSz(_historyLength));
    }
}

template <class TState>
TState& StateScheduler<TState>::getCurrentState() {
    return _stateBuffer[hg::ToSz(_historyLength)];
}

template <class TState>
TState& StateScheduler<TState>::getLatestState() {
    return _stateBuffer.back();
}


template <class TState>
const TState& StateScheduler<TState>::getCurrentState() const {
    return _stateBuffer[hg::ToSz(_historyLength)];
}

template <class TState>
const TState& StateScheduler<TState>::getLatestState() const {
    return _stateBuffer.back();
}

template <class TState>
typename std::deque<TState>::iterator StateScheduler<TState>::begin() {
    return _stateBuffer.begin() + _historyLength;
}

template <class TState>
typename std::deque<TState>::iterator StateScheduler<TState>::end() {
    return _stateBuffer.end();
}

template <class TState>
typename std::deque<TState>::const_iterator StateScheduler<TState>::cbegin() const {
    return _stateBuffer.cbegin() + _historyLength;
}

template <class TState>
typename std::deque<TState>::const_iterator StateScheduler<TState>::cend() const {
    return _stateBuffer.cend();
}

#endif // !STATE_SCHEDULER_HPP

