// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridWorld/Model/Chunk.hpp>
#include <GridWorld/Private/Model_conversions.hpp>

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Utility/Base64.hpp>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <optional>
#include <string>
#include <type_traits>

namespace gridworld {
namespace detail {

namespace json = rapidjson;

namespace {
constexpr auto LOG_ID = "Griddy";

#define ENSURE_JSON_CONTAINS(_value_, _member_name_, _member_type_)                    \
    do {                                                                                      \
        if (!(_value_).HasMember(_member_name_) ||                                         \
            !(_value_)[_member_name_].Is##_member_type_()) {                               \
            HG_THROW_TRACED(JsonParseError, 0, "No array member '{}' found.", _member_name_); \
        }                                                                                     \
    } while (false)

template <class taIntegral>
taIntegral GetIntMember(const json::Value& aJsonValue, const char* aMemberName) {
    static_assert(std::is_integral<taIntegral>::value, "GetIntMember only works with integral types.");

    ENSURE_JSON_CONTAINS(aJsonValue, aMemberName, Int);

    return static_cast<taIntegral>(aJsonValue[aMemberName].GetInt());
}

using ExtensionKind = std::optional<ChunkExtensionInterface::SerializationMethod>;

std::string ExtensionKindToString(const ExtensionKind& aExtensionKind) {
    if (!aExtensionKind.has_value()) {
        return "empty";
    }

    switch (*aExtensionKind) {
    case ExtensionKind::value_type::BINARY:
        return "binary";
        break;

    default:
        HG_UNREACHABLE("Invalid value for ExtensionKind ({}).", (int)*aExtensionKind);
    }
}

ExtensionKind StringToExtensionKind(const std::string& aString) {
    /*--*/ if (aString == "empty") {
        return std::nullopt;
    } else if (aString == "binary") {
        return {ExtensionKind::value_type::BINARY};
    } else {
        HG_THROW_TRACED(JsonParseError, 0, "Unsupported string: '{}'.", aString);
    }
}

void Base64Encode(
    /*  in */ const ChunkExtensionInterface&               aChunkExtension,
    /*  in */ ChunkExtensionInterface::SerializationMethod aPreferredSerializationMethod,
    /* out */ std::string&                                 aBase64EncodeBuffer,
    /*  in */ std::vector<std::uint8_t>*                   aSerializationBuffer = nullptr)
//
{
    std::vector<std::uint8_t>  defaultSerBuf;
    std::vector<std::uint8_t>* serBuf = aSerializationBuffer ? aSerializationBuffer : &defaultSerBuf;

    serBuf->clear();
    aBase64EncodeBuffer.clear();

    switch (aPreferredSerializationMethod) {
    case ChunkExtensionInterface::SerializationMethod::BINARY:
        {
            aChunkExtension.serializeToBinary(*serBuf);

            const auto encodeSizePrediction =
                hg::util::GetRecommendedOutputBufferSizeForBase64Encode(hg::stopz(serBuf->size()));

            aBase64EncodeBuffer.resize(hg::pztos(encodeSizePrediction));

            const auto encodeSizeActual = hg::util::Base64Encode(serBuf->data(),
                                                                 hg::stopz(serBuf->size()),
                                                                 aBase64EncodeBuffer.data(),
                                                                 hg::stopz(aBase64EncodeBuffer.size()));

            aBase64EncodeBuffer.resize(hg::pztos(encodeSizeActual));
        }
        break;

    default:
        HG_UNREACHABLE("Invalid value for ChunkExtensionInterface::SerializationMethod ({}).",
                       (int)aPreferredSerializationMethod);
    }
}

void Base64Decode(
    /* out */ const ChunkExtensionInterface&               aChunkExtension,
    /*  in */ ChunkExtensionInterface::SerializationMethod aPreferredSerializationMethod)
//
{
    //std::vector<std::uint8_t>  defaultSerBuf;
    //std::vector<std::uint8_t>* serBuf = aSerializationBuffer ? aSerializationBuffer : &defaultSerBuf;

    //serBuf->clear();
    //aBase64EncodeBuffer.clear();

    //switch (aPreferredSerializationMethod) {
    //case ChunkExtensionInterface::SerializationMethod::BINARY:
    //    {
    //        aChunkExtension.serializeToBinary(*serBuf);

    //        const auto encodeSizePrediction =
    //            hg::util::GetRecommendedOutputBufferSizeForBase64Encode(hg::stopz(serBuf->size()));

    //        aBase64EncodeBuffer.resize(hg::pztos(encodeSizePrediction));

    //        const auto encodeSizeActual = hg::util::Base64Encode(serBuf->data(),
    //                                                             hg::stopz(serBuf->size()),
    //                                                             aBase64EncodeBuffer.data(),
    //                                                             hg::stopz(aBase64EncodeBuffer.size()));

    //        aBase64EncodeBuffer.resize(hg::pztos(encodeSizeActual));
    //    }
    //    break;

    //default:
    //    HG_UNREACHABLE("Invalid value for ChunkExtensionInterface::SerializationMethod ({}).",
    //                   (int)aPreferredSerializationMethod);
    //}
}
} // namespace

json::Value CellToJson(const CellModel& aCell, json::Document& aDocument) {
    const auto cellFlags = aCell.getFlags();

    auto& allocator = aDocument.GetAllocator();

    json::Value value;
    value.SetObject();

    value.AddMember("flags", json::Value{cellFlags}.Move(), allocator);

    if (cellFlags & CellModel::FLOOR_INITIALIZED) {
        const auto& floor = aCell.getFloor();

        json::Value jsonFloor;
        jsonFloor.SetObject();

        jsonFloor.AddMember("spriteId", json::Value{floor.spriteId}.Move(), allocator);

        value.AddMember("floor", jsonFloor, allocator);
    }

    if (cellFlags & CellModel::WALL_INITIALIZED) {
        const auto& wall = aCell.getWall();

        json::Value jsonWall;
        jsonWall.SetObject();

        jsonWall.AddMember("spriteId", json::Value{wall.spriteId}.Move(), allocator);
        jsonWall.AddMember("spriteId_lowered", json::Value{wall.spriteId_lowered}.Move(), allocator);
        jsonWall.AddMember("shape",
                           json::Value{(int)wall.shape}.Move(),
                           allocator); // TODO: C-style cast (better to convert to name string

        value.AddMember("wall", jsonWall, allocator);
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
        floor.spriteId = GetIntMember<decltype(floor.spriteId)::WrappedType>(aJson["floor"], "spriteId");

        cell.setFloor(floor);
    }

    if (flags & CellModel::WALL_INITIALIZED) {
        if (!aJson.HasMember("wall") || !aJson["wall"].IsObject()) {
            HG_THROW_TRACED(JsonParseError, 0, "No object member '{}' found.", "wall");
        }

        CellModel::Wall wall;
        wall.spriteId = GetIntMember<decltype(wall.spriteId)::WrappedType>(aJson["wall"], "spriteId");
        wall.spriteId_lowered =
            GetIntMember<decltype(wall.spriteId_lowered)::WrappedType>(aJson["wall"],
                                                                       "spriteId_lowered");
        wall.shape = (decltype(wall.shape))GetIntMember<int>(aJson["wall"], "shape"); // TODO

        cell.setWall(wall);
    }

    return cell;
}

json::Document ChunkToJson(const Chunk& aChunk) {
    const auto chunkWidth  = aChunk.getCellCountX();
    const auto chunkHeight = aChunk.getCellCountY();

    json::Document doc;
    auto&          allocator = doc.GetAllocator();
    doc.SetObject();

    // Add dimensions
    doc.AddMember("width", json::Value{chunkWidth}.Move(), allocator);
    doc.AddMember("height", json::Value{chunkHeight}.Move(), allocator);

    // Add cells
    json::Value cellArray{json::kArrayType};
    cellArray.Reserve(chunkWidth * chunkHeight, allocator);
    for (hg::PZInteger y = 0; y < chunkHeight; y += 1) {
        for (hg::PZInteger x = 0; x < chunkWidth; x += 1) {
            auto value = CellToJson(aChunk.getCellAtUnchecked(x, y), doc);
            cellArray.PushBack(value, allocator);
        }
    }
    doc.AddMember("cells", cellArray, allocator);

    // Add extension
    const auto* extension = aChunk.getExtension();
    if (extension) {
        const auto method = extension->getPreferredSerializationMethod();
        const auto kind   = ExtensionKindToString(method);

        json::Value vKind;
        vKind.SetString(kind.c_str(), kind.size());
        doc.AddMember("extension_kind", vKind.Move(), allocator);

        std::vector<std::uint8_t> serializationBuffer; // TODO: make reusable buffer
        std::string encodeBuffer; // TODO: make reusable buffer
        Base64Encode(*extension, method, encodeBuffer, &serializationBuffer);

        json::Value vExt;
        vExt.SetString(encodeBuffer.c_str(), encodeBuffer.size());
        doc.AddMember("extension_data", vExt.Move(), allocator);
            
    } else {
        const auto kind = ExtensionKindToString(std::nullopt);

        json::Value vKind;
        vKind.SetString(kind.c_str(), kind.size());
        doc.AddMember("extension_kind", vKind.Move(), allocator);
    }

    return doc;
}

Chunk JsonToChunk(const json::Document&        aJsonDocument,
                  const ChunkExtensionFactory& aChunkExtensionFactory) {
    // Read dimensions
    const auto chunkWidth  = GetIntMember<hg::PZInteger>(aJsonDocument, "width");
    const auto chunkHeight = GetIntMember<hg::PZInteger>(aJsonDocument, "height");

    Chunk chunk{chunkWidth, chunkHeight};

    // Read cells
    ENSURE_JSON_CONTAINS(aJsonDocument, "cells", Array);

    const auto& array = aJsonDocument["cells"];
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
            chunk.getCellAtUnchecked(x, y) = CellModel{JsonToCell(*valuePtr)};
            valuePtr++;
        }
    }

