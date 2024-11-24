// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_COMPACT_VECTOR_HPP
#define UHOBGOBLIN_UTIL_COMPACT_VECTOR_HPP

#include <Hobgoblin/HGExcept.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <new>
#include <type_traits>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {
namespace detail {

class CompressedSmallVectorStorageBase {
protected:
    //! Stores a 32-bit size counter in bytes 0..3 of the pointed-to buffer.
    //! \warning aBufferBeginning must NOT be null (but this is not checked by the function)!
    static void _storeSizeInBuffer(void* aBufferBeginning, std::uint32_t aSize) {
        std::memcpy(aBufferBeginning, &aSize, sizeof(aSize));
    }

    //! Stores a 32-bit capacity counter in bytes 4..7 of the pointed-to buffer.
    //! \warning aBufferBeginning must NOT be null (but this is not checked by the function)!
    static void _storeCapacityInBuffer(void* aBufferBeginning, std::uint32_t aCapacity) {
        std::memcpy(static_cast<std::uint32_t*>(aBufferBeginning) + 1, &aCapacity, sizeof(aCapacity));
    }

    //! Loads a 32-bit size counter from bytes 0..3 of the pointed-to buffer.
    //! \warning aBufferBeginning must NOT be null (but this is not checked by the function)!
    static std::uint32_t _loadSizeFromBuffer(const void* aBufferBeginning) {
        std::uint32_t size;
        std::memcpy(&size, aBufferBeginning, sizeof(size));
        return size;
    }

    //! Loads a 32-bit capacity counter from bytes 4..7 of the pointed-to buffer.
    //! \warning aBufferBeginning must NOT be null (but this is not checked by the function)!
    static std::uint32_t _loadCapacityFromBuffer(const void* aBufferBeginning) {
        std::uint32_t capacity;
        std::memcpy(&capacity,
                    static_cast<const char*>(aBufferBeginning) + sizeof(capacity),
                    sizeof(capacity));
        return capacity;
    }

    //! Stores a 64-bit pointer in bytes 0..7 of the pointed-to buffer IN BIG-ENDIAN FORMAT.
    //! \warning aBuffer must NOT be null (but this is not checked by the function)!
    static void _storeHeapPointerToBuffer(std::uint8_t* aBuffer, void* aPointer) {
        static_assert(sizeof(void*) == 8, "Function supports only 64-bit pointers.");

        const auto ptrAsInt = reinterpret_cast<uint64_t>(aPointer);
        HG_ASSERT((ptrAsInt & 0x03ULL) == 0);

        // clang-format off
        aBuffer[7] = static_cast<std::uint8_t>((ptrAsInt >>  0) & 0xFF);
        aBuffer[6] = static_cast<std::uint8_t>((ptrAsInt >>  8) & 0xFF);
        aBuffer[5] = static_cast<std::uint8_t>((ptrAsInt >> 16) & 0xFF);
        aBuffer[4] = static_cast<std::uint8_t>((ptrAsInt >> 24) & 0xFF);
        aBuffer[3] = static_cast<std::uint8_t>((ptrAsInt >> 32) & 0xFF);
        aBuffer[2] = static_cast<std::uint8_t>((ptrAsInt >> 40) & 0xFF);
        aBuffer[1] = static_cast<std::uint8_t>((ptrAsInt >> 48) & 0xFF);
        aBuffer[0] = static_cast<std::uint8_t>((ptrAsInt >> 56) & 0xFF);
        // clang-format on
    }

