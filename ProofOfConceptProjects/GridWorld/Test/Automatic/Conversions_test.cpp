#include <GridWorld/Private/Conversions.hpp>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <gtest/gtest.h>

#include <iostream>

namespace gridworld {
namespace detail {

class GriddyConversionsTest : public ::testing::Test {
protected:
    JsonDocument   _document;
    JsonAllocator& _allocator = _document.GetAllocator();

    JsonDocument _jsonDocumentFromString(const char* aJsonString) {
        JsonDocument doc;
        doc.Parse(aJsonString);
        return doc;
    }

#if 1
    void _print(const JsonDocument& aDoc) {
        json::StringBuffer                     stringbuf;
        json::PrettyWriter<json::StringBuffer> writer(stringbuf);
        aDoc.Accept(writer);
        std::cerr << stringbuf.GetString() << std::endl;
    }

    void _print(const json::Value& aVal) {
        json::StringBuffer                     stringbuf;
        json::PrettyWriter<json::StringBuffer> writer(stringbuf);
        aVal.Accept(writer);
        std::cerr << stringbuf.GetString() << std::endl;
    }
#endif
};

TEST_F(GriddyConversionsTest, EmptyCellConversionToJsonAndBack) {
    CellModel cell;

    EXPECT_EQ(cell, JsonToCell(CellToJson(cell, _allocator)));
}

TEST_F(GriddyConversionsTest, EmptyCellConversionFromJson) {
    static constexpr auto EMPTY_CELL_JSON = R"_({ "flags": 0 })_";

    const auto cell = JsonToCell(_jsonDocumentFromString(EMPTY_CELL_JSON));

    const CellModel emptyCell{};

    EXPECT_EQ(cell, emptyCell);
}

TEST_F(GriddyConversionsTest, CellConversionFromInvalidJson) {
    static constexpr auto INVALID_CELL_JSON_1 = R"_({ "flags": "asdf" })_";
    static constexpr auto INVALID_CELL_JSON_2 = R"_({ "flaks":    30  })_";

    EXPECT_THROW(JsonToCell(_jsonDocumentFromString(INVALID_CELL_JSON_1)), JsonParseError);
    EXPECT_THROW(JsonToCell(_jsonDocumentFromString(INVALID_CELL_JSON_2)), JsonParseError);
}

TEST_F(GriddyConversionsTest, CellWithFloorConversionToJsonAndBack) {
    CellModel cell;
    cell.setFloor({111});

    EXPECT_EQ(cell, JsonToCell(CellToJson(cell, _allocator)));
}

TEST_F(GriddyConversionsTest, CellWithWallConversionToJsonAndBack) {
    CellModel cell;
    cell.setWall({111, 222, Shape::CIRCLE});

    EXPECT_EQ(cell, JsonToCell(CellToJson(cell, _allocator)));
}

TEST_F(GriddyConversionsTest, CellWithFloorAndWallConversionToJsonAndBack) {
    CellModel cell;
    cell.setFloor({111});
    cell.setWall({111, 222, Shape::CIRCLE});

    EXPECT_EQ(cell, JsonToCell(CellToJson(cell, _allocator)));
}

TEST_F(GriddyConversionsTest, Test1) {
    CellModel cell;
    cell.setFloor({111});
    cell.setWall({111, 222, Shape::FULL_SQUARE});

    Chunk chunk{4, 4, cell};
    chunk.getCellAtUnchecked(2, 0).setFloor({333});
    chunk.getCellAtUnchecked(3, 1).resetWall();
    chunk.getCellAtUnchecked(1, 2).resetFloor();

    // std::cerr << ChunkToJson(chunk) << std::endl;

    EXPECT_EQ(chunk, JsonToChunk(ChunkToJson(chunk)));
}

} // namespace detail
} // namespace gridworld
