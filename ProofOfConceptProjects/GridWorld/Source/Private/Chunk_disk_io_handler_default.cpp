#pragma once

#include <GridWorld/Private/Chunk_disk_io_handler_default.hpp>

#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/Utility/File_io.hpp>

#include <GridWorld/Private/Model_conversions.hpp>

namespace gridworld {
namespace detail {

namespace {
const std::filesystem::path CHUNKS_FOLDER = "DCIO_CHUNKS";

} // namespace

DefaultChunkDiskIoHandler::~DefaultChunkDiskIoHandler() = default;

std::optional<Chunk> DefaultChunkDiskIoHandler::loadChunkFromRuntimeCache(ChunkId aChunkId) {
    return loadChunkFromPersistentCache(aChunkId);
}

void DefaultChunkDiskIoHandler::storeChunkInRuntimeCache(const Chunk& aChunk, ChunkId aChunkId) {
    storeChunkInPersistentCache(aChunk, aChunkId);
}

std::optional<Chunk> DefaultChunkDiskIoHandler::loadChunkFromPersistentCache(ChunkId aChunkId) {
    const auto path = _buildPathToChunk(aChunkId);
    if (std::filesystem::exists(path)) {

    } else {
        return std::nullopt;
    }
}

void DefaultChunkDiskIoHandler::storeChunkInPersistentCache(const Chunk& aChunk, ChunkId aChunkId) {

}

std::filesystem::path DefaultChunkDiskIoHandler::_buildPathToChunk(ChunkId aChunkId) const {
    return _basePath / CHUNKS_FOLDER / fmt::format(FMT_STRING("chunk_{}_{}"), aChunkId.x, aChunkId.y);
}

} // namespace detail
} // namespace gridworld
