#pragma once

#include <GridWorld/Model/Chunk.hpp>

#include <Hobgoblin/Common.hpp>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <string>

namespace gridworld {
namespace detail {

// namespace hg = ::jbatnozic::hobgoblin;

namespace json = rapidjson;

std::string ChunkToJson(const Chunk& aChunk) {
  json::Document doc;
  doc.SetObject();
  doc.AddMember("width", json::Value{aChunk.getWidth()}, doc.GetAllocator());
  doc.AddMember("height", json::Value{aChunk.getHeight()}, doc.GetAllocator());
  // ...
  json::StringBuffer stringbuf;
  json::PrettyWriter<json::StringBuffer> writer(stringbuf);
  doc.Accept(writer);
  return stringbuf.GetString();
}

Chunk JsonToChunk(std::string aJsonString) {
  json::Document doc;
  doc.ParseInsitu(aJsonString.data());

  Chunk chunk{};
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