    //! Loads a 64-bit pointer from bytes 0..7 of the pointed-to buffer (assumes that the
    //! pointer is stored IN BIG-ENDIAN FORMAT but it will be converted to host endianess).
    //! \warning aBuffer must NOT be null (but this is not checked by the function)!
    static void* _loadHeapPointerFromBuffer(const std::uint8_t* aBuffer) {
        static_assert(sizeof(void*) == 8, "Function supports only 64-bit pointers.");

        // clang-format off
        const auto ptrAsInt =
            ((static_cast<std::uint64_t>(aBuffer[7]) <<  0) & 0x00000000000000FF) |
            ((static_cast<std::uint64_t>(aBuffer[6]) <<  8) & 0x000000000000FF00) |
            ((static_cast<std::uint64_t>(aBuffer[5]) << 16) & 0x0000000000FF0000) |
            ((static_cast<std::uint64_t>(aBuffer[4]) << 24) & 0x00000000FF000000) |
            ((static_cast<std::uint64_t>(aBuffer[3]) << 32) & 0x000000FF00000000) |
            ((static_cast<std::uint64_t>(aBuffer[2]) << 40) & 0x0000FF0000000000) |
            ((static_cast<std::uint64_t>(aBuffer[1]) << 48) & 0x00FF000000000000) |
            ((static_cast<std::uint64_t>(aBuffer[0]) << 56) & 0xFF00000000000000) ;
        // clang-format on

        return reinterpret_cast<void*>(ptrAsInt & ~0x03ULL);
    }
};

inline std::uint32_t CalcSizeWithOffset(std::uint32_t aSize, std::int32_t aOffset) {
    return (aOffset > 0) ? (aSize + static_cast<std::uint32_t>(aOffset))
                         : (aSize - static_cast<std::uint32_t>(-aOffset));
}

#ifdef _MSC_VER
// https://stackoverflow.com/a/55530422
#define EMPTY_BASES __declspec(empty_bases)
#else
#define EMPTY_BASES
#endif

// MARK: Small Storage

template <class T, std::uint32_t taInPlaceCapacity, class taAllocator>
class EMPTY_BASES CompressedSmallVectorStorage_Small
    : private CompressedSmallVectorStorageBase
    , private taAllocator {
public:
    static_assert(sizeof(T) * taInPlaceCapacity < sizeof(void*),
                  "Invalid parameters for Small storage.");

//! Control byte of the instance.
//! Least significant bit stores the in-place flag (IN_PLACE_BIT), other bits store size.
#define CTRL _data.back()

    //! Main constructor.
    explicit CompressedSmallVectorStorage_Small(const taAllocator& aAlloc = taAllocator{})
        : taAllocator{aAlloc} {
        CTRL = IN_PLACE_BIT;
    }

    //! Copy constructor.
    CompressedSmallVectorStorage_Small(const CompressedSmallVectorStorage_Small& aOther)
        : taAllocator{static_cast<const taAllocator&>(aOther)} {
        CTRL = IN_PLACE_BIT;
        _copyAllElementsFrom(aOther);
    }

    //! Copy assignment operator.
    CompressedSmallVectorStorage_Small& operator=(const CompressedSmallVectorStorage_Small& aOther) {
        if (&aOther != this) {
            (*static_cast<taAllocator*>(this)) = static_cast<const taAllocator&>(aOther);
            clear();
            _copyAllElementsFrom(aOther);
        }
        return SELF;
    }

    //! Move constructor.
    CompressedSmallVectorStorage_Small(CompressedSmallVectorStorage_Small&& aOther) noexcept
        : taAllocator{static_cast<const taAllocator&>(aOther)} {
        CTRL = IN_PLACE_BIT;
        _moveAllElementsFrom(aOther);
    }

    //! Move assignment operator.
    CompressedSmallVectorStorage_Small& operator=(CompressedSmallVectorStorage_Small&& aOther) noexcept {
        if (&aOther != this) {
            (*static_cast<taAllocator*>(this)) = static_cast<const taAllocator&>(aOther);
            clear();
            _moveAllElementsFrom(aOther);
        }
        return SELF;
    }

    //! Destructor.
    ~CompressedSmallVectorStorage_Small() {
        clear();

        if (!_isInPlace()) {
            auto* ptr = _loadHeapPointer<T>();
            if (ptr != nullptr) {
                _freeHeapBuffer(ptr, getCapacity() + N_OBJECTS_FOR_SIZE_DATA);
            }
        }
    }

    void clear() {
        T*         elements = getAddressOfFirstElement();
        const auto size     = getSize();
        for (std::uint32_t i = 0; i < size; i += 1) {
            elements[i].~T();
        }

        if (_isInPlace()) {
            CTRL = IN_PLACE_BIT;
        } else {
            _storeSizeInBuffer(_loadHeapPointer(), 0);
        }
    }

    std::uint32_t getCapacity() const {
        if (_isInPlace()) {
            return taInPlaceCapacity;
        }
        auto* p = _loadHeapPointer();
        return _loadCapacityFromBuffer(p);
    }

    void setCapacity(std::uint32_t aNewCapacity) {
        const auto capacity = getCapacity();
        if (aNewCapacity > capacity) {
            _increaseCapacity(aNewCapacity);
        } else if (aNewCapacity < capacity) {
            _decreaseCapacity(aNewCapacity);
        }
    }

    std::uint32_t getSize() const {
        if (_isInPlace()) {
            return (CTRL >> 1);
        }
        auto* p = _loadHeapPointer();
        return _loadSizeFromBuffer(p);
    }

    void adjustSizeCounter(std::int32_t aOffset) {
        const auto size = getSize();
        HG_ASSERT((aOffset != 0) &&
                  ((aOffset > 0) ? (size + static_cast<std::uint32_t>(aOffset) <= getCapacity())
                                 : (size >= static_cast<std::uint32_t>(-aOffset))));
        const std::uint32_t newSize = CalcSizeWithOffset(size, aOffset);
        if (_isInPlace()) {
            _data.back() = static_cast<std::uint8_t>(newSize << 1) | IN_PLACE_BIT;
        } else {
            _storeSizeInBuffer(_loadHeapPointer(), newSize);
        }
    }

    T* getAddressOfFirstElement() {
        if (_isInPlace()) {
            return std::launder(reinterpret_cast<T*>(_data.data()));
        }
        return (_loadHeapPointer<T>() + N_OBJECTS_FOR_SIZE_DATA);
    }

    const T* getAddressOfFirstElement() const {
        return const_cast<CompressedSmallVectorStorage_Small*>(this)->getAddressOfFirstElement();
    }

private:
    //! Two 32-bit counters: size and capacity (stored on the heap).
    static constexpr std::size_t METADATA_SIZE = sizeof(std::uint32_t) * 2;

    //! Tells how many allocations of sizeof(T) are needed to provide at least 64 bits,
    //! to provide storage for metadata (since allocators work in T-sized chunks).
    static constexpr unsigned N_OBJECTS_FOR_SIZE_DATA = (METADATA_SIZE + sizeof(T) - 1) / sizeof(T);

    //! Small storage always has the size of one pointer.
    static constexpr std::size_t BUFFER_SIZE = sizeof(void*);

    static constexpr std::uint8_t IN_PLACE_BIT = 0x01;

    // STORAGE SCHEME:
    // - In place:
    //   - byte CTRL (_data.back()) stores IN_PLACE_BIT (lowest bit), and size (other bits).
    //   - other bytes of _data provide storage for T.
    //
    // - On heap:
    //   - bytes 0..7 (0..3 on 32bit platforms) of _data store pointer to the buffer (which is
    //     on the heap) with the least significant bit of the pointer being used to store
    //     IN_PLACE_BIT (the memory we get back from malloc and company is always aligned to at
    //     least 4 bytes, so the lowest two bits should always be free for our use). The pointer is
    //     stored in BIG ENDIAN format, thus it is always possible to check the status of the
    //     container (in-place or not) by inspecting _data[7]. The first 8 bytes of the heap buffer
    //     are used to store size (bytes 0..3) and capacity (bytes 4..7) of the container; bytes
    //     after that store the elements.
    alignas(T) std::array<std::uint8_t, BUFFER_SIZE> _data = {};

    bool _isInPlace() const {
        return CTRL & IN_PLACE_BIT;
    }

    template <class U = void>
    U* _loadHeapPointer() {
        return static_cast<U*>(_loadHeapPointerFromBuffer(_data.data()));
    }

    template <class U = void>
    const U* _loadHeapPointer() const {
        return static_cast<const U*>(_loadHeapPointerFromBuffer(_data.data()));
    }

    void _storeHeapPointer(void* aPointer) {
        _storeHeapPointerToBuffer(_data.data(), aPointer);
    }

    void _increaseCapacity(std::uint32_t aNewCapacity) {
        HG_ASSERT(aNewCapacity > getCapacity());

        T* heapBuffer = nullptr;

        if (_isInPlace()) {
            if (aNewCapacity <= taInPlaceCapacity) {
                return;
            }

            heapBuffer = _allocateHeapBuffer(aNewCapacity + N_OBJECTS_FOR_SIZE_DATA);

            const auto size     = getSize();
            auto*      elements = getAddressOfFirstElement();
            for (std::uint32_t i = 0; i < size; i += 1) {
                new (static_cast<void*>(heapBuffer + N_OBJECTS_FOR_SIZE_DATA + i))
                    T{std::move(elements[i])};
                elements[i].~T();
            }

            _storeSizeInBuffer(heapBuffer, size);
        } else {
            heapBuffer = _allocateHeapBuffer(aNewCapacity + N_OBJECTS_FOR_SIZE_DATA);

            auto*      elements = getAddressOfFirstElement();
            const auto size     = getSize();
            const auto capacity = getCapacity();
            for (std::uint32_t i = 0; i < size; i += 1) {
                new (static_cast<void*>(heapBuffer + N_OBJECTS_FOR_SIZE_DATA + i))
                    T{std::move(elements[i])};
                elements[i].~T();
            }

            _storeSizeInBuffer(heapBuffer, size);
            _freeHeapBuffer(_loadHeapPointer<T>(), capacity + N_OBJECTS_FOR_SIZE_DATA);
        }

        _storeCapacityInBuffer(heapBuffer, aNewCapacity);
        _storeHeapPointer(heapBuffer);
    }

    void _decreaseCapacity(std::uint32_t aNewCapacity) {
        HG_ASSERT(aNewCapacity < getCapacity());
        HG_ASSERT(aNewCapacity >= getSize());

        if (_isInPlace()) {
            return; // Cannot reduce further
        }

        if (aNewCapacity <= taInPlaceCapacity) {
            const auto  originalCapacity = getCapacity();
            const auto  size             = getSize();
            auto* const heapBuffer       = _loadHeapPointer<T>();

            CTRL = static_cast<std::uint8_t>(size << 1) | 0x01;

            auto* const elements = getAddressOfFirstElement();
            for (std::uint32_t i = 0; i < size; i += 1) {
                new (static_cast<void*>(elements + i))
                    T{std::move(heapBuffer[i + N_OBJECTS_FOR_SIZE_DATA])};
                heapBuffer[i + N_OBJECTS_FOR_SIZE_DATA].~T();
            }

            _freeHeapBuffer(heapBuffer, originalCapacity + N_OBJECTS_FOR_SIZE_DATA);
        } else {
            const auto  originalCapacity = getCapacity();
            const auto  size             = getSize();
            auto* const heapBuffer       = _loadHeapPointer<T>();

            auto* const newHeapBuffer = _allocateHeapBuffer(aNewCapacity);

            _storeSizeInBuffer(newHeapBuffer, size);
            _storeCapacityInBuffer(newHeapBuffer, aNewCapacity + N_OBJECTS_FOR_SIZE_DATA);

            for (std::uint32_t i = 0; i < size; i += 1) {
                new (static_cast<void*>(newHeapBuffer + i + N_OBJECTS_FOR_SIZE_DATA))
                    T{std::move(heapBuffer[i + N_OBJECTS_FOR_SIZE_DATA])};
                heapBuffer[i + N_OBJECTS_FOR_SIZE_DATA].~T();
            }

            _storeHeapPointer(newHeapBuffer);
            _freeHeapBuffer(heapBuffer, originalCapacity + N_OBJECTS_FOR_SIZE_DATA);
        }
    }

    T* _allocateHeapBuffer(std::uint32_t aTCount) {
        auto* p = this->allocate(aTCount);
        HG_HARD_ASSERT(p != nullptr && ((reinterpret_cast<std::uintptr_t>(p) & 0x3ULL) == 0) &&
                       "Allocator returned an address that is not at least 4-byte aligned");
        return p;
    }

    void _freeHeapBuffer(T* aBuffer, std::uint32_t aTCount) {
        this->deallocate(aBuffer, aTCount);
    }

    void _copyAllElementsFrom(const CompressedSmallVectorStorage_Small& aSource) {
        const auto sourceSize = aSource.getSize();
        setCapacity(sourceSize);

        const T* sourceElements = aSource.getAddressOfFirstElement();
        T*       elements       = getAddressOfFirstElement();
        for (std::uint32_t i = 0; i < sourceSize; i += 1) {
            new (static_cast<void*>(elements + i)) T{sourceElements[i]};
        }

        if (_isInPlace()) {
            CTRL = static_cast<std::uint8_t>(sourceSize << 1) | IN_PLACE_BIT;
        } else {
            _storeSizeInBuffer(_loadHeapPointer(), sourceSize);
        }
    }

    void _moveAllElementsFrom(CompressedSmallVectorStorage_Small& aSource) {
        if (aSource._isInPlace()) {
            const auto sourceSize     = aSource.getSize();
            const T*   sourceElements = aSource.getAddressOfFirstElement();
            T*         elements       = getAddressOfFirstElement();
            for (std::uint32_t i = 0; i < sourceSize; i += 1) {
                new (static_cast<void*>(elements + i)) T{std::move(sourceElements[i])};
                sourceElements[i].~T();
            }
            CTRL = static_cast<std::uint8_t>(sourceSize << 1) | IN_PLACE_BIT;
        } else {
            std::memcpy(_data.data(), aSource._data.data(), _data.size());
        }

        // Mark aSource as empty - either the pointer was taken over, or the in-place
        // objects were moved from and destroyed.
        aSource.CTRL = IN_PLACE_BIT;
    }

#undef CTRL
};

// MARK: Large Storage

template <class T, std::uint32_t taInPlaceCapacity, class taAllocator>
class EMPTY_BASES CompressedSmallVectorStorage_Large
    : private CompressedSmallVectorStorageBase
    , private taAllocator {
public:
    static_assert(sizeof(T) * taInPlaceCapacity >= sizeof(void*),
                  "Invalid parameters for Large storage.");

//! Control byte of the instance.
//! Least significant bit stores the in-place flag (IN_PLACE_BIT), other bits store size.
#define CTRL _data.back()

    //! Main constructor.
    explicit CompressedSmallVectorStorage_Large(const taAllocator& aAlloc = taAllocator{})
        : taAllocator{aAlloc} {
        CTRL = IN_PLACE_BIT;
    }

    //! Copy constructor.
    CompressedSmallVectorStorage_Large(const CompressedSmallVectorStorage_Large& aOther)
        : taAllocator{static_cast<const taAllocator&>(aOther)} {
        CTRL = IN_PLACE_BIT;
        _copyAllElementsFrom(aOther);
    }

    //! Copy assignment operator.
    CompressedSmallVectorStorage_Large& operator=(const CompressedSmallVectorStorage_Large& aOther) {
        if (&aOther != this) {
            (*static_cast<taAllocator*>(this)) = static_cast<const taAllocator&>(aOther);
            clear();
            _copyAllElementsFrom(aOther);
        }
        return SELF;
    }

    //! Move constructor.
    CompressedSmallVectorStorage_Large(CompressedSmallVectorStorage_Large&& aOther) noexcept
        : taAllocator{static_cast<const taAllocator&>(aOther)} {
        CTRL = IN_PLACE_BIT;
        _moveAllElementsFrom(aOther);
    }

    //! Move assignment operator.
    CompressedSmallVectorStorage_Large& operator=(CompressedSmallVectorStorage_Large&& aOther) noexcept {
        if (&aOther != this) {
            (*static_cast<taAllocator*>(this)) = static_cast<const taAllocator&>(aOther);
            clear();
            _moveAllElementsFrom(aOther);
        }
        return SELF;
    }

    //! Destructor.
    ~CompressedSmallVectorStorage_Large() {
        clear();

        if (!_isInPlace()) {
            auto* ptr = _loadHeapPointer<T>();
            if (ptr != nullptr) {
                _freeHeapBuffer(ptr, getCapacity());
            }
        }
    }

    std::uint32_t getCapacity() const {
        if (_isInPlace()) {
            return taInPlaceCapacity;
        }
        return _loadCapacityFromBuffer(_data.data());
    }

    void setCapacity(std::uint32_t aNewCapacity) {
        const auto capacity = getCapacity();
        if (aNewCapacity > capacity) {
            _increaseCapacity(aNewCapacity);
        } else if (aNewCapacity < capacity) {
            _decreaseCapacity(aNewCapacity);
        }
    }

    std::uint32_t getSize() const {
        if (_isInPlace()) {
            return static_cast<std::uint32_t>(CTRL >> 1);
        }
        return _loadSizeFromBuffer(_data.data());
    }

    void adjustSizeCounter(std::int32_t aOffset) {
        const auto size = getSize();
        HG_ASSERT((aOffset != 0) &&
                  ((aOffset > 0) ? (size + static_cast<std::uint32_t>(aOffset) <= getCapacity())
                                 : (size >= static_cast<std::uint32_t>(-aOffset))));
        const std::uint32_t newSize = CalcSizeWithOffset(size, aOffset);
        if (_isInPlace()) {
            CTRL = static_cast<std::uint8_t>(newSize << 1) | IN_PLACE_BIT;
        } else {
            _storeSizeInBuffer(_data.data(), newSize);
        }
    }

    T* getAddressOfFirstElement() {
        if (_isInPlace()) {
            return std::launder(reinterpret_cast<T*>(_data.data()));
        }
        return _loadHeapPointer<T>();
    }

    const T* getAddressOfFirstElement() const {
        return const_cast<CompressedSmallVectorStorage_Large*>(this)->getAddressOfFirstElement();
    }

    void clear() {
        T*         elements = getAddressOfFirstElement();
        const auto size     = getSize();
        for (std::uint32_t i = 0; i < size; i += 1) {
            elements[i].~T();
        }

        if (_isInPlace()) {
            CTRL = IN_PLACE_BIT;
        } else {
            _storeSizeInBuffer(_data.data(), 0);
        }
    }

private:
    //! Two 32-bit counters: size and capacity (stored in-place when elements are on the heap).
    static constexpr std::size_t METADATA_SIZE = sizeof(std::uint32_t) * 2;

    static constexpr std::size_t BUFFER_SIZE =
        std::max(METADATA_SIZE + sizeof(void*), sizeof(T) * taInPlaceCapacity + 1);

    static constexpr std::uint8_t IN_PLACE_BIT = 0x01;

    // STORAGE SCHEME:
    // - In place:
    //   - byte CTRL (_data.back()) stores IN_PLACE_BIT (lowest bit), and size (other bits).
    //   - other bytes of _data provide storage for T.
    //
    // - On heap:
    //   - bytes 0..3 and 4..7 of _data store the size and capacity counters, respectively.
    //     bytes 8..15 store the pointer to the dynamically allocated buffer where the elements
    //     are stored. This pointer is stored in BIG ENDIAN format, and if BUFFER_SIZE is exactly
    //     16, the last byte of the pointer will double as the CTRL byte. Thus it is always
    //     possible to check the status of the container (in-place or not) by inspecting
    //     _data.back(). (Note: also check storage scheme of the small storage class.)
    alignas(T) std::array<std::uint8_t, BUFFER_SIZE> _data = {};

    bool _isInPlace() const {
        return _data.back() & IN_PLACE_BIT;
    }

    template <class U = void>
    U* _loadHeapPointer() {
        return static_cast<U*>(_loadHeapPointerFromBuffer(_data.data() + METADATA_SIZE));
    }

    template <class U = void>
    const U* _loadHeapPointer() const {
        return static_cast<const U*>(_loadHeapPointerFromBuffer(_data.data() + METADATA_SIZE));
    }

    void _storeHeapPointer(void* aPointer) {
        _storeHeapPointerToBuffer(_data.data() + METADATA_SIZE, aPointer);
    }

    T* _allocateHeapBuffer(std::uint32_t aTCount) {
        auto* p = this->allocate(aTCount);
        HG_HARD_ASSERT(p != nullptr && ((reinterpret_cast<std::uintptr_t>(p) & 0x3ULL) == 0) &&
                       "Allocator returned an address that is not at least 4-byte aligned");
        return p;
    }

    void _freeHeapBuffer(T* aBuffer, std::uint32_t aTCount) {
        this->deallocate(aBuffer, aTCount);
    }

    void _increaseCapacity(std::uint32_t aNewCapacity) {
        HG_ASSERT(aNewCapacity > getCapacity());

        T* heapBuffer = nullptr;

        if (_isInPlace()) {
            if (aNewCapacity <= taInPlaceCapacity) {
                return;
            }

            heapBuffer = _allocateHeapBuffer(aNewCapacity);

            const auto size     = getSize();
            auto*      elements = getAddressOfFirstElement();
            for (std::uint32_t i = 0; i < size; i += 1) {
                new (static_cast<void*>(heapBuffer + i)) T{std::move(elements[i])};
                elements[i].~T();
            }

            _storeSizeInBuffer(_data.data(), size);
        } else {
            heapBuffer = _allocateHeapBuffer(aNewCapacity);

            auto*      elements = getAddressOfFirstElement();
            const auto size     = getSize();
            const auto capacity = getCapacity();
            for (std::uint32_t i = 0; i < size; i += 1) {
                new (static_cast<void*>(heapBuffer + i)) T{std::move(elements[i])};
                elements[i].~T();
            }

            _storeSizeInBuffer(_data.data(), size);
            _freeHeapBuffer(_loadHeapPointer<T>(), capacity);
        }

        _storeCapacityInBuffer(_data.data(), aNewCapacity);
        _storeHeapPointer(heapBuffer);
        CTRL &= static_cast<std::uint8_t>(~0x01);
    }

    void _decreaseCapacity(std::uint32_t aNewCapacity) {
        HG_ASSERT(aNewCapacity < getCapacity());
        HG_ASSERT(aNewCapacity >= getSize());

        if (_isInPlace()) {
            return; // Cannot reduce further
        }

        if (aNewCapacity <= taInPlaceCapacity) {
            const auto  originalCapacity = getCapacity();
            const auto  size             = getSize();
            auto* const heapBuffer       = _loadHeapPointer<T>();

            CTRL = static_cast<std::uint8_t>(size << 1) | 0x01;

            auto* const elements = getAddressOfFirstElement();
            for (std::uint32_t i = 0; i < size; i += 1) {
                new (static_cast<void*>(elements + i)) T{std::move(heapBuffer[i])};
                heapBuffer[i].~T();
            }

            _freeHeapBuffer(heapBuffer, originalCapacity);
        } else {
            const auto  originalCapacity = getCapacity();
            const auto  size             = getSize();
            auto* const heapBuffer       = _loadHeapPointer<T>();

            auto* const newHeapBuffer = _allocateHeapBuffer(aNewCapacity);

            for (std::uint32_t i = 0; i < size; i += 1) {
                new (static_cast<void*>(newHeapBuffer + i)) T{std::move(heapBuffer[i])};
                heapBuffer[i].~T();
            }

            _storeHeapPointer(newHeapBuffer);
            _freeHeapBuffer(heapBuffer, originalCapacity);
        }
    }

    void _copyAllElementsFrom(const CompressedSmallVectorStorage_Large& aSource) {
        const auto sourceSize = aSource.getSize();
        setCapacity(sourceSize);

        const T* sourceElements = aSource.getAddressOfFirstElement();
        T*       elements       = getAddressOfFirstElement();
        for (std::uint32_t i = 0; i < sourceSize; i++) {
            new (static_cast<void*>(elements + i)) T{sourceElements[i]};
        }

        if (_isInPlace()) {
            CTRL = static_cast<std::uint8_t>(sourceSize << 1) | IN_PLACE_BIT;
        } else {
            _storeSizeInBuffer(_data.data(), sourceSize);
        }
    }

    void _moveAllElementsFrom(CompressedSmallVectorStorage_Large& aSource) {
        if (aSource._isInPlace()) {
            const auto sourceSize     = aSource.getSize();
            const T*   sourceElements = aSource.getAddressOfFirstElement();
            T*         elements       = getAddressOfFirstElement();
            for (std::uint32_t i = 0; i < sourceSize; i++) {
                new (static_cast<void*>(elements + i)) T{std::move(sourceElements[i])};
                sourceElements[i].~T();
            }

            CTRL = static_cast<std::uint8_t>(sourceSize << 1) | IN_PLACE_BIT;
        } else {
            std::memcpy(_data.data(), aSource._data.data(), _data.size());
        }

        // Mark aSource as empty - either the pointer was taken over, or the in-place
        // objects were moved from and destroyed.
        aSource.CTRL = IN_PLACE_BIT;
    }

#undef CTRL
};

#undef EMPTY_BASES

// MARK: Storage Selector

enum class StorageClassId {
    SMALL, // Corresponds to CompressedSmallVectorStorage_Small
    LARGE  // Corresponds to CompressedSmallVectorStorage_Large
};

template <class T, std::uint32_t taInPlaceCapacity>
constexpr StorageClassId SelectStorageClass() {
    // \/ because CTRL bytes have 7 bits free to hold the size \/
    static_assert(taInPlaceCapacity <= 127,
                  "CompressedSmallVector cannot hold more than 127 elements in-place.");
    return ((sizeof(T) * taInPlaceCapacity < sizeof(void*)) ? StorageClassId::SMALL
                                                            : StorageClassId::LARGE);
}

template <class T,
          std::uint32_t taInPlaceCapacity,
          class taAllocator,
          StorageClassId taStorageClassId = SelectStorageClass<T, taInPlaceCapacity>()>
class CompressedSmallVectorStorageSelector {};

template <typename T, std::uint32_t taInPlaceCapacity, class taAllocator>
class CompressedSmallVectorStorageSelector<T, taInPlaceCapacity, taAllocator, StorageClassId::SMALL> {
public:
    using type = CompressedSmallVectorStorage_Small<T, taInPlaceCapacity, taAllocator>;
};

template <typename T, std::uint32_t taInPlaceCapacity, class taAllocator>
class CompressedSmallVectorStorageSelector<T, taInPlaceCapacity, taAllocator, StorageClassId::LARGE> {
public:
    using type = CompressedSmallVectorStorage_Large<T, taInPlaceCapacity, taAllocator>;
};

} // namespace detail

