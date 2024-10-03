// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_COMPACT_VECTOR_HPP
#define UHOBGOBLIN_UTIL_COMPACT_VECTOR_HPP

#include <Hobgoblin/HGExcept.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {
namespace detail {

inline std::uint32_t CalcSizeWithOffset(std::uint32_t aSize, std::int32_t aOffset) {
    return (aOffset > 0) ? (aSize + static_cast<std::uint32_t>(aOffset))
                         : (aSize - static_cast<std::uint32_t>(-aOffset));
}

// MARK: Endianess

#ifdef __ORDER_LITTLE_ENDIAN__
#define HG_LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#else
#define HG_LITTLE_ENDIAN 1234
#endif

#ifdef __ORDER_BIG_ENDIAN__
#define HG_BIG_ENDIAN __ORDER_BIG_ENDIAN__
#else
#define HG_BIG_ENDIAN 4321
#endif

#ifdef __BYTE_ORDER__
#define HG_ENDIANESS __BYTE_ORDER__
#else
#define HG_ENDIANESS HG_LITTLE_ENDIAN
#endif

template <class T>
T Flip64(T aValue) {
    static_assert(sizeof(T) == 8, "Flip64 only works with 8-byte values.");
    static_assert(std::is_integral_v<T>, "Flip64 only works with integral types.");

    char bytes[8];
    std::memcpy(&bytes, &aValue, 8);

    // clang-format off
    const auto rv = ((static_cast<T>(bytes[7]) <<  0) & 0x00000000000000FF)
                  | ((static_cast<T>(bytes[6]) <<  8) & 0x000000000000FF00)
                  | ((static_cast<T>(bytes[5]) << 16) & 0x0000000000FF0000)
                  | ((static_cast<T>(bytes[4]) << 24) & 0x00000000FF000000)
                  | ((static_cast<T>(bytes[3]) << 32) & 0x000000FF00000000)
                  | ((static_cast<T>(bytes[2]) << 40) & 0x0000FF0000000000)
                  | ((static_cast<T>(bytes[1]) << 48) & 0x00FF000000000000)
                  | ((static_cast<T>(bytes[0]) << 56) & 0xFF00000000000000);
    // clang-format on

    return rv;
}

template <class T>
T HostToBigEndian64(T aValue) {
    static_assert(sizeof(T) == 8, "HostToBigEndian64 only works with 8-byte values.");
    static_assert(std::is_integral_v<T>, "HostToBigEndian64 only works with integral types.");

#if HG_ENDIANESS == HG_LITTLE_ENDIAN
    return Flip64(aValue);
#else
    return aValue;
#endif
}

template <class T>
T BigEndianToHost64(T aValue) {
    static_assert(sizeof(T) == 8, "BigEndianToHost64 only works with 8-byte values.");
    static_assert(std::is_integral_v<T>, "BigEndianToHost64 only works with integral types.");

#if HG_ENDIANESS == HG_LITTLE_ENDIAN
    return Flip64(aValue);
#else
    return aValue;
#endif
}

// MARK: ?

template <class T, bool taIsEmpty = std::is_empty<T>::value>
class StorePointerIfNotEmpty {};

template <class T>
class StorePointerIfNotEmpty<T, true> { // Empty
public:
    StorePointerIfNotEmpty() = default;
    StorePointerIfNotEmpty(T* /*aPointer*/) {}

    T* getPointer() const {
        return &_dummyObject;
    }

private:
    static T _dummyObject;
};

template <class T>
T StorePointerIfNotEmpty<T, true>::_dummyObject = {};

template <class T>
class StorePointerIfNotEmpty<T, false> { // Non-empty
public:
    StorePointerIfNotEmpty()
        : ptr{nullptr} {}
    StorePointerIfNotEmpty(T* aPointer)
        : ptr{aPointer} {};

    T* getPointer() const {
        return ptr;
    }

private:
    T* ptr;
};

// MARK: Storage 1

template <class T, class taAllocator>
class CompressedSmallVectorStorage_1 {
public:
    static_assert(sizeof(T) < sizeof(void*), "");

    CompressedSmallVectorStorage_1(const taAllocator* aAllocator = nullptr)
        : _data{aAllocator} {
        _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
    }

    ~CompressedSmallVectorStorage_1() {
        const auto size = getSize();
        if (size > 0) {
            _destroyAllElements(getAddressOfFirstElement(), size);
        }
        if (!_isInPlace()) {
            auto* p = _loadHeapPointer();
            _freeBuffer((T*)p);
        }
    }

    CompressedSmallVectorStorage_1(const CompressedSmallVectorStorage_1& aOther) {
        if (aOther._isInPlace()) {
            if (aOther._getSizeBit()) {
                _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT | SIZE_BIT;
                new (getAddressOfFirstElement()) T{*aOther.getAddressOfFirstElement()};
            } else {
                _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
            }
        } else {
            auto* buffer =
                _allocateBuffer(aOther.getCapacity() +
                                N_OBJECTS_FOR_SIZE_DATA); // TODO: allocate only as much as is needed
            std::memcpy(buffer, aOther._loadHeapPointer(), 2 * sizeof(std::uint32_t));
            for (std::uint32_t i = 0; i < aOther.getSize(); i += 1) {
                new (buffer + N_OBJECTS_FOR_SIZE_DATA + i) T{aOther.getAddressOfFirstElement()[i]};
            }
            _storeHeapPointer(buffer);
        }
    }

