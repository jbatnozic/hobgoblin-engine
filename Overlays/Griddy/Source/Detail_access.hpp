// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Griddy/Model/Cell.hpp>
#include <Griddy/World/World.hpp>

namespace jbatnozic {
namespace griddy {

// Cells

inline const detail::CellModelExt::ExtensionData& GetExtensionData(const CellModel& aCell) {
    return static_cast<const detail::CellModelExt&>(aCell).mutableExtensionData;
}

inline detail::CellModelExt::ExtensionData& GetMutableExtensionData(const CellModel& aCell) {
    return static_cast<const detail::CellModelExt&>(aCell).mutableExtensionData;
}

} // namespace griddy
} // namespace jbatnozic