// MARK: Growth Strategy

namespace growth_strategy {
class IncreaseByOne {
public:
    std::uint32_t operator()(std::uint32_t aCurrentCapacity) const {
        return aCurrentCapacity + 1;
    }
};

template <double taPercent>
class IncreaseByNPercent {
public:
    std::uint32_t operator()(std::uint32_t aCurrentCapacity) const {
        if (((aCurrentCapacity & 0x01) == 1) || aCurrentCapacity == 0) {
            aCurrentCapacity += 1;
        }
        return static_cast<std::uint32_t>(aCurrentCapacity * (1.0 + taPercent));
    }
};

using IncreaseByFiftyPercent = IncreaseByNPercent<0.5>;

using IncreaseByHundredPercent = IncreaseByNPercent<1.0>;

// Golden ratio = 1.618033988749...
using IncreaseByGoldenRatio = IncreaseByNPercent<0.618033988749>;

using Default = IncreaseByGoldenRatio;
} // namespace growth_strategy

// MARK: Vector

//! CompressedSmallVector is a sequential random-access container (similar to `std::vector`) that
//! is memory-optimized in several ways.
//!
//! \tparam T type of the stored element. Must have non-throwing move constructor and move
//!           assignment operator.
//! \tparam taInPlaceCapacity maximum number of elements that can be stored inside of the vector's
//!                           own space (i.e., without having to use dynamic allocations). This
//!                           value can be between 0 and 127 (inclusive). Defaults to 1.
//! \tparam taGrowthStrategy strategy for increasing the capacity of the vector. Defaults to
//!                          `IncreaseByGoldenRatio` to provide good amortized time of inserting
//!                          elements. Note: this parameter only applies to appending and
//!                          inserting. `reserve()` and `resize()` will use exact values provided.
//! \tparam taAllocator standard-compliant allocator type. Defaults to `std::allocator<T>`, which
//!                     is stateless and just calls malloc/free.
//!
//! The size of an instance of this class varies depending on the template parameters:
//! -      if `sizeof(T) * taInPlaceCapacity < 8`, then the instance will take up 8 bytes.
//!   Basically, it is possible to store 7 chars, 3 shorts or 1 int in-place. When in-place storage
//!   is not possible due to the number of elements, the whole 8 bytes will be used to store a
//!   pointer to a dynamically allocated buffer that holds size and capacity of the vector, and
//!   then the elements.
//! - else if `sizeof(T) * taInPlaceCapacity < 16`, then the instance will take up 16 bytes. This
//!   case is similar to the previous one, except that when the vector needs to allocate memory
//!   dynamically, there is now enough space in-place to store the size and capacity.
//! - else if `sizeof(T) * taInPlaceCapacity >= 16`, then the instance will take up as many bytes
//!   as is needed to store `taInPlaceCapacity` instances of `T` in-place, plus 1 byte (and
//!   remember that there could also be padding).
//!
//! `CompresedSmallVector` achieves this by aliasing space for `T`s and for metadata about
//! dynamically allocated buffers and holding only one of those at a time in its storage (similar
//! to `std::variant`). It also uses only 32-bit counters for size and capacity (31 bit usable).
//!
//! Values 8 and 16 used above are multiples of the size of a pointer. This class has only ever
//! been tested on ILP64.
//!
//! If a stateful allocator is used, the size of an instance of the vector is increased by the
//! size of the allocator.
//!
//! In all cases, the provided API is a subset of the API provided by `std::vector`.
//! However, the maximum supported size and capacity of `CompressedSmallVector` are fixed at
//! 2,147,483,647 elements.
template <class T,
          std::uint32_t taInPlaceCapacity = 1,
          class taGrowthStrategy          = growth_strategy::IncreaseByGoldenRatio,
          class taAllocator               = std::allocator<T>>
