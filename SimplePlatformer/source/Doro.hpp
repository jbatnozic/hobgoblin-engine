#pragma once

#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include <cstdint>
#include <functional>

using RemoteId = std::int64_t;
using CreatorFunc = std::function<RemoteId(hg::RN_Node&, hg::util::Packet&)>;

class SynchronizedObject {
public:

private:

};

// Before main() map name##tag to creator func