    CompressedSmallVectorStorage_1& operator=(const CompressedSmallVectorStorage_1& aOther) {
        if (&aOther != this) {
            this->~CompressedSmallVectorStorage_1();
            if (aOther._isInPlace()) {
                if (aOther._getSizeBit()) {
                    _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT | SIZE_BIT;
                    new (getAddressOfFirstElement()) T{*aOther.getAddressOfFirstElement()};
                } else {
                    _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
                }
            } else {
                auto* buffer = _allocateBuffer(aOther.getCapacity() + N_OBJECTS_FOR_SIZE_DATA);
                std::memcpy(buffer, aOther._loadHeapPointer(), 2 * sizeof(std::uint32_t));
                for (std::uint32_t i = 0; i < aOther.getSize(); i += 1) {
                    new (buffer + N_OBJECTS_FOR_SIZE_DATA + i) T{aOther.getAddressOfFirstElement()[i]};
                }
                _storeHeapPointer(buffer);
            }
        }
        return *this;
    }

    CompressedSmallVectorStorage_1(CompressedSmallVectorStorage_1&& aOther) {
        if (aOther._isInPlace()) {
            if (aOther._getSizeBit()) {
                _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT | SIZE_BIT;
                new (getAddressOfFirstElement()) T{std::move(*aOther.getAddressOfFirstElement())};
                aOther.getAddressOfFirstElement()->~T();
                aOther._data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
            } else {
                _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
            }
        } else {
            _storeHeapPointer(aOther._loadHeapPointer());
            aOther._data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
        }
    }

    CompressedSmallVectorStorage_1& operator=(CompressedSmallVectorStorage_1&& aOther) {
        if (&aOther != this) {
            this->~CompressedSmallVectorStorage_1();
            if (aOther._isInPlace()) {
                if (aOther._getSizeBit()) {
                    _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT | SIZE_BIT;
                    new (getAddressOfFirstElement()) T{std::move(*aOther.getAddressOfFirstElement())};
                    aOther.getAddressOfFirstElement()->~T();
                    aOther._data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
                } else {
                    _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
                }
            } else {
                _storeHeapPointer(aOther._loadHeapPointer());
                aOther._data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
            }
        }
        return *this;
    }

    std::uint32_t getCapacity() const {
        if (_isInPlace()) {
            return 1u;
        }
        auto* p = _loadHeapPointer();
        return _loadCapacityFromHeap(p);
    }

    void setCapacity(std::uint32_t aNewCapacity) {
        const auto capacity = getCapacity();
        if (aNewCapacity > capacity) {
            _increaseCapacity(aNewCapacity);
        } else {
            _decreaseCapacity(aNewCapacity);
        }
    }

    std::uint32_t getSize() const {
        if (_isInPlace()) {
            return static_cast<std::uint32_t>(_getSizeBit());
        }
        auto* p = _loadHeapPointer();
        return _loadSizeFromHeap(p);
    }

    void adjustSizeCounter(std::int32_t aOffset) {
        const auto size = getSize();
        HG_ASSERT((aOffset != 0) &&
                  ((aOffset > 0) ? (size + static_cast<std::uint32_t>(aOffset) <= getCapacity())
                                 : (size >= static_cast<std::uint32_t>(-aOffset))));
        const std::uint32_t newSize = CalcSizeWithOffset(size, aOffset);
        if (_isInPlace()) {
            _data.storageBuffer[BUFFER_SIZE - 1] =
                (_data.storageBuffer[BUFFER_SIZE - 1] & ~SIZE_BIT) | ((newSize > 0) ? SIZE_BIT : 0);
        } else {
            _storeSizeOnHeap(_loadHeapPointer(), newSize);
        }
    }

    T* getAddressOfFirstElement() {
        if (_isInPlace()) {
            return reinterpret_cast<T*>(&_data.storageBuffer);
        }
        return (static_cast<T*>(_loadHeapPointer()) + N_OBJECTS_FOR_SIZE_DATA);
    }

    const T* getAddressOfFirstElement() const {
        return const_cast<CompressedSmallVectorStorage_1*>(this)->getAddressOfFirstElement();
    }

    void clear() {
        const auto size = getSize();
        if (size > 0) {
            _destroyAllElements(getAddressOfFirstElement(), size);
            const auto sizeAdjustment = -static_cast<std::int32_t>(size);
            adjustSizeCounter(sizeAdjustment);
        }
    }

private:
    //! Tells how many allocations sizeof(T) are needed to provide at least 64 bits
    //! (to provide storage for size and capacity counters).
    static constexpr unsigned N_OBJECTS_FOR_SIZE_DATA =
        (2 * sizeof(std::uint32_t) + sizeof(T) - 1) / sizeof(T);

    static constexpr char IN_PLACE_BIT = 0x01; // when set, size == 0 or 1
    static constexpr char SIZE_BIT     = 0x02; // when set, size == 1+

