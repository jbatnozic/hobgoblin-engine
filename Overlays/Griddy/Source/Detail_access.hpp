// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Griddy/Model/Cell.hpp>
#include <Griddy/Model/Lights.hpp>
#include <Griddy/World/World.hpp>

namespace griddy {

// Cells

inline const detail::CellModelExt::ExtensionData& GetExtensionData(const CellModel& aCell) {
    return static_cast<const detail::CellModelExt&>(aCell).mutableExtensionData;
}

inline detail::CellModelExt::ExtensionData& GetMutableExtensionData(const CellModel& aCell) {
    return static_cast<const detail::CellModelExt&>(aCell).mutableExtensionData;
}

// Lights

inline const detail::LightModelExt::ExtensionData& GetExtensionData(const LightModel& aCell) {
    return static_cast<const detail::LightModelExt&>(aCell).mutableExtensionData;
}

inline detail::LightModelExt::ExtensionData& GetMutableExtensionData(const LightModel& aCell) {
    return static_cast<const detail::LightModelExt&>(aCell).mutableExtensionData;
}

} // namespace griddy
