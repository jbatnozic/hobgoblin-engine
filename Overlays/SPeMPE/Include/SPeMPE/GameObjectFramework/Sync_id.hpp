// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

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

// clang-format on