    // STORAGE SCHEME:
    // - In place:
    //   - bytes 0..6 of _data.storageBuffer provide storage for T.
    //   - byte 7 of _data.storageBuffer stores IN_PLACE_BIT and SIZE_BIT.
    //
    // - On heap:
    //   - bytes 0..7 of _data.storageBuffer store pointer to the buffer (which is on the heap)
    //     with the two least significant bits of the pointer being used to store
    //     IN_PLACE_BIT and SIZE_BIT (the memory we get back from malloc and company is
    //     generally aligned to at least 4 bytes, so these two are always 0). The
    //     pointer is stored in BIG ENDIAN format, thus it is always possible to check
    //     the status of the container by inspecting _data.storageBuffer[7]. The first 8 bytes of
    //     the heap buffer store size and capacity (in that order) of the container.
    static constexpr std::size_t BUFFER_SIZE = sizeof(void*);

    class DataHolder : public StorePointerIfNotEmpty<const taAllocator> {
    public:
        DataHolder()
            : StorePointerIfNotEmpty<const taAllocator>{} {}
        DataHolder(taAllocator* aAllocator)
            : StorePointerIfNotEmpty<const taAllocator>{aAllocator} {}

        alignas(void*) char storageBuffer[BUFFER_SIZE];

        taAllocator& getAllocator() {
            const auto* p = this->getPointer();
            HG_ASSERT(p != nullptr);
            return *p;
        }
    } _data;

    bool _isInPlace() const {
        return _data.storageBuffer[BUFFER_SIZE - 1] & IN_PLACE_BIT;
    }

    bool _getSizeBit() const {
        return _data.storageBuffer[BUFFER_SIZE - 1] & SIZE_BIT;
    }

    void* _loadHeapPointer() {
        const auto intptr = *reinterpret_cast<std::uintptr_t*>(&_data.storageBuffer);
        return reinterpret_cast<void*>(BigEndianToHost64(intptr) & ~0x03);
    }

    const void* _loadHeapPointer() const {
        return const_cast<CompressedSmallVectorStorage_1*>(this)->_loadHeapPointer();
    }

    void _storeHeapPointer(void* aPointer) {
        const auto intptr = HostToBigEndian64(reinterpret_cast<std::uintptr_t>(aPointer) & ~0x03);
        std::memcpy(&_data.storageBuffer, &intptr, 8);
    }

    static void _storeSizeOnHeap(void* aBufferBeginning, std::uint32_t aSize) {
        std::memcpy(aBufferBeginning, &aSize, sizeof(aSize));
    }

    static void _storeCapacityOnHeap(void* aBufferBeginning, std::uint32_t aCapacity) {
        std::memcpy(static_cast<char*>(aBufferBeginning) + sizeof(aCapacity),
                    &aCapacity,
                    sizeof(aCapacity));
    }

    static std::uint32_t _loadSizeFromHeap(const void* aBufferBeginning) {
        std::uint32_t size;
        std::memcpy(&size, aBufferBeginning, sizeof(size));
        return size;
    }

    static std::uint32_t _loadCapacityFromHeap(const void* aBufferBeginning) {
        std::uint32_t capacity;
        std::memcpy(&capacity,
                    static_cast<const char*>(aBufferBeginning) + sizeof(capacity),
                    sizeof(capacity));
        return capacity;
    }

    T* _allocateBuffer(std::uint32_t aTCount) {
        const auto align = alignof(T);
        const auto size  = sizeof(T);
        auto*      p     = (T*)std::malloc(aTCount * size); // TODO(use allocator)
        HG_HARD_ASSERT(p != nullptr);
        return p;
    }

    void _freeBuffer(T* aBuffer) {
        free(aBuffer); // TODO(use allocator)
    }

    void _increaseCapacity(std::uint32_t aNewCapacity) {
        HG_ASSERT(aNewCapacity > getCapacity());

        T* buffer = _allocateBuffer(aNewCapacity + N_OBJECTS_FOR_SIZE_DATA);
        HG_HARD_ASSERT((reinterpret_cast<std::uintptr_t>(buffer) & 0x03) == 0);

        if (_isInPlace()) {
            auto* first = getAddressOfFirstElement();
            if (getSize() == 1) {
                new (buffer + N_OBJECTS_FOR_SIZE_DATA) T{std::move(*first)};
                _destroyAllElements(first, 1);
                _storeSizeOnHeap(buffer, 1);
            } else {
                _storeSizeOnHeap(buffer, 0);
            }
        } else {
            auto*      elements = getAddressOfFirstElement();
            const auto size     = getSize();
            for (std::uint32_t i = 0; i < size; i += 1) {
                new (buffer + N_OBJECTS_FOR_SIZE_DATA + i) T{std::move(elements[i])};
            }
            _destroyAllElements(elements, size);
            _storeSizeOnHeap(buffer, size);
            _freeBuffer((T*)_loadHeapPointer());
        }

        _storeCapacityOnHeap(buffer, aNewCapacity);
        _storeHeapPointer(buffer);
    }

    void _decreaseCapacity(std::uint32_t aNewCapacity) {
        HG_NOT_IMPLEMENTED();
    }

    void _destroyAllElements(T* aElements, std::uint32_t aSize) {
        for (std::uint32_t i = 0; i < aSize; i += 1) {
            aElements[aSize - 1 - i].~T();
        }
    }
};

// MARK: Storage 2

template <class T, class taAllocator>
class CompressedSmallVectorStorage_2 {
public:
    static_assert(sizeof(T) >= sizeof(void*) && sizeof(T) < sizeof(void*) * 2, "");