    // Read extension
    ENSURE_JSON_CONTAINS(aJsonDocument, "extension_kind", String);

    auto extension = aChunkExtensionFactory ? aChunkExtensionFactory() : nullptr;

    const auto kind = StringToExtensionKind(aJsonDocument["extension_kind"].GetString());
    if (kind.has_value()) {
        if (extension != nullptr) {
            ENSURE_JSON_CONTAINS(aJsonDocument, "extension_data", String);

            const auto& value               = aJsonDocument["extension_data"];
            const auto* encodedExtString    = value.GetString();
            const auto  encodedExtStringLen = value.GetStringLength();

            Base64Decode(*extension, *kind /*, encodedExtString, encodedExtStringLen*/);
        } else {
            HG_LOG_WARN(
                LOG_ID,
                "Found serialized data for chunk extension but failed to instantiate the extension.");
        }
    }

    chunk.setExtension(std::move(extension));

    return chunk;
}

std::string ChunkToJsonString(const Chunk& aChunk) {
    const auto doc = ChunkToJson(aChunk);

    json::StringBuffer               stringbuf;
    json::Writer<json::StringBuffer> writer(stringbuf);
    doc.Accept(writer);

    return stringbuf.GetString();
}

Chunk JsonStringToChunk(std::string aJsonString, const ChunkExtensionFactory& aChunkExtensionFactory) {
    json::Document doc;
    doc.ParseInsitu(aJsonString.data());

    return JsonToChunk(doc, aChunkExtensionFactory);
}

/*
TODO:
  ChunkToBinary
  BinaryToChunk
*/

} // namespace detail
} // namespace gridworld
