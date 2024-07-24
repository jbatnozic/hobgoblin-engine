#include "../../Source/Model/Chunk_disk_io_handler_interface.hpp"
#include "../../Source/Model/Chunk_spooler_default.hpp"

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Utility/Grids.hpp>
#include <Hobgoblin/Window.hpp>

#include <memory>
#include <thread>
#include <unordered_map>

namespace hg = jbatnozic::hobgoblin;

using gridworld::detail::Chunk;
using gridworld::detail::ChunkDiskIoHandlerInterface;
using gridworld::detail::ChunkId;
using gridworld::detail::ChunkSpoolerInterface;

#define CHUNK_COUNT_X 32
#define CHUNK_COUNT_Y 32

namespace {
class FakeWorld {
public:
    FakeWorld(hg::PZInteger          aWorldWidth,
              hg::PZInteger          aWorldHeight,
              ChunkSpoolerInterface& aChunkSpooler)
        : _chunkspool{aChunkSpooler} {
        _chunkGrid.resize(aWorldWidth, aWorldHeight);
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
                    _chunkGrid.at(id.y, id.x) = std::make_unique<Chunk>(std::move(*chunk.chunk));
                } else {
                    // create new one
                    _chunkGrid.at(id.y, id.x) = std::make_unique<Chunk>(1, 1);
                }
            }
        }

        // Unload excess tiles
        {
            for (hg::PZInteger y = 0; y < _chunkGrid.getHeight(); y += 1) {
                for (hg::PZInteger x = 0; x < _chunkGrid.getWidth(); x += 1) {
                    auto& chunkPtr = _chunkGrid.at(y, x);
                    if (!chunkPtr) {
                        continue;
                    }
                    if (std::abs(_playerPosition.x - x) > 2 || std::abs(_playerPosition.y - y) > 2) {
                        _chunkspool.unloadChunk({(std::uint16_t)x, (std::uint16_t)y}, std::move(*chunkPtr));
                        chunkPtr.reset();
                    }
                }
            }
        }

        // Set new chunks to load
        {
            std::vector<ChunkSpoolerInterface::LoadRequest> loadRequests;
            for (int yOff = -2; yOff <= 2; yOff += 1) {
                for (int xOff = -2; xOff <= 2; xOff += 1) {
                    const auto x = aPosition.x + xOff;
                    const auto y = aPosition.y + yOff;
                    if (x >= 0 && x < CHUNK_COUNT_X && y >= 0 && y < CHUNK_COUNT_Y) {
                        if (_chunkGrid[y][x] == nullptr) {
                            ChunkSpoolerInterface::LoadRequest lr;
                            lr.chunkId = {(std::uint16_t)x, (std::uint16_t)y};
                            lr.priority = 0;
                            loadRequests.push_back(lr);
                        }
                    }
                }
            }
            _chunkspool.setChunksToLoad(std::move(loadRequests));
        }

        _playerPosition = aPosition;
        _chunkspool.unpause();
    }

    void draw(hg::gr::RenderTarget& aTarget) {
        const float resolution = 16.f;

        hg::gr::RectangleShape emptyRect{{resolution, resolution}};
        emptyRect.setFillColor(hg::gr::COLOR_TRANSPARENT);
        emptyRect.setOutlineColor(hg::gr::COLOR_BLACK);
        emptyRect.setOutlineThickness(2.f);

        hg::gr::RectangleShape filledRect{{resolution, resolution}};
        filledRect.setFillColor(hg::gr::COLOR_GREEN);

        for (hg::PZInteger y = 0; y < _chunkGrid.getHeight(); y += 1) {
            for (hg::PZInteger x = 0; x < _chunkGrid.getWidth(); x += 1) {
                if (_chunkGrid[y][x] == nullptr) {
                    emptyRect.setPosition({x * resolution, y * resolution});
                    aTarget.draw(emptyRect);
                } else {
                    filledRect.setPosition({x * resolution, y * resolution});
                    aTarget.draw(filledRect);
                }
                if (x == _playerPosition.x && y == _playerPosition.y) {
                    hg::gr::CircleShape circle{resolution / 4.f};
                    circle.setFillColor(hg::gr::COLOR_RED);
                    circle.setOrigin({resolution / 4.f, resolution / 4.f});
                    circle.setPosition({(x + 0.5f) * resolution, (y + 0.5f) * resolution});
                    aTarget.draw(circle);
                }
            }
        }
    }