class CompressedSmallVector {
public:
    using value_type             = T;
    using allocator_type         = taAllocator;
    using iterator               = value_type*;
    using const_iterator         = const value_type*;
    using reverse_iterator       = typename std::reverse_iterator<iterator>;
    using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;
    using size_type              = std::size_t;

    static_assert(
        std::is_nothrow_move_assignable_v<T> && std::is_nothrow_move_constructible_v<T>,
        "CompressedSmallVector only works with types that are exception-safe in move operations.");

    //! Note: constructor not marked explicit because it's also not explicit for `std::vector`.
    CompressedSmallVector(const taAllocator& aAllocator = taAllocator{})
        : _storage{aAllocator} {}

    explicit CompressedSmallVector(size_type aCount, const taAllocator& aAllocator = taAllocator())
        : _storage{aAllocator} {
        resize(aCount);
    }

    CompressedSmallVector(size_type          aCount,
                          const T&           aValue,
                          const taAllocator& aAllocator = taAllocator())
        : _storage{aAllocator} {
        reserve(aCount);
        for (size_type i = 0; i < aCount; i += 1) {
            push_back(aValue);
        }
    }

    size_type size() const {
        return _storage.getSize();
    }

    size_type capacity() const {
        return _storage.getCapacity();
    }

    //! Reserve exact capacity, disregarding `taGrowthStrategy`.
    void reserve(size_type aDesiredCapacity) {
        const auto desiredCapacity = static_cast<std::uint32_t>(aDesiredCapacity);
        if (desiredCapacity > _storage.getCapacity()) {
            _storage.setCapacity(desiredCapacity);
        }
    }

