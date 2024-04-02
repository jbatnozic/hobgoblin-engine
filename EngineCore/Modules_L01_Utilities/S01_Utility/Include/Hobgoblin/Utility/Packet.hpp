#ifndef UHOBGOBLIN_UTILITY_PACKET_HPP
#define UHOBGOBLIN_UTILITY_PACKET_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Unicode/Unicode_string.hpp>

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! Forward-declare the Packet class.
class Packet;

//! Forward-declare PacketExtender.
//!
//! This class is used to define append and extract operations into/from a Packet.
class PacketExtender;

// Forward-declare append operators
Packet& operator<<(PacketExtender& aPacket, bool                 aData);
Packet& operator<<(PacketExtender& aPacket, std::int8_t          aData);
Packet& operator<<(PacketExtender& aPacket, std::uint8_t         aData);
Packet& operator<<(PacketExtender& aPacket, std::int16_t         aData);
Packet& operator<<(PacketExtender& aPacket, std::uint16_t        aData);
Packet& operator<<(PacketExtender& aPacket, std::int32_t         aData);
Packet& operator<<(PacketExtender& aPacket, std::uint32_t        aData);
Packet& operator<<(PacketExtender& aPacket, std::int64_t         aData);
Packet& operator<<(PacketExtender& aPacket, std::uint64_t        aData);
Packet& operator<<(PacketExtender& aPacket, float                aData);
Packet& operator<<(PacketExtender& aPacket, double               aData);
Packet& operator<<(PacketExtender& aPacket, std::string_view     aData);
Packet& operator<<(PacketExtender& aPacket, const UnicodeString& aData);
Packet& operator<<(PacketExtender& aPacket, const Packet&        aData);

// Forward-declare extract operators
Packet& operator>>(PacketExtender& aPacket, bool&                aData);
Packet& operator>>(PacketExtender& aPacket, std::int8_t&         aData);
Packet& operator>>(PacketExtender& aPacket, std::uint8_t&        aData);
Packet& operator>>(PacketExtender& aPacket, std::int16_t&        aData);
Packet& operator>>(PacketExtender& aPacket, std::uint16_t&       aData);
Packet& operator>>(PacketExtender& aPacket, std::int32_t&        aData);
Packet& operator>>(PacketExtender& aPacket, std::uint32_t&       aData);
Packet& operator>>(PacketExtender& aPacket, std::int64_t&        aData);
Packet& operator>>(PacketExtender& aPacket, std::uint64_t&       aData);
Packet& operator>>(PacketExtender& aPacket, float&               aData);
Packet& operator>>(PacketExtender& aPacket, double&              aData);
Packet& operator>>(PacketExtender& aPacket, std::string&         aData);
Packet& operator>>(PacketExtender& aPacket, UnicodeString&       aData);
Packet& operator>>(PacketExtender& aPacket, Packet&              aData);

//! Traced exception that's throws when extracting data from a Packet fails.
class PacketExtractError : public TracedRuntimeError {
public:
  using TracedRuntimeError::TracedRuntimeError;
};

//! \brief Utility class to build blocks of data to transfer over the network.
class Packet {
public:
    //! \brief Default constructor
    //!
    //! Creates an empty packet.
    Packet();

    //! \brief Copy constructor.
    Packet(const Packet& aOther) = default;

    //! \brief Move constructor.
    Packet(Packet&& aOther) = default;

    //! \brief Copy assignment operator.
    Packet& operator=(const Packet& aOther) = default;

    //! \brief Move assignment operator.
    Packet& operator=(Packet&& aOther) = default;

    //! \brief Destructor.
    ~Packet();

    //! \brief Clear the packet (reset its whole state).
    //!
    //! After calling clear(), the packet is empty.
    //!
    //! \see append
    void clear();

    //! \brief Get the current reading position in the packet
    //!
    //! The next read operation will read data from this position
    //!
    //! \return The byte offset of the current read position
    //!
    //! \see append
    PZInteger getReadPosition() const;

    //! \brief Get a pointer to the data contained in the Packet.
    //!
    //! \warning the returned pointer may become invalid after
    //! you append data to the packet, therefore it should never
    //! be stored.
    //!
    //! \returns Pointer to the data, or `nullptr` if the Packet is empty.
    //!
    //! \see getDataSize, getMutableData
    const void* getData() const;