    CompressedSmallVectorStorage_2(const taAllocator* aAllocator = nullptr)
        : _data{aAllocator} {
        _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
    }

    ~CompressedSmallVectorStorage_2() {
        const auto size = getSize();
        if (size > 0) {
            _destroyAllElements(getAddressOfFirstElement(), size);
        }
        if (!_isInPlace()) {
            auto* p = _loadHeapPointer();
            _freeBuffer((T*)p);
        }
    }

    CompressedSmallVectorStorage_2(const CompressedSmallVectorStorage_2& aOther) {
        if (aOther._isInPlace()) {
            if (aOther._getSizeBit()) {
                _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT | SIZE_BIT;
                new (getAddressOfFirstElement()) T{*aOther.getAddressOfFirstElement()};
            } else {
                _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
            }
        } else {
            auto* buffer = _allocateBuffer(aOther.getCapacity());
            for (std::uint32_t i = 0; i < aOther.getSize(); i += 1) {
                new (buffer + i) T{aOther.getAddressOfFirstElement()[i]};
            }
            std::memcpy(_data.storageBuffer, aOther._data.storageBuffer, 2 * sizeof(std::uint32_t));
            _storeHeapPointer(buffer);
        }
    }

    CompressedSmallVectorStorage_2& operator=(const CompressedSmallVectorStorage_2& aOther) {
        if (&aOther != this) {
            this->~CompressedSmallVectorStorage_2();
            if (aOther._isInPlace()) {
                if (aOther._getSizeBit()) {
                    _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT | SIZE_BIT;
                    new (getAddressOfFirstElement()) T{*aOther.getAddressOfFirstElement()};
                } else {
                    _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
                }
            } else {
                auto* buffer = _allocateBuffer(aOther.getCapacity());
                for (std::uint32_t i = 0; i < aOther.getSize(); i += 1) {
                    new (buffer + i) T{aOther.getAddressOfFirstElement()[i]};
                }
                std::memcpy(_data.storageBuffer, aOther._data.storageBuffer, 2 * sizeof(std::uint32_t));
                _storeHeapPointer(buffer);
            }
        }
        return *this;
    }

    CompressedSmallVectorStorage_2(CompressedSmallVectorStorage_2&& aOther) {
        if (aOther._isInPlace()) {
            if (aOther._getSizeBit()) {
                _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT | SIZE_BIT;
                new (getAddressOfFirstElement()) T{std::move(*aOther.getAddressOfFirstElement())};
                aOther.getAddressOfFirstElement()->~T();
                aOther._data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
            } else {
                _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
            }
        } else {
            std::memcpy(_data.storageBuffer, aOther._data.storageBuffer, 2 * sizeof(std::uint32_t));
            _storeHeapPointer(aOther._loadHeapPointer());
            aOther._data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
        }
    }

    CompressedSmallVectorStorage_2& operator=(CompressedSmallVectorStorage_2&& aOther) {
        if (&aOther != this) {
            this->~CompressedSmallVectorStorage_2();
            if (aOther._isInPlace()) {
                if (aOther._getSizeBit()) {
                    _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT | SIZE_BIT;
                    new (getAddressOfFirstElement()) T{std::move(*aOther.getAddressOfFirstElement())};
                    aOther.getAddressOfFirstElement()->~T();
                    aOther._data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
                } else {
                    _data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
                }
            } else {
                std::memcpy(_data.storageBuffer, aOther._data.storageBuffer, 2 * sizeof(std::uint32_t));
                _storeHeapPointer(aOther._loadHeapPointer());
                aOther._data.storageBuffer[BUFFER_SIZE - 1] = IN_PLACE_BIT;
            }
        }
        return *this;
    }

    std::uint32_t getCapacity() const {
        if (_isInPlace()) {
            return 1u;
        }
        return _loadCapacityFromStorage();
    }

    void setCapacity(std::uint32_t aNewCapacity) {
        const auto capacity = getCapacity();
        if (aNewCapacity > capacity) {
            _increaseCapacity(aNewCapacity);
        } else {
            _decreaseCapacity(aNewCapacity);
        }
    }

    std::uint32_t getSize() const {
        if (_isInPlace()) {
            return static_cast<std::uint32_t>(_getSizeBit());
        }
        return _loadSizeFromStorage();
    }

    void adjustSizeCounter(std::int32_t aOffset) {
        const auto size = getSize();
        HG_ASSERT((aOffset != 0) &&
                  ((aOffset > 0) ? (size + static_cast<std::uint32_t>(aOffset) <= getCapacity())
                                 : (size >= static_cast<std::uint32_t>(-aOffset))));
        const std::uint32_t newSize = CalcSizeWithOffset(size, aOffset);
        if (_isInPlace()) {
            _data.storageBuffer[BUFFER_SIZE - 1] =
                (_data.storageBuffer[BUFFER_SIZE - 1] & ~SIZE_BIT) | ((newSize > 0) ? SIZE_BIT : 0);
        } else {
            _storeSizeInStorage(newSize);
        }
    }

    T* getAddressOfFirstElement() {
        if (_isInPlace()) {
            return reinterpret_cast<T*>(&_data.storageBuffer);
        }
        return static_cast<T*>(_loadHeapPointer());
    }

