// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Private/Chunk_disk_io_handler_interface.hpp>

#include <GridGoblin/Model/Chunk.hpp>
#include <GridGoblin/Model/Chunk_id.hpp>
#include <GridGoblin/Private/Model_conversions.hpp>
#include <GridGoblin/World/World_config.hpp>

#include <filesystem>
#include <memory>
#include <optional>

namespace jbatnozic {
namespace gridgoblin {

// Forward declare
class Binder;

namespace detail {

//! Default disk I/O handles for chunks which stores each chunk as a single textual file.
class DefaultChunkDiskIoHandler : public ChunkDiskIoHandlerInterface {
public:
    DefaultChunkDiskIoHandler(const WorldConfig& aConfig);

    ~DefaultChunkDiskIoHandler() override;

    void setBinder(Binder* aBinder) override;

    std::optional<Chunk> loadChunkFromRuntimeCache(ChunkId aChunkId) override;

    void storeChunkInRuntimeCache(const Chunk& aChunk, ChunkId aChunkId) override;

    std::optional<Chunk> loadChunkFromPersistentCache(ChunkId aChunkId) override;

    void storeChunkInPersistentCache(const Chunk& aChunk, ChunkId aChunkId) override;

    void dumpRuntimeCache() override;

private:
    Binder* _binder = nullptr;

    ReusableConversionBuffers* _reusableConversionBuffers = nullptr;

    std::filesystem::path _basePath;

    std::filesystem::path _buildPathToChunk(ChunkId aChunkId) const;
};

} // namespace detail
} // namespace gridgoblin
} // namespace jbatnozic