    //! \brief Get a non-const pointer to the data contained in the Packet.
    //! 
    //! \warning the returned pointer may become invalid after
    //! you append data to the packet, therefore it should never
    //! be stored.
    //! 
    //! \warning the returned pointer will allow you to edit the data stored
    //! in the packet. DO NOT do this unless you're sure you know what you're
    //! doing.
    //! 
    //! \returns Pointer to the data, or `nullptr` if the Packet is empty.
    //!
    //! \see getData, getDataSize
    void* getMutableData();

    //! \brief Get the size of the data contained in the packet
    //!
    //! This function returns the number of bytes pointed to by
    //! what getData returns.
    //!
    //! \return Data size, in bytes
    //!
    //! \see getData
    PZInteger getDataSize() const;

    //! TODO
    PZInteger getRemainingDataSize() const;

    //! \brief Tell if the reading position has reached the
    //!        end of the packet
    //!
    //! This function is useful to know if there is some data
    //! left to be read, without actually reading it.
    //!
    //! \return True if all data was read, false otherwise
    //!
    //! \see operator bool
    bool endOfPacket() const;

    ///////////////////////////////////////////////////////////////////////////
    // APPENDING                                                             //
    ///////////////////////////////////////////////////////////////////////////

    //! TODO
    template <class T>
    void append(T&& aData);

    //! \brief Append data to the end of the packet
    //!
    //! \param aData      Pointer to the sequence of bytes to append
    //! \param aByteCount Number of bytes to append
    //!
    //! \see clear
    //! \see getReadPosition
    void appendBytes(const void* aData, PZInteger aByteCount);

    //! Compile-time checker for whether a type has a compatible append operator<< into `hg::util::Packet`.
    template <class taType>
    class supports_appending_of {
        template <class taType2>
        static auto test(int)
            -> decltype( std::declval<PacketExtender&>() << std::declval<const taType2&>(), std::true_type() );

        template <class>
        static auto test(...) -> std::false_type;

    public:
        static const bool value = decltype(test<taType>(0))::value;
    };

    template <class T>
    auto operator<<(T&& aData) -> std::enable_if_t<supports_appending_of<T>::value, Packet&>;

    ///////////////////////////////////////////////////////////////////////////
    // EXTRACTING                                                            //
    ///////////////////////////////////////////////////////////////////////////

    template <class T>
    T extract();

    template <class T>
    T extractNoThrow();

    //! \brief Extracts raw data from the packet; throws on failure.
    //! 
    //! Returns a pointer to a buffer of size N containing the next N bytes to be
    //! extracted from the Packet, where N is equal to `aByteCount`. These bytes
    //! can then be modified, copied, read directly or otherwise used. Clearing
    //! the Packet or appending more data to it will invalidate the returned pointer.
    //! In either case, this function will advance the Packet's internal read counter
    //! by `aByteCount`.
    //!
    //! \param aByteCount How many bytes to extract.
    //!
    //! \returns Pointer to data to be extracted, or `nullptr` if `aByteCount` is 0.
    //!
    //! \throws PacketExtractError if Packet has less than `aByteCount` bytes left
    //!         to extract. If this exception is thrown, the packet will also be
    //!         set to an invalid state.
    void* extractBytes(PZInteger aByteCount);

    //! \brief Extracts raw data from the packet; does not throw on failure.
    //! 
    //! Returns a pointer to a buffer of size N containing the next N bytes to be
    //! extracted from the Packet, where N is equal to `aByteCount`. These bytes
    //! can then be modified, copied, read directly or otherwise used. Clearing
    //! the Packet or appending more data to it will invalidate the returned pointer.
    //! In either case, this function will advance the Packet's internal read counter
    //! by `aByteCount`.
    //!
    //! \param aByteCount How many bytes to extract.
    //!
    //! \returns Pointer to data to be extracted, or `nullptr` if `aByteCount` is 0,
    //!          or if the Packet has less than `aByteCount` bytes left to extract
    //!          (in this case the Packet will alse be set to an invalid state).
    void* extractBytesNoThrow(PZInteger aByteCount);

