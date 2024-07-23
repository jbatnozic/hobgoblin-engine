#pragma once

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

json::Value CellToJson(const CellModel& aCell, RAPIDJSON_DEFAULT_ALLOCATOR& aAllocator);

CellModel JsonToCell(std::string aJsonString);

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