    //! Resize to exact size, disregarding `taGrowthStrategy`.
    void resize(size_type aNewSize) {
        const auto originalSize = size();
        if (aNewSize < originalSize) {
            const auto diff = originalSize - aNewSize;
            for (std::size_t i = 1; i <= diff; i += 1) {
                auto* addr = _storage.getAddressOfFirstElement() + originalSize - i;
                addr->~T();
            }
            _storage.adjustSizeCounter(-static_cast<std::int32_t>(diff));
        } else if (aNewSize > originalSize) {
            reserve(static_cast<std::uint32_t>(aNewSize));
            const auto diff = aNewSize - originalSize;
            for (std::size_t i = 0; i < diff; i += 1) {
                auto* addr = _storage.getAddressOfFirstElement() + originalSize + i;
                new (addr) T{};
            }
            _storage.adjustSizeCounter(+static_cast<std::int32_t>(diff));
        }
    }

    void clear() {
        _storage.clear();
    }

    void push_back(const T& aObject) {
        const auto originalSize = _storage.getSize();
        if (originalSize == _storage.getCapacity()) {
            const auto newCapacity = _growCapacity(_storage.getCapacity());
            _storage.setCapacity(newCapacity);
        }
        auto* addr = _storage.getAddressOfFirstElement() + originalSize;
        new (addr) T{aObject};
        _storage.adjustSizeCounter(+1);
    }

