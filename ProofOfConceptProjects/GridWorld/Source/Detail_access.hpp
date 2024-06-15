#pragma once

#include <GridWorld/Model/Cell.hpp>
#include <GridWorld/Model/Lights.hpp>
#include <GridWorld/World/World.hpp>

namespace gridworld {

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

} // namespace gridworld
