#ifndef STATE_SCHEDULER_HPP
#define STATE_SCHEDULER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Math.hpp>

#include <algorithm>
#include <deque>
#include <vector>

#include <cassert>

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

    hg::PZInteger getBufferLength() const noexcept;
    hg::PZInteger getHistoryLength() const noexcept;

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

    _newStates.clear();

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

template <class TState>
hg::PZInteger StateScheduler<TState>::getBufferLength() const noexcept {
    return (hg::ToPz(_stateBuffer.size()) - _historyLength);
}

template <class TState>
hg::PZInteger StateScheduler<TState>::getHistoryLength() const noexcept {
    return _historyLength;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct TState1 {
};

template <class TState>
class StateScheduler2 {
public:
    StateScheduler2(hg::PZInteger minBufferLength, hg::PZInteger, bool x = true, bool y = true);

    void reset(hg::PZInteger minBufferLength, hg::PZInteger, bool x = true, bool y = true);

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

    //hg::PZInteger getBufferLength() const noexcept;
    //hg::PZInteger getHistoryLength() const noexcept;

    void setDiscardOld(bool b) {
        _discardTooOld = b;
    }

    void resetIfLargerThan(hg::PZInteger s) {
        //if (hg::ToPz(_stateBuffer.size()) > s) {
        //    // TODO Don't fully reset, but keep newest states
        //    reset(_minBufferLength, 0, false, false);
        //}

        assert(s > _minBufferLength);
        while (hg::ToPz(_stateBuffer.size()) > s) {
            // TODO Don't fully reset, but keep newest states
            advance();
        }
    }

private:
    std::deque<TState> _stateBuffer;
    std::vector<TState> _newStates;
    TState _latestBlueState;
    hg::PZInteger _minBufferLength;
    hg::PZInteger _newStatesDelay;
    int _blueHead = -1;
    int _blueTail = -1;
    bool _discardTooOld = false;

    void setAt(const TState& state, hg::PZInteger pos) {
        if (hg::ToPz(_stateBuffer.size()) <= pos) {
            _stateBuffer.resize(hg::ToSz(pos + 1));
        }

        _stateBuffer[hg::ToSz(pos)] = state;
    }
};

template <class TState>
StateScheduler2<TState>::StateScheduler2(hg::PZInteger minBufferLength, hg::PZInteger, bool, bool) {
    reset(minBufferLength, 0, false, false);
}

template <class TState>
void StateScheduler2<TState>::reset(hg::PZInteger minBufferLength, hg::PZInteger, bool, bool) {
    _stateBuffer.clear();
    _stateBuffer.resize(hg::ToSz(minBufferLength));

    _minBufferLength = minBufferLength;

    _newStates.clear();

    _blueHead = -1;
    _blueTail = -1;
}

template <class TState>
void StateScheduler2<TState>::putNewState(const TState& newState, hg::PZInteger delay) {
    if (_blueHead != -1 && _blueTail != -1) {
        setAt(newState, _blueTail + 1);
        _blueTail += 1;
    }
    else {
        _newStates.push_back(newState);
        _newStatesDelay = delay;
    }

    _latestBlueState = newState;
}

template <class TState>
void StateScheduler2<TState>::scheduleNewStates() {
    if (_newStates.empty()) {
        return;
    }

    int startAt = _minBufferLength - _newStatesDelay - hg::ToPz(_newStates.size());
    const bool discardTooOld = _discardTooOld;

    if (startAt >= 0) {
        int i = 0;
        for (auto& state : _newStates) {
            setAt(state, startAt + i);
            i += 1;
        }
        _blueHead = startAt;
        _blueTail = startAt + i - 1;
    }
    else if (discardTooOld) {
        hg::PZInteger stateSelector = -startAt;
        startAt = 0;
        int i = 0;

        while (true) {
            if (stateSelector >= hg::ToPz(_newStates.size())) {
                break;
            }
            setAt(_newStates[hg::ToSz(stateSelector)], startAt + i);
            stateSelector += 1;
            i += 1;
        }

        _blueHead = startAt;
        _blueTail = startAt + i - 1;
    }
    else {
        startAt = 0;
        int i = 0;
        for (auto& state : _newStates) {
            setAt(state, startAt + i);
            i += 1;
        }
        _blueHead = startAt;
        _blueTail = startAt + i - 1;
    }

    _newStates.clear();

    // Unstuck:
    //if (_doUnstuck) {
    //    for (auto& state : _stateBuffer) {
    //        state.unstuck(_rawStateHistory);
    //    }
    //}
}

template <class TState>
void StateScheduler2<TState>::advance() {
    if (hg::ToPz(_stateBuffer.size()) <= _minBufferLength) {
        _stateBuffer.push_back(_latestBlueState);
    }

    if (_blueHead >= 0) _blueHead -= 1;
    if (_blueTail >= 0) _blueTail -= 1;

    if (_blueHead == -1 && _blueTail != -1) {
        _blueHead = 0;
    }

    _stateBuffer.pop_front();

    // Debounce:
    /*if (_doDebounce) {
        _stateBuffer[_historyLength].debounce(_stateBuffer, hg::ToSz(_historyLength));
    }*/
}

template <class TState>
TState& StateScheduler2<TState>::getCurrentState() {
    return _stateBuffer[0];
}

template <class TState>
TState& StateScheduler2<TState>::getLatestState() {
    return _latestBlueState;
}


template <class TState>
const TState& StateScheduler2<TState>::getCurrentState() const {
    return _stateBuffer[0];
}

template <class TState>
const TState& StateScheduler2<TState>::getLatestState() const {
    return _latestBlueState;
}

template <class TState>
typename std::deque<TState>::iterator StateScheduler2<TState>::begin() {
    return _stateBuffer.begin();
}

template <class TState>
typename std::deque<TState>::iterator StateScheduler2<TState>::end() {
    return _stateBuffer.end();
}

template <class TState>
typename std::deque<TState>::const_iterator StateScheduler2<TState>::cbegin() const {
    return _stateBuffer.cbegin();
}

template <class TState>
typename std::deque<TState>::const_iterator StateScheduler2<TState>::cend() const {
    return _stateBuffer.cend();
}

//template <class TState>
//hg::PZInteger StateScheduler2<TState>::getBufferLength() const noexcept {
//    return (hg::ToPz(_stateBuffer.size()) - _historyLength);
//}
//
//template <class TState>
//hg::PZInteger StateScheduler2<TState>::getHistoryLength() const noexcept {
//    return _historyLength;
//}

#endif // !STATE_SCHEDULER_HPP

