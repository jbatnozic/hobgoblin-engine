#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_ID_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_ID_HPP

#include <cstdint>

namespace jbatnozic {
namespace spempe {

//! Number identifying a SynchronizedObject.
using SyncId = std::uint64_t;

constexpr SyncId SYNC_ID_NEW = 0;

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_SYNC_ID_HPP