    const T* getAddressOfFirstElement() const {
        return const_cast<CompressedSmallVectorStorage_2*>(this)->getAddressOfFirstElement();
    }

    void clear() {
        const auto size = getSize();
        if (size > 0) {
            _destroyAllElements(getAddressOfFirstElement(), size);
            const auto sizeAdjustment = -static_cast<std::int32_t>(size);
            adjustSizeCounter(sizeAdjustment);
        }
    }

private:
    static constexpr char IN_PLACE_BIT = 0x01; // when set, size == 0 or 1
    static constexpr char SIZE_BIT     = 0x02; // when set, size == 1+

    // STORAGE SCHEME:
    // - In place:
    //   - bytes 0..4 of _data.storageBuffer provide storage for T.
    //   - byte 15 of _data.storageBuffer stores IN_PLACE_BIT and SIZE_BIT.
    //
    // - On heap:
    //   - bytes 0..3 of _data.storageBuffer store the size of the container.
    //   - bytes 4..7 of _data.storageBuffer store the capacity of the container.
    //   - bytes 8..15 of _data.storageBuffer store pointer to the buffer (which is on the heap)
    //     with the two least significant bits of the pointer being used to store
    //     IN_PLACE_BIT and SIZE_BIT (the memory we get back from malloc and company is
    //     generally aligned to at least 4 bytes, so these two are always 0). The
    //     pointer is stored in BIG ENDIAN format, thus it is always possible to check
    //     the status of the container by inspecting _data.storageBuffer[15].
    static constexpr std::size_t BUFFER_SIZE = sizeof(void*) * 2;

    class DataHolder : StorePointerIfNotEmpty<const taAllocator> {
    public:
        DataHolder()
            : StorePointerIfNotEmpty<const taAllocator>{} {}
        DataHolder(taAllocator* aAllocator)
            : StorePointerIfNotEmpty<const taAllocator>{aAllocator} {}

        alignas(void*) char storageBuffer[BUFFER_SIZE];

        const taAllocator& getAllocator() {
            const auto* p = this->getPointer();
            HG_ASSERT(p != nullptr);
            return *p;
        }
    } _data;

    bool _isInPlace() const {
        return _data.storageBuffer[BUFFER_SIZE - 1] & IN_PLACE_BIT;
    }

    bool _getSizeBit() const {
        return _data.storageBuffer[BUFFER_SIZE - 1] & SIZE_BIT;
    }

    void* _loadHeapPointer() {
        std::uintptr_t intptr;
        std::memcpy(&intptr, &_data.storageBuffer[8], sizeof(intptr));
        return reinterpret_cast<void*>(BigEndianToHost64(intptr) & ~0x03);
    }

    const void* _loadHeapPointer() const {
        return const_cast<CompressedSmallVectorStorage_2*>(this)->_loadHeapPointer();
    }

    void _storeHeapPointer(void* aPointer) {
        const auto intptr = HostToBigEndian64(reinterpret_cast<std::uintptr_t>(aPointer) & ~0x03);
        std::memcpy(&_data.storageBuffer[8], &intptr, 8);
    }

    void _storeSizeInStorage(std::uint32_t aSize) {
        std::memcpy(&_data.storageBuffer[0], &aSize, sizeof(aSize));
    }

    void _storeCapacityInStorage(std::uint32_t aCapacity) {
        std::memcpy(&_data.storageBuffer[4], &aCapacity, sizeof(aCapacity));
    }

    std::uint32_t _loadSizeFromStorage() const {
        std::uint32_t size;
        std::memcpy(&size, &_data.storageBuffer[0], sizeof(size));
        return size;
    }

    std::uint32_t _loadCapacityFromStorage() const {
        std::uint32_t capacity;
        std::memcpy(&capacity, &_data.storageBuffer[4], sizeof(capacity));
        return capacity;
    }

    T* _allocateBuffer(std::uint32_t aTCount) {
        const auto align = alignof(T);
        const auto size  = sizeof(T);
        auto*      p     = (T*)std::malloc(aTCount * size); // TODO(use allocator)
        HG_HARD_ASSERT(p != nullptr);
        return p;
    }

    void _freeBuffer(T* aBuffer) {
        free(aBuffer); // TODO(use allocator)
    }

    void _increaseCapacity(std::uint32_t aNewCapacity) {
        HG_ASSERT(aNewCapacity > getCapacity());

        T* buffer = _allocateBuffer(aNewCapacity);
        HG_HARD_ASSERT((reinterpret_cast<std::uintptr_t>(buffer) & 0x03) == 0);

        if (_isInPlace()) {
            auto* first = getAddressOfFirstElement();
            if (getSize() == 1) {
                new (buffer) T{std::move(*first)};
                _destroyAllElements(first, 1);
                _storeSizeInStorage(1);
            } else {
                _storeSizeInStorage(0);
            }
        } else {
            auto*      elements = getAddressOfFirstElement();
            const auto size     = getSize();
            for (std::uint32_t i = 0; i < size; i += 1) {
                new (buffer + i) T{std::move(elements[i])};
            }
            _destroyAllElements(elements, size);
            _storeSizeInStorage(size);
            _freeBuffer((T*)_loadHeapPointer());
        }

        _storeCapacityInStorage(aNewCapacity);
        _storeHeapPointer(buffer);
    }

