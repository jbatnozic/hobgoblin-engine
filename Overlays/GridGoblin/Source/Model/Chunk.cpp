// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/Model/Chunk.hpp>

#include <GridGoblin/Model/Chunk_extension.hpp>

#include <Hobgoblin/HGExcept.hpp>

namespace jbatnozic {
namespace gridgoblin {

Chunk::Chunk(hg::PZInteger aWidth, hg::PZInteger aHeight)
    : _cells{aWidth, aHeight} {
    if (!isEmpty()) {
        _storeChunkExtensionPointer(nullptr);
    }
}

Chunk::~Chunk() {
    makeEmpty();
}

void Chunk::makeEmpty() {
    if (_cells.getWidth() != 0 && _cells.getHeight() != 0) {
        releaseExtension().reset();
        _cells.reset();
    }
}

void Chunk::setAll(const CellModel& aCell) {
    for (hg::PZInteger y = 0; y < getCellCountY(); y += 1) {
        for (hg::PZInteger x = 0; x < getCellCountX(); x += 1) {
            _cells[y][x] = detail::CellModelExt{aCell};
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// EXTENSIONS                                                            //
///////////////////////////////////////////////////////////////////////////

void Chunk::setExtension(std::unique_ptr<ChunkExtensionInterface> aChunkExtension) {
    releaseExtension().reset();
    _storeChunkExtensionPointer(aChunkExtension.release());
}

ChunkExtensionInterface* Chunk::getExtension() const {
    return _loadChunkExtensionPointer();
}

std::unique_ptr<ChunkExtensionInterface> Chunk::releaseExtension() {
    std::unique_ptr<ChunkExtensionInterface> result{_loadChunkExtensionPointer()};
    _storeChunkExtensionPointer(nullptr);

    return result;
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE                                                               //
///////////////////////////////////////////////////////////////////////////

void Chunk::_storeChunkExtensionPointer(ChunkExtensionInterface* aChunkExtensionPointer) {
    HG_ASSERT(!isEmpty());
    _cells.getExtensionCell().mutableExtensionData.setChunkExtensionPointer(aChunkExtensionPointer);
}

ChunkExtensionInterface* Chunk::_loadChunkExtensionPointer() const {
    HG_ASSERT(!isEmpty());
    return _cells.getExtensionCell().mutableExtensionData.getChunkExtensionPointer();
}

///////////////////////////////////////////////////////////////////////////
// FREE FUNCTIONS                                                        //
///////////////////////////////////////////////////////////////////////////

namespace detail {
bool AreCellsEqual(const Chunk& aChunk1, const Chunk& aChunk2) {
    if (aChunk1.getCellCountX() != aChunk2.getCellCountX() ||
        aChunk1.getCellCountY() != aChunk2.getCellCountY()) {
        return false;
    }

    for (hg::PZInteger y = 0; y < aChunk1.getCellCountY(); y += 1) {
        for (hg::PZInteger x = 0; x < aChunk2.getCellCountX(); x += 1) {
            const auto& lhsCell = static_cast<const CellModel&>(aChunk1.getCellAtUnchecked(x, y));
            const auto& rhsCell = static_cast<const CellModel&>(aChunk2.getCellAtUnchecked(x, y));
            if (lhsCell != rhsCell) {
                return false;
            }
        }
    }

    return true;
}
} // namespace detail

} // namespace gridgoblin
} // namespace jbatnozic