private:
    ChunkSpoolerInterface& _chunkspool;

    hg::util::RowMajorGrid<std::unique_ptr<Chunk>> _chunkGrid;

    hg::math::Vector2pz _playerPosition{0, 0};
};

class FakeDiskIoHandler : public ChunkDiskIoHandlerInterface {
public:
    std::optional<Chunk> loadChunkFromRuntimeCache(ChunkId aChunkId) {
        std::this_thread::sleep_for(std::chrono::milliseconds{50});

        const auto iter = _runtimeCache.find(aChunkId);
        if (iter == _runtimeCache.end()) {
            return {};
        }
        Chunk result = std::move(iter->second);
        _runtimeCache.erase(iter);
        return result;
    }

    void storeChunkInRuntimeCache(const Chunk& aChunk, ChunkId aChunkId) {
        std::this_thread::sleep_for(std::chrono::milliseconds{50});
        _runtimeCache[aChunkId] = aChunk;   
    }

    std::optional<Chunk> loadChunkFromPersistentCache(ChunkId aChunkId) {
        std::this_thread::sleep_for(std::chrono::milliseconds{200});

        const auto iter = _persistentCache.find(aChunkId);
        if (iter == _persistentCache.end()) {
            return {};
        }
        Chunk result = std::move(iter->second);
        _persistentCache.erase(iter);
        return result;
    }

    void storeChunkInPersistentCache(const Chunk& aChunk, ChunkId aChunkId) {
        std::this_thread::sleep_for(std::chrono::milliseconds{200});
        _persistentCache[aChunkId] = aChunk; 
    }

private:
    std::unordered_map<ChunkId, Chunk> _runtimeCache;
    std::unordered_map<ChunkId, Chunk> _persistentCache;
};

class Fixture {
public:
    FakeDiskIoHandler                      _fakeDiskIoHandler;
    gridworld::detail::DefaultChunkSpooler _chunkSpooler{_fakeDiskIoHandler};
    FakeWorld                              _fakeWorld{32, 32, _chunkSpooler};
};
} // namespace

void RunSpoolingTest() {
    Fixture fixture;

    hg::gr::RenderWindow window;
    window.create(hg::win::VideoMode{800, 800}, "SpoolingTest");
    window.setViewCount(1);
    window.getView(0).setCenter({16*16.f, 16*16.f});
    window.getView(0).setSize({32*16.f, 32*16.f});
    window.getView(0).setViewport({0.f, 0.f, 1.f, 1.f});
    window.setFramerateLimit(60);

    hg::math::Vector2pz playerPosition{0, 0};
    fixture._fakeWorld.update(playerPosition);

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            ev.visit([&](const hg::win::Event::Closed&) {
                window.close();
            },
            [&](const hg::win::Event::KeyPressed& aData) {
                switch (aData.physicalKey) {
                    case hg::in::PK_LEFT:
                        if (playerPosition.x > 0) {
                            playerPosition.x -= 1;
                        }
                        break;

                    case hg::in::PK_RIGHT:
                        if (playerPosition.x < CHUNK_COUNT_X - 1) {
                            playerPosition.x += 1;
                        }
                        break;

                    case hg::in::PK_UP:
                        if (playerPosition.y > 0) {
                            playerPosition.y -= 1;
                        }
                        break;

                    case hg::in::PK_DOWN:
                        if (playerPosition.y < CHUNK_COUNT_Y - 1) {
                            playerPosition.y += 1;
                        }
                        break;

                    default:
                    break;
                }
            });
        }

        window.clear(hg::gr::COLOR_LIGHT_GRAY);
        fixture._fakeWorld.update(playerPosition);
        fixture._fakeWorld.draw(window);
        window.display();
    }
}