    void _decreaseCapacity(std::uint32_t aNewCapacity) {
        HG_NOT_IMPLEMENTED();
    }

    void _destroyAllElements(T* aElements, std::uint32_t aSize) {
        for (std::uint32_t i = 0; i < aSize; i += 1) {
            aElements[aSize - 1 - i].~T();
        }
    }
};

// MARK: Storage 3

template <class T, class taAllocator>
class CompressedSmallVectorStorage_3 {
public:
    static_assert(sizeof(T) >= sizeof(void*) * 2, "");

    CompressedSmallVectorStorage_3(const taAllocator* aAllocator = nullptr)
        : _data{aAllocator} {}

    ~CompressedSmallVectorStorage_3() {
        const auto size = getSize();
        if (size > 0) {
            _destroyAllElements(getAddressOfFirstElement(), size);
        }
        if (!_isInPlace()) {
            auto* p = _loadHeapPointer();
            _freeBuffer((T*)p);
        }
    }

    CompressedSmallVectorStorage_3(const CompressedSmallVectorStorage_3& aOther) {
        if (aOther._isInPlace()) {
            if (aOther._getSizeBit()) {
                _status = IN_PLACE_BIT | SIZE_BIT;
                new (getAddressOfFirstElement()) T{*aOther.getAddressOfFirstElement()};
            } else {
                _status = IN_PLACE_BIT;
            }
        } else {
            auto* buffer = _allocateBuffer(aOther.getCapacity());
            for (std::uint32_t i = 0; i < aOther.getSize(); i += 1) {
                new (buffer + i) T{aOther.getAddressOfFirstElement()[i]};
            }
            _storeSizeInStorage(aOther._loadSizeFromStorage());
            _storeCapacityInStorage(aOther._loadCapacityFromStorage());
            _storeHeapPointer(buffer);
            _status = ~IN_PLACE_BIT & ~SIZE_BIT;
        }
    }

    CompressedSmallVectorStorage_3& operator=(const CompressedSmallVectorStorage_3& aOther) {
        if (&aOther != this) {
            this->~CompressedSmallVectorStorage_3();
            if (aOther._isInPlace()) {
                if (aOther._getSizeBit()) {
                    _status = IN_PLACE_BIT | SIZE_BIT;
                    new (getAddressOfFirstElement()) T{*aOther.getAddressOfFirstElement()};
                } else {
                    _status = IN_PLACE_BIT;
                }
            } else {
                auto* buffer = _allocateBuffer(aOther.getCapacity());
                for (std::uint32_t i = 0; i < aOther.getSize(); i += 1) {
                    new (buffer + i) T{aOther.getAddressOfFirstElement()[i]};
                }
                _storeSizeInStorage(aOther._loadSizeFromStorage());
                _storeCapacityInStorage(aOther._loadCapacityFromStorage());
                _storeHeapPointer(buffer);
                _status = ~IN_PLACE_BIT & ~SIZE_BIT;
            }
        }
        return *this;
    }

    CompressedSmallVectorStorage_3(CompressedSmallVectorStorage_3&& aOther) {
        if (aOther._isInPlace()) {
            if (aOther._getSizeBit()) {
                _status = IN_PLACE_BIT | SIZE_BIT;
                new (getAddressOfFirstElement()) T{std::move(*aOther.getAddressOfFirstElement())};
                aOther.getAddressOfFirstElement()->~T();
                aOther._status = IN_PLACE_BIT;
            } else {
                _status = IN_PLACE_BIT;
            }
        } else {
            _storeSizeInStorage(aOther._loadSizeFromStorage());
            _storeCapacityInStorage(aOther._loadCapacityFromStorage());
            _storeHeapPointer(aOther._loadHeapPointer());
            _status        = ~IN_PLACE_BIT & ~SIZE_BIT;
            aOther._status = IN_PLACE_BIT;
        }
    }

    CompressedSmallVectorStorage_3& operator=(CompressedSmallVectorStorage_3&& aOther) {
        if (&aOther != this) {
            this->~CompressedSmallVectorStorage_3();
            if (aOther._isInPlace()) {
                if (aOther._getSizeBit()) {
                    _status = IN_PLACE_BIT | SIZE_BIT;
                    new (getAddressOfFirstElement()) T{std::move(*aOther.getAddressOfFirstElement())};
                    aOther.getAddressOfFirstElement()->~T();
                    aOther._status = IN_PLACE_BIT;
                } else {
                    _status = IN_PLACE_BIT;
                }
            } else {
                _storeSizeInStorage(aOther._loadSizeFromStorage());
                _storeCapacityInStorage(aOther._loadCapacityFromStorage());
                _storeHeapPointer(aOther._loadHeapPointer());
                _status        = ~IN_PLACE_BIT & ~SIZE_BIT;
                aOther._status = IN_PLACE_BIT;
            }
        }
        return *this;
    }

    std::uint32_t getCapacity() const {
        if (_isInPlace()) {
            return 1u;
        }
        return _loadCapacityFromStorage();
    }

    void setCapacity(std::uint32_t aNewCapacity) {
        const auto capacity = getCapacity();
        if (aNewCapacity > capacity) {
            _increaseCapacity(aNewCapacity);
        } else {
            _decreaseCapacity(aNewCapacity);
        }
    }

