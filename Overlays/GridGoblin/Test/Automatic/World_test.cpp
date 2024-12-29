// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/World/World.hpp>

#include <Hobgoblin/Utility/Semaphore.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <vector>

#include "Fake_disk_io_handler.hpp"

namespace jbatnozic {
namespace gridgoblin {

class WorldTest : public ::testing::Test {
public:
    WorldTest() = default;

protected:
    test::FakeDiskIoHandler _fakeDiskIoHandler;
    std::optional<World>    _world;

    World& _createWorld(const WorldConfig& aConfig) {
        _world.emplace(aConfig, &_fakeDiskIoHandler);
        return *_world;
    }

    static WorldConfig _makeDefaultConfig() {
        return {.chunkCountX                 = 8,
                .chunkCountY                 = 8,
                .cellsPerChunkX              = 8,
                .cellsPerChunkY              = 8,
                .cellResolution              = 32.f,
                .maxCellOpenness             = 5,
                .maxLoadedNonessentialChunks = 1};
    }
};

TEST_F(WorldTest, ChunkGetters) {
    auto& w = _createWorld(_makeDefaultConfig());

    const hg::math::Vector2pz pos = {9, 9};
    const ChunkId             id  = w.cellToChunkId(pos);

    EXPECT_EQ(w.getChunkAt(pos), nullptr);
    EXPECT_EQ(w.getChunkAt(pos.x, pos.y), nullptr);
    EXPECT_EQ(w.getChunkAtId(id), nullptr);
    EXPECT_EQ(w.getChunkAtUnchecked(pos), nullptr);
    EXPECT_EQ(w.getChunkAtUnchecked(pos.x, pos.y), nullptr);
    EXPECT_EQ(w.getChunkAtIdUnchecked(id), nullptr);

    const auto editPerm = w.getPermissionToEdit();

    const auto& chunk = w.getChunkAt(*editPerm, pos);
    EXPECT_EQ(&w.getChunkAt(*editPerm, pos.x, pos.y), &chunk);
    EXPECT_EQ(&w.getChunkAtId(*editPerm, id), &chunk);
    EXPECT_EQ(&w.getChunkAtUnchecked(*editPerm, pos), &chunk);
    EXPECT_EQ(&w.getChunkAtUnchecked(*editPerm, pos.x, pos.y), &chunk);
    EXPECT_EQ(&w.getChunkAtIdUnchecked(*editPerm, id), &chunk);

    EXPECT_NE(w.getChunkAt(pos), nullptr);
    EXPECT_NE(w.getChunkAt(pos.x, pos.y), nullptr);
    EXPECT_NE(w.getChunkAtId(id), nullptr);
    EXPECT_NE(w.getChunkAtUnchecked(pos), nullptr);
    EXPECT_NE(w.getChunkAtUnchecked(pos.x, pos.y), nullptr);
    EXPECT_NE(w.getChunkAtIdUnchecked(id), nullptr);

    w.prune();
}

TEST_F(WorldTest, AvailableChunkIterations) {
    _fakeDiskIoHandler.setRuntimeCacheDelay(std::chrono::milliseconds{5});
    _fakeDiskIoHandler.setPersistentCacheDelay(std::chrono::milliseconds{5});

    const auto config = WorldConfig{.chunkCountX                 = 8,
                                    .chunkCountY                 = 8,
                                    .cellsPerChunkX              = 8,
                                    .cellsPerChunkY              = 8,
                                    .cellResolution              = 32.f,
                                    .maxCellOpenness             = 5,
                                    .maxLoadedNonessentialChunks = 2};

    auto& w = _createWorld(config);

    class TestBinder : public Binder {
    public:
        TestBinder(std::function<void()> aReadyCallback)
            : _readyCallback{std::move(aReadyCallback)} {}

        void onChunkReady(ChunkId) override {
            _readyCallback();
        }

    private:
        std::function<void()> _readyCallback;
    };

    EXPECT_EQ(w.availableChunksBegin(), w.availableChunksEnd());

    const auto getAvailableChunks = [&]() -> std::vector<ChunkId> {
        std::vector<ChunkId> result;
        auto                 iter = w.availableChunksBegin();
        while (iter != w.availableChunksEnd()) {
            result.push_back(*iter);
            iter.advance();
        }
        return result;
    };

    auto area = w.createActiveArea();

    const auto setAndCheck = [&](std::vector<ChunkId> aChunksForActiveArea,
                                 std::vector<ChunkId> aExpectedAvailableChunks,
                                 hg::PZInteger        aWaitToLoadCount = 0,
                                 bool                 aSkipUpdate      = false) {
        if (aWaitToLoadCount > 0) {
            hg::util::Semaphore allLoaded{};
            TestBinder          binder{[&, cnt = aWaitToLoadCount]() mutable {
                cnt -= 1;
                if (cnt == 0) {
                    allLoaded.signal();
                }
            }};
            w.setBinder(&binder);
            area.setToChunkList(aChunksForActiveArea);
            allLoaded.wait();
            w.setBinder(nullptr);
        } else {
            area.setToChunkList(aChunksForActiveArea);
        }

        if (!aSkipUpdate) {
            w.update();
        }

        w.prune();
        EXPECT_THAT(getAvailableChunks(), testing::UnorderedElementsAreArray(aExpectedAvailableChunks));
    };

    // clang-format off
    setAndCheck(
        /* active area */ { {1, 0}, {2, 0} },
        /*    expected */ { {1, 0}, {2, 0} },
        /*  load count */ 2
    );
    setAndCheck(
        /* active area */ { {1, 1}, {2, 1} },
        /*    expected */ { {1, 1}, {2, 1}, {1, 0}, {2, 0} },
        /*  load count */ 2
    );
    setAndCheck(
        /* active area */ { {1, 2}, {2, 2} },
        /*    expected */ { {1, 2}, {2, 2}, {1, 1}, {2, 1} },
        /*  load count */ 2
    );
    setAndCheck(
        /* active area */ { },
        /*    expected */ { {1, 2}, {2, 2} },
        /*  load count */ 0
    );
    setAndCheck(
        /* active area */ { {0, 0}, {0, 1}, {0, 2} },
        /*    expected */ { {1, 2}, {2, 2} },
        /*  load count */ 3,
        /* skip update */ true
    );
    // clang-format on
}

} // namespace gridgoblin
} // namespace jbatnozic