    void push_back(T&& aObject) {
        const auto originalSize = _storage.getSize();
        if (originalSize == _storage.getCapacity()) {
            const auto newCapacity = _growCapacity(_storage.getCapacity());
            _storage.setCapacity(newCapacity);
        }
        auto* addr = _storage.getAddressOfFirstElement() + originalSize;
        new (addr) T{std::move(aObject)};
        _storage.adjustSizeCounter(+1);
    }

    template <class... taArgs>
    decltype(auto) emplace_back(taArgs&&... aArgs) {
        const auto originalSize = _storage.getSize();
        if (originalSize == _storage.getCapacity()) {
            const auto newCapacity = _growCapacity(_storage.getCapacity());
            _storage.setCapacity(newCapacity);
        }
        auto* addr = _storage.getAddressOfFirstElement() + originalSize;
        new (addr) T{std::forward<taArgs>(aArgs)...};
        _storage.adjustSizeCounter(+1);
        return back();
    }

    void pop_back() {
        HG_ASSERT(size() > 0);
        auto* addr = _storage.getAddressOfFirstElement() + _storage.getSize() - 1;
        addr->~T();
        _storage.adjustSizeCounter(-1);
    }

    void erase(iterator aIterator) {
        HG_ASSERT(aIterator >= begin());
        const auto index = aIterator - begin();
        HG_ASSERT(index < size());

        auto*      elements     = _storage.getAddressOfFirstElement();
        const auto originalSize = size();
        for (std::size_t i = index; i < originalSize - 1; i += 1) {
            elements[i] = std::move(elements[i + 1]);
        }
        pop_back();
    }

