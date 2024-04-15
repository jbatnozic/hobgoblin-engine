// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <SPeMPE/GameContext/Context_components.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <cassert>
#include <iomanip>
#include <sstream>

#include <Hobgoblin/Common.hpp>

namespace jbatnozic {
namespace spempe {

ContextComponent::~ContextComponent() = default;

namespace detail {

ComponentTable::ComponentTable(hobgoblin::PZInteger aTableSize) {
    _table.resize(hobgoblin::pztos(aTableSize));
}

void ComponentTable::detachComponent(ContextComponent& aComponent) {
    for (auto& entry : _table) {
        if (entry.component == std::addressof(aComponent)) {
            entry.component = nullptr;
            entry.tagHash = 0;
        }
    }
}

std::string ComponentTable::toString(char aSeparator) const {
    std::stringstream ss;
    for (std::size_t i = 0; i < _table.size(); i += 1) {
        if (_table[i].component == nullptr) {
            continue;
        }

        ss << std::setw(3) << i << ": " << _table[i].tagHash 
           << " (" << _table[i].tag << ")" << aSeparator;
    }
    
    auto result = ss.str();

    if (result.empty()) {
        return "<Component table empty>";
    }

    result.pop_back(); // Remove the trailing separator
    return result;
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

void ComponentTable::_attachComponent(ContextComponent& aComponent,
                                      std::string aTag,
                                      ContextComponent::TagHash aTagHash) {
    for (const auto& entry : _table) {
        if (entry.tagHash == aTagHash) {
            HG_THROW_TRACED(hobgoblin::TracedLogicError, 0, "Tag hash collision ({})", aTagHash);
        }
    }

    const auto pos = aTagHash % _table.size();

    for (std::size_t i = 0; i < _table.size(); i += 1) {
        auto& node = _table[(pos + i) % _table.size()];
        if (node.component == nullptr) {
            node.component = std::addressof(aComponent);
            node.tagHash = aTagHash;
            node.tag = std::move(aTag);
            return;
        }
    }

    HG_THROW_TRACED(hobgoblin::TracedLogicError, 0, "No space for component.");
}

ContextComponent* ComponentTable::_getComponentPtr(ContextComponent::TagHash aTagHash) const {
    const auto pos = aTagHash % _table.size();

    if (_table[pos].tagHash == aTagHash) {
        assert(_table[pos].component != nullptr);
        return _table[pos].component;
    }

    for (std::size_t i = 1; i < _table.size(); i += 1) {
        auto& node = _table[(pos + i) % _table.size()];
        if (node.tagHash == aTagHash) {
            assert(node.component != nullptr);
            return node.component;
        }
    }

    return nullptr;
}

ContextComponent& ComponentTable::_getComponent(ContextComponent::TagHash aTagHash) const {
    if (const auto result = _getComponentPtr(aTagHash)) {
        return *result;
    }

    HG_THROW_TRACED(hobgoblin::TracedLogicError, 0, "Component not present.");
}

} // namespace detail
} // namespace spempe
} // namespace jbatnozic

// clang-format on
