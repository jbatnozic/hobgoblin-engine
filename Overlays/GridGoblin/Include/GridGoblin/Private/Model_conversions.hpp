// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Model/Cell.hpp>
#include <GridGoblin/Model/Chunk.hpp>
#include <GridGoblin/Model/Chunk_extension.hpp>

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include <rapidjson/document.h>

#include <functional>
#include <memory>
#include <string>

namespace jbatnozic {
namespace gridgoblin {
namespace detail {

///////////////////////////////////////////////////////////////////////////
// JSON CONVERSIONS (FOR PERSISTENT STORAGE)                             //
///////////////////////////////////////////////////////////////////////////

namespace json = rapidjson;

class JsonParseError : public jbatnozic::hobgoblin::TracedRuntimeError {
public:
    using jbatnozic::hobgoblin::TracedRuntimeError::TracedRuntimeError;
};

using ChunkExtensionFactory = std::function<std::unique_ptr<ChunkExtensionInterface>(const Chunk&)>;

using A = std::string;
using B = hg::util::Packet;

//! Creates a JSON Value from the passed CellModel instance using the allocator
//! of the passed document (does not attach the Value anywhere).
//!
//! \note Any data the cell might be extended with will NOT be included.
json::Value CellToJson(const CellModel& aCell, json::Document& aJsonDocument);

//! Creates a CellModel instance from the passed JSON Value.
//!
//! \throws JsonParseError is the JSON data does not correspond to the expected format.
CellModel JsonToCell(const json::Value& aJsonValue);

//! Creates a JSON Document from the passed Chunk.
//!
//! \note Any data the chunk's cells might be extended with will NOT be included.
json::Document ChunkToJson(const Chunk& aChunk);

//! Creates a Chunk from the passed JSON Document.
//!
//! \throws JsonParseError is the JSON data does not correspond to the expected format.
Chunk JsonToChunk(const json::Document&        aJsonDocument,
                  const ChunkExtensionFactory& aChunkExtensionFactory = nullptr);

//! Creates a JSON-encoded string (without whitespaces, newlines or any pretty
//! formatting) from the passed Chunk.
//!
//! \note Any data the chunk's cells might be extended with will NOT be included.
std::string ChunkToJsonString(const Chunk& aChunk);

//! Creates a Chunk from the passed JSON-encoded string.
//!
//! \throws JsonParseError is the JSON data does not correspond to the expected format.
Chunk JsonStringToChunk(std::string                  aJsonString,
                        const ChunkExtensionFactory& aChunkExtensionFactory = nullptr);

} // namespace detail
} // namespace gridgoblin
} // namespace jbatnozic