    //! Compile-time checker for whether a type has a compatible extract operator>> from `hg::util::Packet`.
    template <class taType>
    class supports_extracting_of {
        template <class taType2>
        static auto test(int)
            -> decltype( std::declval<PacketExtender&>() >> std::declval<taType2&>(), std::true_type() );

        template <class>
        static auto test(...) -> std::false_type;

    public:
        static const bool value = decltype(test<taType>(0))::value;
    };

    //! TODO
    template <class T>
    auto operator>>(T& aData) -> std::enable_if_t<supports_extracting_of<T>::value, Packet&>;

    class NoThrowAdapter {
    public:
        template <class T,
                  T_ENABLE_IF(Packet::supports_appending_of<T&>::value)>
        NoThrowAdapter& operator<<(T& aRef) {
          _packet.append(aRef);
          return SELF;
        }

        template <class T,
                  T_ENABLE_IF(Packet::supports_extracting_of<T&>::value)>
        NoThrowAdapter& operator>>(T& aRef) {
          aRef = _packet.extractNoThrow<T>();
          return SELF;
        }

        //! A bool-like type that cannot be converted to integer or pointer types.
        using BoolType = void (NoThrowAdapter::*)();

        //! TODO
        operator BoolType() const {
          return (_packet) ? &NoThrowAdapter::_dummy : NULL;
        }

    private:
        friend class Packet;

        NoThrowAdapter(Packet& aPacket) : _packet{aPacket} {}

        void _dummy() {}

        Packet& _packet;
    };

    //! TODO
    NoThrowAdapter noThrow();

    ///////////////////////////////////////////////////////////////////////////
    // TESTING VALIDITY                                                      //
    ///////////////////////////////////////////////////////////////////////////

    //! A bool-like type that cannot be converted to integer or pointer types.
    using BoolType = bool (Packet::*)(std::size_t);

    //! \brief Test the validity of the packet, for reading
    //!
    //! This operator allows to test the packet as a boolean
    //! variable, to check if a reading operation was successful.
    //!
    //! A packet will be in an invalid state if it has no more
    //! data to read.
    //!
    //! This behavior is the same as standard C++ streams.
    //!
    //! Usage example:
    //! \code
    //! float x;
    //! packet >> x;
    //! if (packet)
    //! {
    //!    // ok, x was extracted successfully
    //! }
    //!
    //! // -- or --
    //!
    //! float x;
    //! if (packet >> x)
    //! {
    //!    // ok, x was extracted successfully
    //! }
    //! \endcode
    //!
    //! Don't focus on the return type, it's equivalent to bool but
    //! it disallows unwanted implicit conversions to integer or
    //! pointer types.
    //!
    //! \return True if last data extraction from packet was successful
    //!
    //! \see endOfPacket
    //!
    operator BoolType() const;

private:
    // Befriend appending operators
    friend Packet& operator<<(PacketExtender& aPacket, bool                 aData);
    friend Packet& operator<<(PacketExtender& aPacket, std::int8_t          aData);
    friend Packet& operator<<(PacketExtender& aPacket, std::uint8_t         aData);
    friend Packet& operator<<(PacketExtender& aPacket, std::int16_t         aData);
    friend Packet& operator<<(PacketExtender& aPacket, std::uint16_t        aData);
    friend Packet& operator<<(PacketExtender& aPacket, std::int32_t         aData);
    friend Packet& operator<<(PacketExtender& aPacket, std::uint32_t        aData);
    friend Packet& operator<<(PacketExtender& aPacket, std::int64_t         aData);
    friend Packet& operator<<(PacketExtender& aPacket, std::uint64_t        aData);
    friend Packet& operator<<(PacketExtender& aPacket, float                aData);
    friend Packet& operator<<(PacketExtender& aPacket, double               aData);
    friend Packet& operator<<(PacketExtender& aPacket, std::string_view     aData);
    friend Packet& operator<<(PacketExtender& aPacket, const UnicodeString& aData);
    friend Packet& operator<<(PacketExtender& aPacket, const Packet&        aData);

