// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_RN_RAW_DATA_VIEW_HPP
#define UHOBGOBLIN_RN_RAW_DATA_VIEW_HPP

#include <Hobgoblin/Utility/Packet.hpp>

#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

//! Utility class to enable sending and receiving raw byte data with RigelNet
class RN_RawDataView {
public:
    RN_RawDataView(const void* data, PZInteger dataByteCount)
        : _data{data}
        , _dataByteCount{dataByteCount}
    {
    }

    RN_RawDataView()
        : RN_RawDataView{nullptr, 0}
    {
    }

    const void* getData() const noexcept {
        return _data;
    }

    PZInteger getDataSize() const noexcept {
        return _dataByteCount;
    }

    friend util::OutputStream& operator<<(util::OutputStreamExtender& ostream, const RN_RawDataView& self) {
        if (self._data != nullptr && self._dataByteCount > 0) {
            ostream << self._dataByteCount;
            ostream->appendBytes(self._data, self._dataByteCount);
        } else {
          ostream << (std::int32_t)0;
        }
        return *ostream;
    }

    friend util::InputStream& operator>>(util::InputStreamExtender& istream, RN_RawDataView& self) {
        istream->noThrow() >> self._dataByteCount;
        if (*istream) {
            self._data = istream->extractBytesNoThrow(self._dataByteCount);
        }

        return *istream;
    }

private:
    const void*  _data;
    std::int32_t _dataByteCount;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_RAW_DATA_VIEW_HPP

// clang-format on
