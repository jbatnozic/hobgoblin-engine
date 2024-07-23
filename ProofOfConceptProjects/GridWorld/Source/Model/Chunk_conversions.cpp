
#include <GridWorld/Private/Conversions.hpp>

#include <GridWorld/Model/Chunk.hpp>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <type_traits>

namespace gridworld {
namespace detail {

namespace json = rapidjson;

namespace {
template <class taIntegral>
taIntegral GetIntMember(const json::Value& aJsonValue, const char* aMemberName) {
    static_assert(std::is_integral<taIntegral>::value, "GetIntMember only works with integral types.");

    if (!aJsonValue.HasMember(aMemberName) || !aJsonValue[aMemberName].IsInt()) {
        HG_THROW_TRACED(JsonParseError, 0, "No int member '{}' found.", aMemberName);
    }

    return static_cast<taIntegral>(aJsonValue[aMemberName].GetInt());
}
} // namespace

json::Value CellToJson(const CellModel& aCell, JsonAllocator& aAllocator) {
    const auto cellFlags = aCell.getFlags();

    json::Value value;
    value.SetObject();

    value.AddMember("flags", json::Value{cellFlags}.Move(), aAllocator);

    if (cellFlags & CellModel::FLOOR_INITIALIZED) {
        const auto& floor = aCell.getFloor();

        json::Value jsonFloor;
        jsonFloor.SetObject();

        jsonFloor.AddMember("spriteId", json::Value{floor.spriteId}.Move(), aAllocator);

        value.AddMember("floor", jsonFloor, aAllocator);
    }

    if (cellFlags & CellModel::WALL_INITIALIZED) {
        const auto& wall = aCell.getWall();

        json::Value jsonWall;
        jsonWall.SetObject();

        jsonWall.AddMember("spriteId", json::Value{wall.spriteId}.Move(), aAllocator);
        jsonWall.AddMember("spriteId_lowered", json::Value{wall.spriteId_lowered}.Move(), aAllocator);
        jsonWall.AddMember("shape",
                           json::Value{(int)wall.shape}.Move(),
                           aAllocator); // TODO: C-style cast (better to convert to name string

        value.AddMember("wall", jsonWall, aAllocator);
    }

    return value;
}

CellModel JsonToCell(const json::Value& aJson) {
    CellModel cell;

    const auto flags = GetIntMember<decltype(cell.getFlags())>(aJson, "flags");

    if (flags & CellModel::FLOOR_INITIALIZED) {
        if (!aJson.HasMember("floor") || !aJson["floor"].IsObject()) {
            HG_THROW_TRACED(JsonParseError, 0, "No object member '{}' found.", "floor");
        }

        CellModel::Floor floor;
        floor.spriteId = GetIntMember<decltype(floor.spriteId)>(aJson["floor"], "spriteId");

        cell.setFloor(floor);
    }

    if (flags & CellModel::WALL_INITIALIZED) {
        if (!aJson.HasMember("wall") || !aJson["wall"].IsObject()) {
            HG_THROW_TRACED(JsonParseError, 0, "No object member '{}' found.", "wall");
        }

        CellModel::Wall wall;
        wall.spriteId = GetIntMember<decltype(wall.spriteId)>(aJson["wall"], "spriteId");
        wall.spriteId_lowered =
            GetIntMember<decltype(wall.spriteId_lowered)>(aJson["wall"], "spriteId_lowered");
        wall.shape = (decltype(wall.shape))GetIntMember<int>(aJson["wall"], "shape"); // TODO

        cell.setWall(wall);
    }

    return cell;
}

std::string ChunkToJson(const Chunk& aChunk) {
    const auto chunkWidth  = aChunk.getCellCountX();
    const auto chunkHeight = aChunk.getCellCountY();

    json::Document doc;
    doc.SetObject();

    // Add dimensions
    doc.AddMember("width", json::Value{chunkWidth}.Move(), doc.GetAllocator());
    doc.AddMember("height", json::Value{chunkHeight}.Move(), doc.GetAllocator());

    // Add cells
    json::Value cellArray{json::kArrayType};
    cellArray.Reserve(chunkWidth * chunkHeight, doc.GetAllocator());
    for (hg::PZInteger y = 0; y < chunkHeight; y += 1) {
        for (hg::PZInteger x = 0; x < chunkWidth; x += 1) {
            auto value = CellToJson(aChunk.getCellAtUnchecked(x, y), doc.GetAllocator());
            cellArray.PushBack(value, doc.GetAllocator());
        }
    }
    doc.AddMember("cells", cellArray, doc.GetAllocator());

    // Add extension
    // TODO

    json::StringBuffer               stringbuf;
    json::Writer<json::StringBuffer> writer(stringbuf);
    doc.Accept(writer);
    return stringbuf.GetString();
}

Chunk JsonToChunk(std::string aJsonString) {
    json::Document doc;
    doc.ParseInsitu(aJsonString.data());

    // Read dimensions
    const auto chunkWidth  = GetIntMember<hg::PZInteger>(doc, "width");
    const auto chunkHeight = GetIntMember<hg::PZInteger>(doc, "height");

    Chunk chunk{chunkWidth, chunkHeight};

    // Read cells
    if (!doc.HasMember("cells") || !doc["cells"].IsArray()) {
        HG_THROW_TRACED(JsonParseError, 0, "No array member '{}' found.", "cells");
    }

    const auto& array = doc["cells"];
    if (array.Size() != hg::pztos(chunkWidth * chunkHeight)) {
        HG_THROW_TRACED(JsonParseError,
                        0,
                        "Cell array is {} (expected {}).",
                        array.Size(),
                        chunkWidth * chunkHeight);
    }

    auto* valuePtr = array.Begin();
    for (hg::PZInteger y = 0; y < chunkHeight; y += 1) {
        for (hg::PZInteger x = 0; x < chunkWidth; x += 1) {
            chunk.getCellAtUnchecked(x, y) = CellModelExt{JsonToCell(*valuePtr)};
            valuePtr++;
        }
    }

    // Read extension
    // TODO

    return chunk;
}

/*
TODO:
  ChunkToBinary
  BinaryToChunk
*/

} // namespace detail
} // namespace gridworld
