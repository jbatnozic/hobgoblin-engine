// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_PAKKET_HPP
#define UHOBGOBLIN_UTIL_PAKKET_HPP

#include <Hobgoblin/Utility/Streams.hpp>

#include <Hobgoblin/Math/Core.hpp>

#include <cstring>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class Pakket final
    : public OutputStream
    , public InputStream {
public:
    //! \brief Default constructor
    //!
    //! Creates an empty packet.
    Pakket() = default;

    //! \brief Copy constructor.
    Pakket(const Pakket& aOther) = default;

    //! \brief Move constructor.
    Pakket(Pakket&& aOther) = default;

    //! \brief Copy assignment operator.
    Pakket& operator=(const Pakket& aOther) = default;

    //! \brief Move assignment operator.
    Pakket& operator=(Pakket&& aOther) = default;

    //! \brief Destructor.
    ~Pakket() override = default;

    //! \brief Clear the packet (reset its whole state).
    //!
    //! After calling clear(), the packet is empty.
    //!
    //! \see append
    void clear() {
        _buffer.clear();
        _readPos = 0;
        _isValid = true;
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: OUTPUT STREAM                                                   //
    ///////////////////////////////////////////////////////////////////////////

    Pakket& appendBytes(const void* aData, PZInteger aByteCount) override { // TODO: NeverNull
        if (aData && aByteCount > 0) {
            _buffer.insert(_buffer.end(),
                           static_cast<const std::uint8_t*>(aData),
                           static_cast<const std::uint8_t*>(aData) + aByteCount);
        }
        return SELF;
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: INPUT STREAM                                                    //
    ///////////////////////////////////////////////////////////////////////////

    std::int64_t seek(std::int64_t aPosition) override {
        if (_buffer.size() == 0) {
            return 0;
        }

        const std::size_t min = 0;
        const std::size_t max = _buffer.size() - 1;

        _readPos = math::Clamp(static_cast<std::size_t>(aPosition), min, max);
        return static_cast<std::int64_t>(_readPos);
    }

    std::int64_t seekRelative(std::int64_t aPosition) override {
        if (_buffer.size() == 0) {
            return 0;
        }

        const std::size_t min = 0;
        const std::size_t max = _buffer.size() - 1;

        const std::size_t newPos =
            static_cast<std::size_t>(static_cast<std::int64_t>(_readPos) + aPosition);

        _readPos = math::Clamp(newPos, min, max);
        return static_cast<std::int64_t>(_readPos);
    }

    int read(void* aDestination, PZInteger aByteCount) override { // TODO: NeverNull
        if (!_isValid) {
            return -1;
        }

        if (aByteCount == 0) {
            return 0;
        }

        const auto rem = _buffer.size() - _readPos;
        if (rem >= pztos(aByteCount)) {
            std::memcpy(aDestination, &(_buffer[_readPos]), pztos(aByteCount));
            _readPos += pztos(aByteCount);
            return aByteCount;
        } else if (rem > 0) {
            std::memcpy(aDestination, &(_buffer[_readPos]), rem);
            _readPos += rem;
            return (int)rem;
        } else {
            return 0;
        }
    }

    void* extractBytes(PZInteger aByteCount) override {
        if (aByteCount == 0) {
            return nullptr;
        }

        const auto rem = _buffer.size() - _readPos;
        if (pztos(aByteCount) > rem) {
            _isValid = false;
            throw new double; // TODO
        }

        void* result = &(_buffer[_readPos]);
        _readPos += pztos(aByteCount);
        return result;
    }

    void* extractBytesNoThrow(PZInteger aByteCount) override {
        if (aByteCount == 0) {
            return nullptr;
        }

        const auto rem = _buffer.size() - _readPos;
        if (pztos(aByteCount) > rem) {
            _isValid = false;
            return nullptr;
        }

        void* result = &(_buffer[_readPos]);
        _readPos += pztos(aByteCount);
        return result;
    }

    bool isPredetermined() const override {
        return true;
    }

    std::int64_t getDataSize() const override {
        return static_cast<std::int64_t>(_buffer.size());
    }

    std::int64_t getRemainingDataSize() const override {
        return static_cast<std::int64_t>(_buffer.size() - _readPos);
    }

    const void* getData() const override {
        return _buffer.data();
    }

    std::int64_t getReadPosition() const override {
        return static_cast<std::int64_t>(_readPos);
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: TESTING VALIDITY                                                //
    ///////////////////////////////////////////////////////////////////////////

    bool hasError() const override {
        return !_isValid;
    }

    operator StreamBool::BoolType() const {
        return hasError() ? nullptr : &StreamBool::dummy;
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: NOTHROW ADAPTER                                                 //
    ///////////////////////////////////////////////////////////////////////////

    HG_DISAMBIGUATE_NOTHROW(Pakket)

private:
    std::vector<std::uint8_t> _buffer;         //!< Buffer of data stored in the packet
    std::size_t               _readPos = 0;    //!< Current reading position in the packet
    bool                      _isValid = true; //!< Reading state of the packet

    void _setError() override {
        _isValid = false;
    }
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_PAKKET_HPP