    std::uint32_t getSize() const {
        if (_isInPlace()) {
            return static_cast<std::uint32_t>(_getSizeBit());
        }
        return _loadSizeFromStorage();
    }

    void adjustSizeCounter(std::int32_t aOffset) {
        const auto size = getSize();
        HG_ASSERT((aOffset != 0) &&
                  ((aOffset > 0) ? (size + static_cast<std::uint32_t>(aOffset) <= getCapacity())
                                 : (size >= static_cast<std::uint32_t>(-aOffset))));
        const std::uint32_t newSize = CalcSizeWithOffset(size, aOffset);
        if (_isInPlace()) {
            _status = (_status & ~SIZE_BIT) | ((newSize > 0) ? SIZE_BIT : 0);
        } else {
            _storeSizeInStorage(newSize);
        }
    }

    T* getAddressOfFirstElement() {
        if (_isInPlace()) {
            return reinterpret_cast<T*>(&_data.objectStorage);
        }
        return static_cast<T*>(_loadHeapPointer());
    }

    const T* getAddressOfFirstElement() const {
        return const_cast<CompressedSmallVectorStorage_3*>(this)->getAddressOfFirstElement();
    }

    void clear() {
        const auto size = getSize();
        if (size > 0) {
            _destroyAllElements(getAddressOfFirstElement(), size);
            const auto sizeAdjustment = -static_cast<std::int32_t>(size);
            adjustSizeCounter(sizeAdjustment);
        }
    }

private:
    static constexpr char IN_PLACE_BIT = 0x01; // when set, size == 0 or 1
    static constexpr char SIZE_BIT     = 0x02; // when set, size == 1+

    struct HeapInfo {
        void*         ptr;
        std::uint32_t size;
        std::uint32_t capacity;
    };

    // STORAGE SCHEME:
    // - In place:
    //   - _objectStorage of the union provides storage for T.
    //   - _status stores IN_PLACE_BIT and SIZE_BIT.
    //
    // - On heap:
    //   - _heapInfo of the union stores the address, size and capacity of
    //     the heap buffer which contains instances of T.
    //   - _status stores IN_PLACE_BIT and SIZE_BIT.
    class DataHolder : StorePointerIfNotEmpty<const taAllocator> {
    public:
        DataHolder()
            : StorePointerIfNotEmpty<const taAllocator>{} {}
        DataHolder(const taAllocator* aAllocator)
            : StorePointerIfNotEmpty<const taAllocator>{aAllocator} {}

        union {
            alignas(T) char objectStorage[sizeof(T)];
            HeapInfo heapInfo;
        };

        const taAllocator& getAllocator() {
            const auto* p = this->getPointer();
            HG_ASSERT(p != nullptr);
            return *p;
        }
    } _data;
    char _status = IN_PLACE_BIT;

    bool _isInPlace() const {
        return _status & IN_PLACE_BIT;
    }

    bool _getSizeBit() const {
        return _status & SIZE_BIT;
    }

    void* _loadHeapPointer() {
        return _data.heapInfo.ptr;
    }

    const void* _loadHeapPointer() const {
        return _data.heapInfo.ptr;
    }

    void _storeHeapPointer(void* aPointer) {
        _data.heapInfo.ptr = aPointer;
    }

    void _storeSizeInStorage(std::uint32_t aSize) {
        _data.heapInfo.size = aSize;
    }

    void _storeCapacityInStorage(std::uint32_t aCapacity) {
        _data.heapInfo.capacity = aCapacity;
    }

    std::uint32_t _loadSizeFromStorage() const {
        return _data.heapInfo.size;
    }

    std::uint32_t _loadCapacityFromStorage() const {
        return _data.heapInfo.capacity;
    }

    T* _allocateBuffer(std::uint32_t aTCount) {
        const auto align = alignof(T);
        const auto size  = sizeof(T);
        auto*      p     = (T*)std::malloc(aTCount * size); // TODO(use allocator)
        HG_HARD_ASSERT(p != nullptr);
        return p;
    }

    void _freeBuffer(T* aBuffer) {
        free(aBuffer); // TODO(use allocator)
    }

    void _increaseCapacity(std::uint32_t aNewCapacity) {
        HG_ASSERT(aNewCapacity > getCapacity());

        T* buffer = _allocateBuffer(aNewCapacity);
        HG_HARD_ASSERT((reinterpret_cast<std::uintptr_t>(buffer) & 0x03) == 0);

        if (_isInPlace()) {
            auto* first = getAddressOfFirstElement();
            if (getSize() == 1) {
                new (buffer) T{std::move(*first)};
                _destroyAllElements(first, 1);
                _storeSizeInStorage(1);
            } else {
                _storeSizeInStorage(0);
            }
            _status = (~IN_PLACE_BIT & ~SIZE_BIT);
        } else {
            auto*      elements = getAddressOfFirstElement();
            const auto size     = getSize();
            for (std::uint32_t i = 0; i < size; i += 1) {
                new (buffer + i) T{std::move(elements[i])};
            }
            _destroyAllElements(elements, size);
            _storeSizeInStorage(size);
            _freeBuffer((T*)_loadHeapPointer());
        }

        _storeCapacityInStorage(aNewCapacity);
        _storeHeapPointer(buffer);
    }

