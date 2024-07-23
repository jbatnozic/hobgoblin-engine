#pragma once

#include <GridWorld/Model/Chunk.hpp>

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <rapidjson/document.h>

#include <string>

namespace gridworld {
namespace detail {

// namespace hg = ::jbatnozic::hobgoblin;

namespace json = rapidjson;

using JsonDocument  = json::Document;
using JsonAllocator = JsonDocument::AllocatorType;

class JsonParseError : public jbatnozic::hobgoblin::TracedRuntimeError {
public:
    using jbatnozic::hobgoblin::TracedRuntimeError::TracedRuntimeError;
};

json::Value CellToJson(const CellModel& aCell, JsonAllocator& aAllocator);

CellModel JsonToCell(const json::Value& aJsonString);

std::string ChunkToJson(const Chunk& aChunk);

Chunk JsonToChunk(std::string aJsonString);

/*
TODO:
  ChunkToJson
  JsonToChunk
  ChunkToBinary
  BinaryToChunk
*/

} // namespace detail
} // namespace gridworld
