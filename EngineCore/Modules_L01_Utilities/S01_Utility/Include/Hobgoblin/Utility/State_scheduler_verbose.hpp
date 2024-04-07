#ifndef UHOBGOBLIN_UTIL_STATE_SCHEDULER_VERBOSE_HPP
#define UHOBGOBLIN_UTIL_STATE_SCHEDULER_VERBOSE_HPP

#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Utility/State_scheduler_simple.hpp>

#include <sstream>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! This class does the same as `SimpleStateScheduler`, but it will log its entire state
//! before and after any mutating operation. Thus its use is recommended only for debugging
//! purposes, as it will flood the output with huge amounts of text.
//! It's required that `taState` supports operator<< to std::ostream.
template <class taState>
class VerboseStateScheduler {
public:
    VerboseStateScheduler(PZInteger aDefaultDelay);

    // Main:

    void reset(PZInteger aDefaultDelay);

    void putNewState(const taState& aNewState, PZInteger aDelay = 0);

    void scheduleNewStates();

    void advance();

    // Extra/utility:

    PZInteger getDefaultDelay() const noexcept;

    void setDefaultDelay(PZInteger aNewDefaultDelay);

    bool isChainingBlueStatesAllowed() const;

    void setChainingBlueStatesAllowed(bool aChainingBlueStatesAllowed);

    void alignToDelay(PZInteger aDelay);

    // Access stored states:

    taState& getCurrentState();
    taState& getFollowingState();
    taState& getLatestState();

    bool isCurrentStateFresh() const;

    const taState& getCurrentState() const;
    const taState& getLatestState() const;

    typename std::vector<taState>::iterator begin();
    typename std::vector<taState>::iterator end();

    typename std::vector<taState>::const_iterator cbegin() const;
    typename std::vector<taState>::const_iterator cend() const;

private:
    static constexpr auto LOG_ID = "VerboseStateScheduler";
    SimpleStateScheduler<taState> _ssch;

    std::string _printState() const {
        std::ostringstream oss;
        for (PZInteger i = 0; i < _ssch._individualBufferSize(); i += 1) {
            if (i <= _ssch._bluePos) {
                oss << "BLUE(";
            }
            else {
                oss << "RED(";
            }
            oss << _ssch._scheduledStateAt(i) << ')';
            if (i < _ssch._individualBufferSize() - 1) {
                oss << " | ";
            }
        }
        return oss.str();
    }
};

template <class taState>
VerboseStateScheduler<taState>::VerboseStateScheduler(PZInteger aDefaultDelay)
    : _ssch{aDefaultDelay}
{
}

// Main:

//! Helper macro to print the state of a VerboseStateScheduler, do some action,
//! and then print the state again.
#define VSC_DO(...) \
    do { \
        HG_LOG_DEBUG(LOG_ID, "{} entering; state is: {}", CURRENT_FUNCTION, _printState()); \
        __VA_ARGS__ \
        HG_LOG_DEBUG(LOG_ID, "{} exiting; state is: {}", CURRENT_FUNCTION, _printState()); \
    } while (false)

template <class taState>
void VerboseStateScheduler<taState>::reset(PZInteger aDefaultDelay) {
    VSC_DO({ _ssch.reset(aDefaultDelay); });
}

template <class taState>
void VerboseStateScheduler<taState>::putNewState(const taState& aNewState, PZInteger aDelay) {
    VSC_DO({
        std::ostringstream oss;
        oss << aNewState;
        HG_LOG_DEBUG(LOG_ID, "New state: {} Delay: {}", oss.str(), aDelay);
        _ssch.putNewState(aNewState, aDelay);
    });
}

template <class taState>
void VerboseStateScheduler<taState>::scheduleNewStates() {
    VSC_DO({ _ssch.scheduleNewStates(); });
}

template <class taState>
void VerboseStateScheduler<taState>::advance() {
    VSC_DO({ _ssch.advance(); });
}

// Extra/utility:

template <class taState>
PZInteger VerboseStateScheduler<taState>::getDefaultDelay() const noexcept {
    return _ssch.getDefaultDelay();
}

template <class taState>
void VerboseStateScheduler<taState>::setDefaultDelay(PZInteger aNewDefaultDelay) {
    VSC_DO({ _ssch.setDefaultDelay(aNewDefaultDelay); });
}

template <class taState>
bool VerboseStateScheduler<taState>::isChainingBlueStatesAllowed() const {
    return _ssch.isChainingBlueStatesAllowed();
}

template <class taState>
void VerboseStateScheduler<taState>::setChainingBlueStatesAllowed(bool aChainingBlueStatesAllowed) {
    _ssch.setChainingBlueStatesAllowed(aChainingBlueStatesAllowed);
}

template <class taState>
void VerboseStateScheduler<taState>::alignToDelay(PZInteger aDelay) {
    VSC_DO({ _ssch.alignToDelay(aDelay); });
}

// Access states:

template <class taState>
taState& VerboseStateScheduler<taState>::getCurrentState() {
    return _ssch.getCurrentState();
}

template <class taState>
taState& VerboseStateScheduler<taState>::getFollowingState() {
    return _ssch.getFollowingState();
}

template <class taState>
taState& VerboseStateScheduler<taState>::getLatestState() {
    return _ssch.getLatestState();
}

template <class taState>
bool VerboseStateScheduler<taState>::isCurrentStateFresh() const {
    return _ssch.isCurrentStateFresh();
}

template <class taState>
const taState& VerboseStateScheduler<taState>::getCurrentState() const {
    return _ssch.getCurrentState();
}

template <class taState>
const taState& VerboseStateScheduler<taState>::getLatestState() const {
    return _ssch.getLatestState();
}

template <class taState>
typename std::vector<taState>::iterator VerboseStateScheduler<taState>::begin() {
    return _ssch.begin();
}

template <class taState>
typename std::vector<taState>::iterator VerboseStateScheduler<taState>::end() {
    return _ssch.end();
}

template <class taState>
typename std::vector<taState>::const_iterator VerboseStateScheduler<taState>::cbegin() const {
    return _ssch.cbegin();
}

template <class taState>
typename std::vector<taState>::const_iterator VerboseStateScheduler<taState>::cend() const {
    return _ssch.cend();
}

#undef VSC_DO

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_STATE_SCHEDULER_VERBOSE_HPP