    void shrink_to_fit() {
        _storage.setCapacity(_storage.getSize());
    }

    T& operator[](size_type aIndex) {
        return _storage.getAddressOfFirstElement()[aIndex];
    }

    const T& operator[](size_type aIndex) const {
        return _storage.getAddressOfFirstElement()[aIndex];
    }

    T& at(size_type aIndex) {
        HG_VALIDATE_ARGUMENT(aIndex < size());
        return _storage.getAddressOfFirstElement()[aIndex];
    }

    const T& at(size_type aIndex) const {
        HG_VALIDATE_ARGUMENT(aIndex < size());
        return _storage.getAddressOfFirstElement()[aIndex];
    }

    bool empty() const {
        return size() == 0;
    }

    iterator begin() {
        return _storage.getAddressOfFirstElement();
    }

    const_iterator begin() const {
        return _storage.getAddressOfFirstElement();
    }

    const_iterator cbegin() const {
        return _storage.getAddressOfFirstElement();
    }

    iterator end() {
        return _storage.getAddressOfFirstElement() + size();
    }

    const_iterator end() const {
        return _storage.getAddressOfFirstElement() + size();
    }

    const_iterator cend() const {
        return _storage.getAddressOfFirstElement() + size();
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(begin());
    }

    value_type* data() {
        return _storage.getAddressOfFirstElement();
    }

    const value_type* data() const {
        return _storage.getAddressOfFirstElement();
    }

    value_type& front() {
        HG_ASSERT(size() > 0);
        return *_storage.getAddressOfFirstElement();
    }

    const value_type& front() const {
        HG_ASSERT(size() > 0);
        return _storage.getAddressOfFirstElement();
    }

    value_type& back() {
        HG_ASSERT(size() > 0);
        return *(_storage.getAddressOfFirstElement() + size() - 1);
    }

    const value_type& back() const {
        HG_ASSERT(size() > 0);
        return *(_storage.getAddressOfFirstElement() + size() - 1);
    }

private:
    using Storage =
        typename detail::CompressedSmallVectorStorageSelector<T, taInPlaceCapacity, taAllocator>::type;

    Storage _storage;

    static std::uint32_t _growCapacity(std::uint32_t aCurrentCapacity) {
        const auto newCapacity = static_cast<std::uint32_t>(taGrowthStrategy{}(aCurrentCapacity));
        HG_ASSERT(newCapacity > aCurrentCapacity);
        return newCapacity;
    }
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_COMPACT_VECTOR_HPP
