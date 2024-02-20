#pragma once

#include <GridWorld/Model/Cell.hpp>
#include <GridWorld/World/World.hpp>

namespace gridworld {

inline
const detail::CellModelExt::ExtensionData& GetExtensionData(const model::Cell& aCell) {
    return static_cast<const detail::CellModelExt&>(aCell).mutableExtensionData;
}

inline
detail::CellModelExt::ExtensionData& GetMutableExtensionData(const model::Cell& aCell) {
    return static_cast<const detail::CellModelExt&>(aCell).mutableExtensionData;
}

} // namespace gridworld
