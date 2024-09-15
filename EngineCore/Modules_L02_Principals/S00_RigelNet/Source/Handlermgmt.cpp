// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Handlermgmt.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>

#if HG_BUILD_TYPE == HG_DEBUG
#include <Hobgoblin/Logging.hpp>
#include <sstream>
static constexpr auto LOG_ID = "Hobgoblin.RigelNet";
#endif

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

void RN_IndexHandlers() {
    rn_detail::RN_GlobalHandlerMapper::getInstance().index();
}

namespace rn_detail {

std::size_t RN_CStringHash::operator()(const char* key) const {
    // 'Murmur One-At-A-Time 32b' hash implementation
    std::uint32_t hash{0xC613FC15u};
    for (; *key; key += 1) {
        hash ^= *key;
        hash *= 0x5BD1E995u;
        hash ^= hash >> 15;
    }
    return static_cast<std::size_t>(hash);
}

bool RN_CStringEquals::operator()(const char* x, const char* y) const {
    return (std::strcmp(x, y) == 0);
}

RN_GlobalHandlerMapper::RN_GlobalHandlerMapper() {}

RN_GlobalHandlerMapper& RN_GlobalHandlerMapper::getInstance() {
    static RN_GlobalHandlerMapper singletonInstance{};
    return singletonInstance;
}

const char* RN_GlobalHandlerMapper::nameWithId(RN_HandlerId id) const {
    return _handlerNames[id];
}

RN_HandlerFunc RN_GlobalHandlerMapper::handlerWithId(RN_HandlerId id) const {
    if (id < 0 || ToSz(id) >= _handlerPointers.size()) {
        return nullptr;
    }

    return _handlerPointers[id];
}

RN_HandlerId RN_GlobalHandlerMapper::idWithName(const char* name) const {
    for (int i = 0; i < static_cast<int>(_handlerNames.size()); i += 1) {
        if (std::strcmp(_handlerNames[i], name) == 0) {
            return static_cast<RN_HandlerId>(i);
        }
    }
    return -1;
}

void RN_GlobalHandlerMapper::addMapping(const char* name, RN_HandlerFunc func) {
    auto retpair = _rawMappings.emplace(name, func);
    assert(retpair.second); // Must be newly inserted
}

void RN_GlobalHandlerMapper::index() {
    _handlerNames.clear();
    _handlerNames.reserve(_rawMappings.size());

    _handlerPointers.clear();
    _handlerPointers.reserve(_rawMappings.size());

    for (auto pair : _rawMappings) {
        _handlerNames.push_back(pair.first);
    }

    std::sort(
        _handlerNames.begin(),
        _handlerNames.end(),
        [](const char* x, const char* y){
            return (std::strcmp(x, y) < 0);
        });

    for (auto name : _handlerNames) {
        _handlerPointers.push_back(_rawMappings[name]);
    }

#if HG_BUILD_TYPE == HG_DEBUG
    std::ostringstream oss;
    oss << "RigelNet handler table:\n";
    for (std::size_t i = 0; i < _handlerNames.size(); i += 1) {
        oss << "    " << i << ": " << _handlerNames[i] << '\n';
    }
    HG_LOG_DEBUG(LOG_ID, "{}", oss.str());
#endif
}

///////////////////////////////////////////////////////////////////////////////

RN_StaticHandlerInitializer::RN_StaticHandlerInitializer(const char* name, RN_HandlerFunc func) {
    RN_GlobalHandlerMapper::getInstance().addMapping(name, func);
}

///////////////////////////////////////////////////////////////////////////////

RN_HandlerNameToIdCacher::RN_HandlerNameToIdCacher(const char* handlerName)
    : _handlerName{handlerName}
    , _handlerId{0}
{
}

RN_HandlerId RN_HandlerNameToIdCacher::getHandlerId() {
    auto& globalMapper = RN_GlobalHandlerMapper::getInstance();
    if (std::strcmp(globalMapper.nameWithId(_handlerId), _handlerName) != 0) {
        _handlerId = globalMapper.idWithName(_handlerName);
    }
    return _handlerId;
}

} // namespace rn_detail

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
