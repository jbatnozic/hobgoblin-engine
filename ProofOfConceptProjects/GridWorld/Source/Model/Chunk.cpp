#include <GridWorld/Model/Chunk.hpp>

#include <Hobgoblin/HGExcept.hpp>

namespace gridworld {

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

} // namespace gridworld
