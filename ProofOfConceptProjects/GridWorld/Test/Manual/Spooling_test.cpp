#include "../../Source/Model/Chunk_disk_io_handler_interface.hpp"
#include "../../Source/Model/Chunk_spooler_default.hpp"

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Utility/Grids.hpp>
#include <Hobgoblin/Window.hpp>

#include <memory>
#include <unordered_map>

namespace hg = jbatnozic::hobgoblin;

using gridworld::detail::Chunk;
using gridworld::detail::ChunkDiskIoHandlerInterface;
using gridworld::detail::ChunkId;
using gridworld::detail::ChunkSpoolerInterface;

namespace {
class FakeWorld {
public:
    FakeWorld(hg::PZInteger          aWorldWidth,
              hg::PZInteger          aWorldHeight,
              ChunkSpoolerInterface& aChunkSpooler)
        : _chunkspool{aChunkSpooler} {
        _grid.resize(aWorldWidth, aWorldHeight);
    }

    void update(hg::math::Vector2pz aPosition) {
        _chunkspool.pause();

        // Collect loaded chunks
        {
            auto loadedChunks = _chunkspool.getLoaded();
            for (auto& chunk : loadedChunks) {
                const auto id = chunk.id;
                if (chunk.chunk) {
                    // TODO: check that this grid location is null
                    _grid.at(id.x, id.y) = std::make_unique<Chunk>(std::move(*chunk.chunk));
                } else {
                    // TODO
                }
            }
        }

        // Unload excess tiles
        {

        }

        // Set new chunks to load
        {
            _chunkspool.setChunksToLoad({});
        }
    }

private:
    ChunkSpoolerInterface& _chunkspool;

    hg::util::RowMajorGrid<std::unique_ptr<Chunk>> _grid;
};

class Z : public ChunkDiskIoHandlerInterface {
public:
    std::optional<Chunk> loadChunkFromRuntimeCache(ChunkId aChunkId) {
        return {};
    }

    void storeChunkInRuntimeCache(const Chunk& aChunk, ChunkId aChunkId) {}

    std::optional<Chunk> loadChunkFromPersistentCache(ChunkId aChunkId) {
        return {};
    }

    void storeChunkInPersistentCache(const Chunk& aChunk, ChunkId aChunkId) {}

private:
    std::unordered_map<ChunkId, Chunk> _cache;
};

class Fixture {
public:
private:
    Z                                      _z;
    gridworld::detail::DefaultChunkSpooler _chunkSpooler{_z};
};
} // namespace

void RunSpoolingTest() {
    hg::gr::RenderWindow window;
    window.create(hg::win::VideoMode{800, 800}, "SpoolingTest");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            ev.visit([&](const hg::win::Event::Closed&) {
                window.close();
            });
        }

        // step
    }
}
