
#include <GridWorld/Private/Conversions.hpp>

#include <GridWorld/Model/Chunk.hpp>

#include <Hobgoblin/Common.hpp>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <string>

namespace gridworld {
namespace detail {

// namespace hg = ::jbatnozic::hobgoblin;

namespace json = rapidjson;

json::Value CellToJson(const CellModel& aCell, RAPIDJSON_DEFAULT_ALLOCATOR& aAllocator) {
  json::Value value;
  value.SetObject();
  value.AddMember("flags", json::Value{aCell.getFlags()}.Move(), aAllocator);
  return value;
}

CellModel JsonToCell(std::string aJsonString) {
  return {};
}

std::string ChunkToJson(const Chunk& aChunk) {
    json::Document doc;
    doc.SetObject();
    doc.AddMember("width", json::Value{aChunk.getCellCountX()}, doc.GetAllocator());
    doc.AddMember("height", json::Value{aChunk.getCellCountY()}, doc.GetAllocator());
    // ...
    json::Value cellArray{json::kArrayType};
    cellArray.Reserve(1, doc.GetAllocator());
    //cellArray.PushBack(<value>, doc.GetAllocator());
    doc.AddMember("cells", cellArray, doc.GetAllocator());

    json::StringBuffer                     stringbuf;
    json::Writer<json::StringBuffer> writer(stringbuf); // another option: pretty writer
    doc.Accept(writer);
    return stringbuf.GetString();
}

Chunk JsonToChunk(std::string aJsonString) {
    json::Document doc;
    doc.ParseInsitu(aJsonString.data());

    Chunk chunk{};

    return chunk;
}

/*
TODO:
  ChunkToJson
  JsonToChunk
  ChunkToBinary
  BinaryToChunk
*/

} // namespace detail
} // namespace gridworld