    void _decreaseCapacity(std::uint32_t aNewCapacity) {
        HG_NOT_IMPLEMENTED();
    }

    void _destroyAllElements(T* aElements, std::uint32_t aSize) {
        for (std::uint32_t i = 0; i < aSize; i += 1) {
            aElements[aSize - 1 - i].~T();
        }
    }
};

// MARK: Storage Selector

enum class TypeSizeClass {
    SMALL,
    MEDIUM,
    LARGE
};

template <class T>
constexpr TypeSizeClass GetSizeClass() {
    return (sizeof(T) < sizeof(void*))
               ? TypeSizeClass::SMALL
               : ((sizeof(T) < sizeof(void*) * 2) ? TypeSizeClass::MEDIUM : TypeSizeClass::LARGE);
}

template <class T, class taAllocator, TypeSizeClass taSize = GetSizeClass<T>()>
class CompressedSmallVectorStorageSelector {};

template <typename T, class taAllocator>
class CompressedSmallVectorStorageSelector<T, taAllocator, TypeSizeClass::SMALL> {
public:
    using type = CompressedSmallVectorStorage_1<T, taAllocator>;
};

template <typename T, class taAllocator>
class CompressedSmallVectorStorageSelector<T, taAllocator, TypeSizeClass::MEDIUM> {
public:
    using type = CompressedSmallVectorStorage_2<T, taAllocator>;
};

template <typename T, class taAllocator>
class CompressedSmallVectorStorageSelector<T, taAllocator, TypeSizeClass::LARGE> {
public:
    using type = CompressedSmallVectorStorage_3<T, taAllocator>;
};

} // namespace detail

// MARK: Growth Strategy

namespace growth_strategy {
class IncreaseByOne {
public:
    std::uint32_t operator()(std::uint32_t aCurrentSize) const {
        return aCurrentSize + 1;
    }
};

class IncreaseByFiftyPercent {
public:
    std::uint32_t operator()(std::uint32_t aCurrentSize) const {
        if ((aCurrentSize & 0x01) > 0) {
            aCurrentSize += 1; // make it even
        }
        return static_cast<std::uint32_t>(aCurrentSize * 1.5);
    }
};
} // namespace growth_strategy

// MARK: Vector

//! CompressedSmallVector is a sequential random-access container geared towards reducing
//! memory usage and the number of dynamic allocations in cases where it's highly likely
//! that the vector will only ever be empty or have a single element.
//!
//! It does so by providing storage for a single element in-place, same as other 'small vector'
//! implementations, and only resorts to dynamic allocations if storage for 2 or more elements
//! are needed. It also uses some other techniques, such as packing data into unions, to reduce
//! the size of the vector object itself. The concrete size depends on the size of T, as follows:
//! - `sizeof(T) < 8`: `CompressedSmallVector<T>` takes up 8 bytes (when size > 1, size and
//!   capacity are also stored on the heap).
//! - `sizeof(T) >= 9 && sizeof(T) < 16`: `CompressedSmallVector<T>` takes up 16 bytes (size and
//!   capacity are stored in-place).
//! - `sizeof(T) >= 16`: `CompressedSmallVector<T>` takes up `sizeof(T) + 1` bytes (+padding).
//!
//! In any case, the provided API is a subset of the API provided by `std::vector`.
//! However, the maximum size and capacity of `CompressedSmallVector` are 4,294,967,295 elements,
//! because size and capacity are stored as 32-bit integers.
template <class T,
          class taGrowthStrategy = growth_strategy::IncreaseByFiftyPercent,
          class taAllocator      = std::allocator<T>>
class CompressedSmallVector {
public:
    using value_type             = T;
    using allocator_type         = taAllocator;
    using iterator               = value_type*;
    using const_iterator         = const value_type*;
    using reverse_iterator       = typename std::reverse_iterator<iterator>;
    using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;
    using size_type              = std::size_t;

    CompressedSmallVector()
        : _storage{nullptr} {}

    CompressedSmallVector(const taAllocator& aAllocator)
        : _storage{&aAllocator} {}

    std::size_t size() const {
        return _storage.getSize();
    }

    std::size_t capacity() const {
        return _storage.getCapacity();
    }

    void reserve(std::size_t aDesiredCapacity) {
        const auto desiredCapacity = static_cast<std::uint32_t>(aDesiredCapacity);
        if (desiredCapacity > _storage.getCapacity()) {
            std::uint32_t newCapacity = _storage.getCapacity();
            while (newCapacity < desiredCapacity) {
                newCapacity = _growCapacity(newCapacity);
            }
            _storage.setCapacity(newCapacity);
        }
    }

    void resize(std::size_t aNewSize) {
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

    T& operator[](std::size_t aIndex) {
        return _storage.getAddressOfFirstElement()[aIndex];
    }

    const T& operator[](std::size_t aIndex) const {
        return _storage.getAddressOfFirstElement()[aIndex];
    }

    T& at(std::size_t aIndex) {
        HG_VALIDATE_ARGUMENT(aIndex < size());
        return _storage.getAddressOfFirstElement()[aIndex];
    }

    const T& at(std::size_t aIndex) const {
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
    using Storage = typename detail::CompressedSmallVectorStorageSelector<T, taAllocator>::type;
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
