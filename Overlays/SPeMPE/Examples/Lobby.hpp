#include <SPeMPE/SPeMPE.hpp>

#include <string>
#include <utility>
#include <vector>

namespace hg = ::jbatnozic::hobgoblin;
namespace spe = ::jbatnozic::spempe;
using namespace hg::qao; // All names from QAO are prefixed with QAO_
using namespace hg::rn;  // All names from RigelNet are prefixed with RN_

struct PlayerId {
    std::string name;
    std::string ipAddress;
    std::string uniqueId;
};

class Lobby : spe::NonstateObject {
public:
    Lobby(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
        : NonstateObject(aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "Lobby")
    {
        // ccomp<spe::NetworkingManagerInterface>();
    }

    hg::PZInteger clientIdxToPlayerIdx(hg::PZInteger aClientIdx) const;

    hg::PZInteger playerIdxToClientIdx(hg::PZInteger aPlayerIdx) const {
        return _lockedIn[aPlayerIdx].clientIndex;
    }

private:
    // spe::NetworkingManagerInterface& aNetwMgr;
    struct TaggedPlayerId : PlayerId {
        hg::PZInteger clientIndex;
    };

    std::vector<TaggedPlayerId> _lockedIn;
    std::vector<TaggedPlayerId> _desired;

    hg::PZInteger getSize() const;

    void _resetDesired() {
        for (std::size_t i = 0; i < _lockedIn.size(); i += 1) {
            _desired[i] = _lockedIn[i];
        }
    }

    void _lockInDesired() {
        for (hg::PZInteger i = 0; i < getSize(); i += 1) {
            if (_lockedIn[i].clientIndex != _desired[i].clientIndex) {
                // send idx update message to _desired[i].clientIndex
                // update varmap
            }
        }
        // send total reset to all clients
        // send game state from scratch
    }

    void _swapPlayers(hg::PZInteger aPlayerIdx1, hg::PZInteger aPlayerIdx2) {
        
    }
};