    // Befriend extracting operators
    friend Packet& operator>>(PacketExtender& aPacket, bool&                aData);
    friend Packet& operator>>(PacketExtender& aPacket, std::int8_t&         aData);
    friend Packet& operator>>(PacketExtender& aPacket, std::uint8_t&        aData);
    friend Packet& operator>>(PacketExtender& aPacket, std::int16_t&        aData);
    friend Packet& operator>>(PacketExtender& aPacket, std::uint16_t&       aData);
    friend Packet& operator>>(PacketExtender& aPacket, std::int32_t&        aData);
    friend Packet& operator>>(PacketExtender& aPacket, std::uint32_t&       aData);
    friend Packet& operator>>(PacketExtender& aPacket, std::int64_t&        aData);
    friend Packet& operator>>(PacketExtender& aPacket, std::uint64_t&       aData);
    friend Packet& operator>>(PacketExtender& aPacket, float&               aData);
    friend Packet& operator>>(PacketExtender& aPacket, double&              aData);
    friend Packet& operator>>(PacketExtender& aPacket, std::string&         aData);
    friend Packet& operator>>(PacketExtender& aPacket, UnicodeString&       aData);
    friend Packet& operator>>(PacketExtender& aPacket, Packet&              aData);

    //! Disallow comparisons between packets
    bool operator==(const Packet& right) const = delete;
    bool operator!=(const Packet& right) const = delete;

    //! \brief Check if the packet can extract a given number of bytes
    //!
    //! This function updates accordingly the state of the packet.
    //!
    //! \param size Size to check
    //!
    //! \return True if \a size bytes can be read from the packet
    bool _checkSize(std::size_t aSize);
    
    static void _logExtractionError(const char* aErrorMessage);

    std::vector<std::uint8_t> _buffer;         //!< Buffer of data stored in the packet
    std::size_t               _readPos = 0;    //!< Current reading position in the packet
    bool                      _isValid = true; //!< Reading state of the packet
};

// TODO (description)
class PacketExtender {
public:
    Packet& operator*() {
      return _packet;
    }

    Packet* operator->() {
      return &_packet;
    }

private:
    friend class Packet;

    PacketExtender(Packet& aPacket) : _packet{aPacket} {}

    Packet& _packet;
};

template <class ...taNoArgs,
          T_ENABLE_IF(sizeof...(taNoArgs) == 0)>
void PackArgs(Packet&) {}

template <class taArgsHead, class ...taArgsRest>
void PackArgs(Packet& aPacket, taArgsHead&& aArgsHead, taArgsRest&&... aArgsRest) {
    aPacket.append(std::forward<taArgsHead>(aArgsHead));
    PackArgs<taArgsRest...>(aPacket, std::forward<taArgsRest>(aArgsRest)...);
}

///////////////////////////////////////////////////////////////////////////
// TEMPLATE OPERATOR DEFINITIONS                                         //
///////////////////////////////////////////////////////////////////////////

template <class T>
auto Packet::operator<<(T&& aData)
    -> std::enable_if_t<supports_appending_of<T>::value, Packet&>
{
    PacketExtender extender{SELF};
    extender << std::forward<T>(aData);
    return SELF;
}

template <class T>
auto Packet::operator>>(T& aData)
    -> std::enable_if_t<supports_extracting_of<T>::value, Packet&>
{
  PacketExtender extender{SELF};
  extender >> aData;
  if (!SELF) {
    HG_THROW_TRACED(
        PacketExtractError,
        0,
        "Failed to extract data from hg::util::Packet (# of bytes remaining: {}).",
        getRemainingDataSize()
    );
  }
  return SELF;
}

///////////////////////////////////////////////////////////////////////////
// TEMPLATE METHOD DEFINITIONS                                           //
///////////////////////////////////////////////////////////////////////////

template <class T>
void Packet::append(T&& aData) {
    SELF << std::forward<T>(aData);
}

template <class T>
T Packet::extract() {
    T result;
    SELF >> result;
    return result;
}

template <class T>
T Packet::extractNoThrow() {
  try {
      T result;
      PacketExtender extender{SELF};
      extender >> result;
      return result;
  } catch (const std::exception& aEx) {
      _logExtractionError(aEx.what());
      _isValid = false;
      return {};
  }
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#endif // !UHOBGOBLIN_UTILITY_PACKET_HPP
