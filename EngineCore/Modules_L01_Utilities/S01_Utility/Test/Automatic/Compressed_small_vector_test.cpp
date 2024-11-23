// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Compressed_small_vector.hpp>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

namespace jbatnozic {
namespace hobgoblin {
namespace util {

//! Dummy stateful allocator (state size = 2 pointers) used to check that CompressedSmallVector
//! is of the correct size when this is used.
template <class T>
class DummyAllocator {
public:
    using value_type = T;

    T* allocate(std::size_t aElementCount) {
        return static_cast<T*>(std::malloc(sizeof(T) * aElementCount));
    }

    void deallocate(const T* aElements, std::size_t /*aElementCount*/) const {
        std::free(aElements);
    }

private:
    void* _p1 = nullptr;
    void* _p2 = nullptr;
};

static_assert(not std::is_empty<DummyAllocator<int>>::value, "DummyAllocator must not be empty.");

#define SIZEOF_POINTER (UINTPTR_MAX / 255 % 255)

namespace detail {

// Check storage selection of vector of int16_t (stateless allocator)

static_assert(
    std::is_same_v<
    CompressedSmallVectorStorageSelector<std::int16_t, 0, std::allocator<std::int16_t>>::type,
    CompressedSmallVectorStorage_Small<std::int16_t, 0, std::allocator<std::int16_t>>>,
    "Unexpected type");

static_assert(
    std::is_same_v<
        CompressedSmallVectorStorageSelector<std::int16_t, 1, std::allocator<std::int16_t>>::type,
        CompressedSmallVectorStorage_Small<std::int16_t, 1, std::allocator<std::int16_t>>>,
    "Unexpected type");

static_assert(
    std::is_same_v<
        CompressedSmallVectorStorageSelector<std::int16_t, 2, std::allocator<std::int16_t>>::type,
        CompressedSmallVectorStorage_Small<std::int16_t, 2, std::allocator<std::int16_t>>>,
    "Unexpected type");

static_assert(
    std::is_same_v<
        CompressedSmallVectorStorageSelector<std::int16_t, 3, std::allocator<std::int16_t>>::type,
        CompressedSmallVectorStorage_Small<std::int16_t, 3, std::allocator<std::int16_t>>>,
    "Unexpected type");

static_assert(
    std::is_same_v<
        CompressedSmallVectorStorageSelector<std::int16_t, 4, std::allocator<std::int16_t>>::type,
        CompressedSmallVectorStorage_Large<std::int16_t, 4, std::allocator<std::int16_t>>>,
    "Unexpected type");

// Check storage selection of vector of int16_t (stateful allocator)

static_assert(
    std::is_same_v<
    CompressedSmallVectorStorageSelector<std::int16_t, 0, DummyAllocator<std::int16_t>>::type,
    CompressedSmallVectorStorage_Small<std::int16_t, 0, DummyAllocator<std::int16_t>>>,
    "Unexpected type");

static_assert(
    std::is_same_v<
        CompressedSmallVectorStorageSelector<std::int16_t, 1, DummyAllocator<std::int16_t>>::type,
        CompressedSmallVectorStorage_Small<std::int16_t, 1, DummyAllocator<std::int16_t>>>,
    "Unexpected type");

static_assert(
    std::is_same_v<
        CompressedSmallVectorStorageSelector<std::int16_t, 2, DummyAllocator<std::int16_t>>::type,
        CompressedSmallVectorStorage_Small<std::int16_t, 2, DummyAllocator<std::int16_t>>>,
    "Unexpected type");

static_assert(
    std::is_same_v<
        CompressedSmallVectorStorageSelector<std::int16_t, 3, DummyAllocator<std::int16_t>>::type,
        CompressedSmallVectorStorage_Small<std::int16_t, 3, DummyAllocator<std::int16_t>>>,
    "Unexpected type");

static_assert(
    std::is_same_v<
        CompressedSmallVectorStorageSelector<std::int16_t, 4, DummyAllocator<std::int16_t>>::type,
        CompressedSmallVectorStorage_Large<std::int16_t, 4, DummyAllocator<std::int16_t>>>,
    "Unexpected type");
} // namespace detail

// Check sizes of vector of int16_t (stateless allocator)

template <class T, std::size_t taInPlaceCapacity>
using CSVectorDefaultAlloc =
    CompressedSmallVector<T, taInPlaceCapacity, growth_strategy::Default, std::allocator<T>>;

static_assert(sizeof(CSVectorDefaultAlloc<std::int16_t, 0>) == 8, "Unexpected size");
static_assert(sizeof(CSVectorDefaultAlloc<std::int16_t, 1>) == 8, "Unexpected size");
static_assert(sizeof(CSVectorDefaultAlloc<std::int16_t, 3>) == 8, "Unexpected size");
static_assert(sizeof(CSVectorDefaultAlloc<std::int16_t, 4>) == 16, "Unexpected size");
static_assert(sizeof(CSVectorDefaultAlloc<std::int16_t, 7>) == 16, "Unexpected size");
static_assert(sizeof(CSVectorDefaultAlloc<std::int16_t, 8>) == 18, "Unexpected size");
static_assert(sizeof(CSVectorDefaultAlloc<std::int16_t, 10>) == 22, "Unexpected size");

// Check sizes of vector of int16_t (stateful allocator)

template <class T, std::size_t taInPlaceCapacity>
using CSVectorDummyAlloc =
    CompressedSmallVector<T, taInPlaceCapacity, growth_strategy::Default, DummyAllocator<T>>;

static_assert(sizeof(CSVectorDummyAlloc<std::int16_t, 0>) == 24, "Unexpected size");
static_assert(sizeof(CSVectorDummyAlloc<std::int16_t, 1>) == 24, "Unexpected size");
static_assert(sizeof(CSVectorDummyAlloc<std::int16_t, 3>) == 24, "Unexpected size");
static_assert(sizeof(CSVectorDummyAlloc<std::int16_t, 4>) == 32, "Unexpected size");
static_assert(sizeof(CSVectorDummyAlloc<std::int16_t, 7>) == 32, "Unexpected size");
static_assert(sizeof(CSVectorDummyAlloc<std::int16_t, 8>) == 40, "Unexpected size");
static_assert(sizeof(CSVectorDummyAlloc<std::int16_t, 10>) == 40, "Unexpected size");

//! Mimics a memory pool for the TrackingAllocator (see below).
//! Tracks allocated blocks.
template <class T>
class TrackingAllocatorPool {
public:
    T* allocate(std::size_t aElementCount) {
        auto*      p         = static_cast<T*>(std::malloc(sizeof(T) * aElementCount));
        const bool didInsert = _map.insert(std::make_pair(p, aElementCount)).second;
        EXPECT_TRUE(didInsert);
        return p;
    }

    void deallocate(const T* aElements, std::size_t aElementCount) {
        const auto iter = _map.find(aElements);
        EXPECT_TRUE(iter != _map.end() && iter->second == aElementCount);
        _map.erase(iter);
        std::free(const_cast<T*>(aElements));
    }

    bool HasNoUnfreedMemory() const {
        return _map.empty();
    }

private:
    std::unordered_map<const T*, std::size_t> _map;
};

template <class T>
class TrackingAllocator {
public:
    using value_type = T;

    explicit TrackingAllocator(TrackingAllocatorPool<T>& aPool)
        : _pool{&aPool} {}

    T* allocate(std::size_t aElementCount) {
        return _pool->allocate(aElementCount);
    }

    void deallocate(const T* aElements, std::size_t aElementCount) {
        _pool->deallocate(aElements, aElementCount);
    }

private:
    TrackingAllocatorPool<T>* _pool;
};

//! Class with a static counter of how many instances of it currently exist.
//! Used to test that CompressedSmallVector destroys elements as expected.
namespace {
template <int taSize>
class InstanceCountedClass {
public:
    InstanceCountedClass() {
        _instanceCount += 1;
    }

    InstanceCountedClass(const InstanceCountedClass&) {
        _instanceCount += 1;
    }

    InstanceCountedClass& operator=(const InstanceCountedClass&) {
        return *this;
    }

    InstanceCountedClass(InstanceCountedClass&&) noexcept {
        _instanceCount += 1;
    }

    InstanceCountedClass& operator=(InstanceCountedClass&&) noexcept {
        return *this;
    }

    ~InstanceCountedClass() {
        _instanceCount -= 1;
    }

    static int getInstanceCount() {
        return _instanceCount;
    }

private:
    static int _instanceCount;

    char _data[taSize] = {};
};

template <int taSize>
int InstanceCountedClass<taSize>::_instanceCount = 0;

//! Class with 2 public fields used to test emplace_back().
template <class taFieldType>
class ClassWithTwoFields {
public:
    ClassWithTwoFields(taFieldType aValue1, taFieldType aValue2)
        : field1{aValue1}
        , field2{aValue2} {}

    taFieldType field1;
    taFieldType field2;
};
} // namespace

class CompressedSmallVectorTest {
public:
    template <class T>
    void VectorIsDefaultConstructed() const {
        CompressedSmallVector<T> vec;
        EXPECT_EQ(vec.size(), 0);
        EXPECT_GE(vec.capacity(), 1);
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.begin(), vec.end());
    }

    template <class T>
    void VectorIsResizedAndSizeChecked() const {
        for (std::size_t i = 0; i < 5; i += 1) {
            CompressedSmallVector<T, 0> vec;
            vec.resize(i);
            EXPECT_EQ(vec.size(), i);
            EXPECT_EQ(vec.end() - vec.begin(), i);
        }
        for (std::size_t i = 0; i < 5; i += 1) {
            CompressedSmallVector<T, 1> vec;
            vec.resize(i);
            EXPECT_EQ(vec.size(), i);
            EXPECT_EQ(vec.end() - vec.begin(), i);
        }
        for (std::size_t i = 0; i < 5; i += 1) {
            CompressedSmallVector<T, 2> vec;
            vec.resize(i);
            EXPECT_EQ(vec.size(), i);
            EXPECT_EQ(vec.end() - vec.begin(), i);
        }
        for (std::size_t i = 0; i < 5; i += 1) {
            CompressedSmallVector<T, 3> vec;
            vec.resize(i);
            EXPECT_EQ(vec.size(), i);
            EXPECT_EQ(vec.end() - vec.begin(), i);
        }
        for (std::size_t i = 0; i < 5; i += 1) {
            CompressedSmallVector<T, 4> vec;
            vec.resize(i);
            EXPECT_EQ(vec.size(), i);
            EXPECT_EQ(vec.end() - vec.begin(), i);
        }
        for (std::size_t i = 0; i < 5; i += 1) {
            CompressedSmallVector<T, 5> vec;
            vec.resize(i);
            EXPECT_EQ(vec.size(), i);
            EXPECT_EQ(vec.end() - vec.begin(), i);
        }
    }

    template <class T>
    void OneElementIsPushedBackToZeroSizedVector(const T& aValue) const {
        CompressedSmallVector<T, 0> vec;
        EXPECT_EQ(vec.size(), 0);
        EXPECT_EQ(vec.capacity(), 0);
        EXPECT_TRUE(vec.empty());

        vec.push_back(aValue);

        EXPECT_EQ(vec.size(), 1);
        EXPECT_GE(vec.capacity(), 1);
        EXPECT_FALSE(vec.empty());
        EXPECT_LT(vec.begin(), vec.end());

        EXPECT_EQ(vec[0], aValue);
        EXPECT_EQ(vec.at(0), aValue);
        EXPECT_EQ(*vec.begin(), aValue);
        EXPECT_EQ(*vec.cbegin(), aValue);
        EXPECT_EQ(*vec.data(), aValue);
    }

    template <class T>
    void OneElementIsPushedBack(const T& aValue) const {
        CompressedSmallVector<T> vec;
        vec.push_back(aValue);

        EXPECT_EQ(vec.size(), 1);
        EXPECT_GE(vec.capacity(), 1);
        EXPECT_FALSE(vec.empty());
        EXPECT_LT(vec.begin(), vec.end());

        EXPECT_EQ(vec[0], aValue);
        EXPECT_EQ(vec.at(0), aValue);
        EXPECT_EQ(*vec.begin(), aValue);
        EXPECT_EQ(*vec.cbegin(), aValue);
        EXPECT_EQ(*vec.data(), aValue);
    }

    template <class T>
    void OneElementIsPushedBackAsRValueRef(const T& aValue) const {
        CompressedSmallVector<T> vec;
        T                        val = aValue;
        vec.push_back(std::move(val));

        EXPECT_EQ(vec.size(), 1);
        EXPECT_GE(vec.capacity(), 1);
        EXPECT_FALSE(vec.empty());
        EXPECT_LT(vec.begin(), vec.end());

        EXPECT_EQ(vec[0], aValue);
        EXPECT_EQ(vec.at(0), aValue);
        EXPECT_EQ(*vec.begin(), aValue);
        EXPECT_EQ(*vec.cbegin(), aValue);
        EXPECT_EQ(*vec.data(), aValue);
    }

    template <class T, class taFieldType>
    void OneElementIsEmplacedBack(const taFieldType& aField1, const taFieldType& aField2) const {
        CompressedSmallVector<T> vec;
        auto&                    elemRef = vec.emplace_back(aField1, aField2);

        EXPECT_EQ(&elemRef, vec.data());

        EXPECT_EQ(vec.size(), 1);
        EXPECT_GE(vec.capacity(), 1);
        EXPECT_FALSE(vec.empty());
        EXPECT_LT(vec.begin(), vec.end());

        EXPECT_EQ(vec[0].field1, aField1);
        EXPECT_EQ(vec.at(0).field2, aField2);
    }

    template <class T>
    void OneElementIsPushedBackAndThenPopped(const T& aValue) const {
        CompressedSmallVector<T, 1, growth_strategy::IncreaseByOne> vec;
        vec.push_back(aValue);
        vec.pop_back();

        EXPECT_EQ(vec.size(), 0);
        EXPECT_EQ(vec.capacity(), 1);
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.begin(), vec.end());
    }

    template <class T>
    void TwoElementsArePushedBack(const T& aValue1, const T& aValue2) const {
        CompressedSmallVector<T> vec;
        vec.push_back(aValue1);
        vec.push_back(aValue2);

        EXPECT_EQ(vec.size(), 2);
        EXPECT_GT(vec.capacity(), 1);
        EXPECT_FALSE(vec.empty());
        EXPECT_LT(vec.begin(), vec.end());

        EXPECT_EQ(vec[0], aValue1);
        EXPECT_EQ(vec.at(0), aValue1);
        EXPECT_EQ(*vec.begin(), aValue1);
        EXPECT_EQ(*vec.cbegin(), aValue1);
        EXPECT_EQ(*vec.data(), aValue1);

        EXPECT_EQ(vec[1], aValue2);
        EXPECT_EQ(vec.at(1), aValue2);
        EXPECT_EQ(*std::next(vec.begin()), aValue2);
        EXPECT_EQ(*std::next(vec.cbegin()), aValue2);
        EXPECT_EQ(*(vec.data() + 1), aValue2);
    }

    template <class T>
    void TwoElementsArePushedBackOnVectorWithLocalStore(const T& aValue1, const T& aValue2) const {
        CompressedSmallVector<T, 2> vec;
        vec.push_back(aValue1);
        vec.push_back(aValue2);

        EXPECT_EQ(vec.size(), 2);
        EXPECT_EQ(vec.capacity(), 2);
        EXPECT_FALSE(vec.empty());
        EXPECT_LT(vec.begin(), vec.end());

        EXPECT_EQ(vec[0], aValue1);
        EXPECT_EQ(vec.at(0), aValue1);
        EXPECT_EQ(*vec.begin(), aValue1);
        EXPECT_EQ(*vec.cbegin(), aValue1);
        EXPECT_EQ(*vec.data(), aValue1);

        EXPECT_EQ(vec[1], aValue2);
        EXPECT_EQ(vec.at(1), aValue2);
        EXPECT_EQ(*std::next(vec.begin()), aValue2);
        EXPECT_EQ(*std::next(vec.cbegin()), aValue2);
        EXPECT_EQ(*(vec.data() + 1), aValue2);
    }

    template <class T>
    void TwoElementsArePushedBackAndThenPopped(const T& aValue1, const T& aValue2) const {
        CompressedSmallVector<T, 1, growth_strategy::IncreaseByOne> vec;
        vec.push_back(aValue1);
        vec.push_back(aValue2);
        vec.pop_back();
        vec.pop_back();

        EXPECT_EQ(vec.size(), 0);
        EXPECT_EQ(vec.capacity(), 2);
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.begin(), vec.end());
    }

    template <class T>
    void OneElementsIsPushedBackAndThenCleared(const T& aValue1) const {
        CompressedSmallVector<T, 1, growth_strategy::IncreaseByOne> vec;
        vec.push_back(aValue1);

        vec.clear();

        EXPECT_EQ(vec.size(), 0);
        EXPECT_EQ(vec.capacity(), 1);
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.begin(), vec.end());
    }

    template <class T>
    void TwoElementsArePushedBackAndThenCleared(const T& aValue1, const T& aValue2) const {
        CompressedSmallVector<T, 1, growth_strategy::IncreaseByOne> vec;
        vec.push_back(aValue1);
        vec.push_back(aValue2);

        vec.clear();

        EXPECT_EQ(vec.size(), 0);
        EXPECT_EQ(vec.capacity(), 2);
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.begin(), vec.end());
    }

    template <class T>
    void EmptyVectorIsCopyConstructed() const {
        CompressedSmallVector<T> vec1;
        CompressedSmallVector<T> vec2{vec1};

        EXPECT_EQ(vec1.size(), 0);
        EXPECT_EQ(vec1.capacity(), 1);
        EXPECT_TRUE(vec1.empty());
        EXPECT_EQ(vec1.begin(), vec1.end());

        EXPECT_EQ(vec2.size(), 0);
        EXPECT_EQ(vec2.capacity(), 1);
        EXPECT_TRUE(vec2.empty());
        EXPECT_EQ(vec2.begin(), vec2.end());
    }

    template <class T>
    void VectorWithOneElementIsCopyConstructed(const T& aValue1) const {
        CompressedSmallVector<T> vec1;
        vec1.push_back(aValue1);

        CompressedSmallVector<T> vec2{vec1};

        EXPECT_EQ(vec1.size(), 1);
        EXPECT_EQ(vec1.capacity(), 1);
        EXPECT_FALSE(vec1.empty());
        EXPECT_LT(vec1.begin(), vec1.end());
        EXPECT_EQ(vec1[0], aValue1);

        EXPECT_EQ(vec2.size(), 1);
        EXPECT_EQ(vec2.capacity(), 1);
        EXPECT_FALSE(vec2.empty());
        EXPECT_LT(vec2.begin(), vec2.end());
        EXPECT_EQ(vec2[0], aValue1);
    }

    template <class T>
    void VectorWithTwoElementsIsCopyConstructed(const T& aValue1, const T& aValue2) const {
        CompressedSmallVector<T, 1, growth_strategy::IncreaseByOne> vec1;
        vec1.push_back(aValue1);
        vec1.push_back(aValue2);

        CompressedSmallVector<T, 1, growth_strategy::IncreaseByOne> vec2{vec1};

        EXPECT_EQ(vec1.size(), 2);
        EXPECT_EQ(vec1.capacity(), 2);
        EXPECT_FALSE(vec1.empty());
        EXPECT_LT(vec1.begin(), vec1.end());
        EXPECT_EQ(vec1[0], aValue1);
        EXPECT_EQ(vec1[1], aValue2);

        EXPECT_EQ(vec2.size(), 2);
        EXPECT_EQ(vec2.capacity(), 2);
        EXPECT_FALSE(vec2.empty());
        EXPECT_LT(vec2.begin(), vec2.end());
        EXPECT_EQ(vec2[0], aValue1);
        EXPECT_EQ(vec2[1], aValue2);
    }

    template <class T>
    void EmptyVectorIsMoveConstructed() const {
        CompressedSmallVector<T> vec1;
        CompressedSmallVector<T> vec2{std::move(vec1)};

        EXPECT_EQ(vec1.size(), 0);
        EXPECT_EQ(vec1.capacity(), 1);
        EXPECT_TRUE(vec1.empty());
        EXPECT_EQ(vec1.begin(), vec1.end());

        EXPECT_EQ(vec2.size(), 0);
        EXPECT_EQ(vec2.capacity(), 1);
        EXPECT_TRUE(vec2.empty());
        EXPECT_EQ(vec2.begin(), vec2.end());
    }

    template <class T>
    void VectorWithOneElementIsMoveConstructed(const T& aValue1) const {
        CompressedSmallVector<T> vec1;
        vec1.push_back(aValue1);

        CompressedSmallVector<T> vec2{std::move(vec1)};

        EXPECT_EQ(vec1.size(), 0);
        EXPECT_EQ(vec1.capacity(), 1);
        EXPECT_TRUE(vec1.empty());
        EXPECT_EQ(vec1.begin(), vec1.end());

        EXPECT_EQ(vec2.size(), 1);
        EXPECT_EQ(vec2.capacity(), 1);
        EXPECT_FALSE(vec2.empty());
        EXPECT_LT(vec2.begin(), vec2.end());
        EXPECT_EQ(vec2[0], aValue1);
    }

    template <class T>
    void VectorWithTwoElementsIsMoveConstructed(const T& aValue1, const T& aValue2) const {
        CompressedSmallVector<T, 1, growth_strategy::IncreaseByOne> vec1;
        vec1.push_back(aValue1);
        vec1.push_back(aValue2);

        CompressedSmallVector<T, 1, growth_strategy::IncreaseByOne> vec2{std::move(vec1)};

        EXPECT_EQ(vec1.size(), 0);
        EXPECT_EQ(vec1.capacity(), 1);
        EXPECT_TRUE(vec1.empty());
        EXPECT_EQ(vec1.begin(), vec1.end());

        EXPECT_EQ(vec2.size(), 2);
        EXPECT_EQ(vec2.capacity(), 2);
        EXPECT_FALSE(vec2.empty());
        EXPECT_LT(vec2.begin(), vec2.end());
        EXPECT_EQ(vec2[0], aValue1);
        EXPECT_EQ(vec2[1], aValue2);
    }

    template <class T, std::uint32_t taInPlaceCapacity>
    void VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted() const {
        ASSERT_EQ(T::getInstanceCount(), 0);

        {
            CompressedSmallVector<T, taInPlaceCapacity> vec1;
            EXPECT_EQ(T::getInstanceCount(), 0);
            vec1.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 1);
            vec1.pop_back();
            EXPECT_EQ(T::getInstanceCount(), 0);
            vec1.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 1);
            vec1.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 2);
            vec1.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 3);
            vec1.pop_back();
            EXPECT_EQ(T::getInstanceCount(), 2);

            CompressedSmallVector<T, taInPlaceCapacity> vec2 = vec1;
            EXPECT_EQ(T::getInstanceCount(), 4);
            vec2.erase(vec2.begin());
            EXPECT_EQ(T::getInstanceCount(), 3);

            vec1.clear();
            EXPECT_EQ(T::getInstanceCount(), 1);

            vec2.push_back({});
            vec2.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 3);

            vec2.clear();
            EXPECT_EQ(T::getInstanceCount(), 0);

            vec2.push_back({});
            vec2.push_back({});
            vec2.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 3);

            CompressedSmallVector<T, taInPlaceCapacity> vec3 = std::move(vec2);
            EXPECT_EQ(T::getInstanceCount(), 3);
        }

        ASSERT_EQ(T::getInstanceCount(), 0);
    }

    template <class T, std::uint32_t taInPlaceCapacity>
    void VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted_TrackingAllocator() const {
        ASSERT_EQ(T::getInstanceCount(), 0);

        TrackingAllocatorPool<T> allocPool;

        using CSVector =
            CompressedSmallVector<T, taInPlaceCapacity, growth_strategy::Default, TrackingAllocator<T>>;

        {
            CSVector vec1{TrackingAllocator<T>{allocPool}};
            EXPECT_EQ(T::getInstanceCount(), 0);
            vec1.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 1);
            vec1.pop_back();
            EXPECT_EQ(T::getInstanceCount(), 0);
            vec1.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 1);
            vec1.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 2);
            vec1.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 3);
            vec1.pop_back();
            EXPECT_EQ(T::getInstanceCount(), 2);

            CSVector vec2 = vec1;
            EXPECT_EQ(T::getInstanceCount(), 4);
            vec2.erase(vec2.begin());
            EXPECT_EQ(T::getInstanceCount(), 3);

            vec1.clear();
            EXPECT_EQ(T::getInstanceCount(), 1);

            vec2.push_back({});
            vec2.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 3);

            vec2.clear();
            EXPECT_EQ(T::getInstanceCount(), 0);

            vec2.push_back({});
            vec2.push_back({});
            vec2.push_back({});
            EXPECT_EQ(T::getInstanceCount(), 3);

            CSVector vec3 = std::move(vec2);
            EXPECT_EQ(T::getInstanceCount(), 3);
        }

        ASSERT_EQ(T::getInstanceCount(), 0);
        ASSERT_EQ(allocPool.HasNoUnfreedMemory(), true);
    }

    template <class T>
    void CopyAssignmentOperatorUsedInMultipleCases_TrackingAllocator(const T& aValue1,
                                                                     const T& aValue2) const {
        TrackingAllocatorPool<T> allocPool;
        TrackingAllocatorPool<T> allocPool2;

        using CSVector = CompressedSmallVector<T, 1, growth_strategy::Default, TrackingAllocator<T>>;

        {
            SCOPED_TRACE("Copying empty vector.");

            CSVector vec1{TrackingAllocator<T>{allocPool}};
            CSVector vec2{TrackingAllocator<T>{allocPool2}};
            vec2 = vec1;

            vec1.emplace_back();
            vec1.emplace_back();
            EXPECT_EQ(vec1.size(), 2);

            vec2.emplace_back();
            vec2.emplace_back();
            EXPECT_EQ(vec2.size(), 2);

            ASSERT_EQ(allocPool2.HasNoUnfreedMemory(), true);
        }

        {
            SCOPED_TRACE("Copying vector with 1 element.");

            CSVector vec1{TrackingAllocator<T>{allocPool}};
            vec1.push_back(aValue1);

            CSVector vec2{TrackingAllocator<T>{allocPool2}};
            vec2 = vec1;

            ASSERT_EQ(vec1.size(), 1);
            ASSERT_EQ(vec2.size(), 1);
            EXPECT_EQ(vec1[0], aValue1);
            EXPECT_EQ(vec2[0], aValue1);

            vec1.push_back(aValue1);
            EXPECT_EQ(vec1.size(), 2);

            vec2.push_back(aValue1);
            EXPECT_EQ(vec2.size(), 2);

            ASSERT_EQ(allocPool2.HasNoUnfreedMemory(), true);
        }

        {
            SCOPED_TRACE("Copying vector with 2 elements.");

            CSVector vec1{TrackingAllocator<T>{allocPool}};
            vec1.push_back(aValue1);
            vec1.push_back(aValue2);

            CSVector vec2{TrackingAllocator<T>{allocPool2}};
            vec2 = vec1;

            ASSERT_EQ(vec1.size(), 2);
            ASSERT_EQ(vec2.size(), 2);
            EXPECT_EQ(vec1[0], aValue1);
            EXPECT_EQ(vec1[1], aValue2);
            EXPECT_EQ(vec2[0], aValue1);
            EXPECT_EQ(vec2[1], aValue2);

            vec1.push_back(aValue1);
            EXPECT_EQ(vec1.size(), 3);

            vec2.push_back(aValue1);
            EXPECT_EQ(vec2.size(), 3);

            ASSERT_EQ(allocPool2.HasNoUnfreedMemory(), true);
        }

        ASSERT_EQ(allocPool.HasNoUnfreedMemory(), true);
    }

    template <class T>
    void MoveAssignmentOperatorUsedInMultipleCases_TrackingAllocator(const T& aValue1,
                                                                     const T& aValue2) const {
        TrackingAllocatorPool<T> allocPool;
        TrackingAllocatorPool<T> allocPool2;

        using CSVector = CompressedSmallVector<T, 1, growth_strategy::Default, TrackingAllocator<T>>;

        {
            SCOPED_TRACE("Moving empty vector.");

            CSVector vec1{TrackingAllocator<T>{allocPool}};
            CSVector vec2{TrackingAllocator<T>{allocPool2}};
            vec2 = std::move(vec1);

            vec1.emplace_back();
            vec1.emplace_back();
            EXPECT_EQ(vec1.size(), 2);

            vec2.emplace_back();
            vec2.emplace_back();
            EXPECT_EQ(vec2.size(), 2);

            ASSERT_EQ(allocPool2.HasNoUnfreedMemory(), true);
        }

        {
            SCOPED_TRACE("Moving vector with 1 element.");

            CSVector vec1{TrackingAllocator<T>{allocPool}};
            vec1.push_back(aValue1);

            CSVector vec2{TrackingAllocator<T>{allocPool2}};
            vec2 = std::move(vec1);

            ASSERT_EQ(vec1.size(), 0);
            ASSERT_EQ(vec2.size(), 1);
            EXPECT_EQ(vec2[0], aValue1);

            vec1.push_back(aValue1);
            vec1.push_back(aValue1);
            EXPECT_EQ(vec1.size(), 2);

            vec2.push_back(aValue1);
            EXPECT_EQ(vec2.size(), 2);

            ASSERT_EQ(allocPool2.HasNoUnfreedMemory(), true);
        }

        {
            SCOPED_TRACE("Moving vector with 2 elements.");

            CSVector vec1{TrackingAllocator<T>{allocPool}};
            vec1.push_back(aValue1);
            vec1.push_back(aValue2);

            CSVector vec2{TrackingAllocator<T>{allocPool2}};
            vec2 = std::move(vec1);

            ASSERT_EQ(vec1.size(), 0);
            ASSERT_EQ(vec2.size(), 2);
            EXPECT_EQ(vec2[0], aValue1);
            EXPECT_EQ(vec2[1], aValue2);

            vec1.push_back(aValue1);
            vec1.push_back(aValue1);
            EXPECT_EQ(vec1.size(), 2);

            vec2.push_back(aValue1);
            EXPECT_EQ(vec2.size(), 3);

            ASSERT_EQ(allocPool2.HasNoUnfreedMemory(), true);
        }

        ASSERT_EQ(allocPool.HasNoUnfreedMemory(), true);
    }

    template <class T>
    void ElementIsErasedFromVectorWithOneElement(const T& aValue) const {
        CompressedSmallVector<T> vec;
        vec.push_back(aValue);

        vec.erase(vec.begin());

        EXPECT_EQ(vec.size(), 0);
        EXPECT_GE(vec.capacity(), 1);
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.begin(), vec.end());
    }

    template <class T>
    void ElementIsErasedFromVectorWithThreeElements(const T& aValue1, const T& aValue2) const {
        CompressedSmallVector<T> vec;
        vec.push_back(aValue1);
        vec.push_back(aValue2);
        vec.push_back(aValue1);

        vec.erase(vec.begin() + 1);

        ASSERT_EQ(vec.size(), 2);
        EXPECT_EQ(vec[0], aValue1);
        EXPECT_EQ(vec[1], aValue1);
    }
};

#define INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(_test_name_, _type_, ...) \
    TEST(CompressedSmallVectorTest, _test_name_##_##_type_) {            \
        CompressedSmallVectorTest{}._test_name_<_type_>(__VA_ARGS__);    \
    }

#define INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(_test_name_, _type_, _in_place_cap_, ...) \
    TEST(CompressedSmallVectorTest, _test_name_##_##_type_##_x##_in_place_cap_) {            \
        CompressedSmallVectorTest{}._test_name_<_type_, _in_place_cap_>(__VA_ARGS__);        \
    }

#define NO_ARGS

using String               = std::string;
const String LONG_STRING_1 = "Big Trouble in Little China";
const String LONG_STRING_2 = "...besides that, it's all in the reflexes!";

// clang-format off
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorIsDefaultConstructed,
    short,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorIsDefaultConstructed,
    double,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorIsDefaultConstructed,
    String,
    NO_ARGS)

INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorIsResizedAndSizeChecked,
    short,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorIsResizedAndSizeChecked,
    double,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorIsResizedAndSizeChecked,
    String,
    NO_ARGS)

INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBackToZeroSizedVector,
    short,
    5)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBackToZeroSizedVector,
    double,
    5.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBackToZeroSizedVector,
    String,
    LONG_STRING_1)

INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBack,
    short,
    5)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBack,
    double,
    5.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBack,
    String,
    LONG_STRING_1)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBackAsRValueRef,
    short,
    5)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBackAsRValueRef,
    double,
    5.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBackAsRValueRef,
    String,
    LONG_STRING_1)

    
using ClassWithTwoFields_char   = ClassWithTwoFields<char>;
using ClassWithTwoFields_float  = ClassWithTwoFields<float>;
using ClassWithTwoFields_String = ClassWithTwoFields<String>;
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsEmplacedBack,
    ClassWithTwoFields_char,
    (char)'5', (char)'6')
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsEmplacedBack,
    ClassWithTwoFields_float,
    5.f, 6.f)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsEmplacedBack,
    ClassWithTwoFields_String,
    LONG_STRING_1, LONG_STRING_2)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBackAndThenPopped,
    short,
    5)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBackAndThenPopped,
    double,
    5.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBackAndThenPopped,
    String,
    LONG_STRING_1)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBack,
    short,
    5, 6)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBack,
    double,
    5.0, 6.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBack,
    String,
    LONG_STRING_1, LONG_STRING_2)

INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBackOnVectorWithLocalStore,
    short,
    5, 6)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBackOnVectorWithLocalStore,
    double,
    5.0, 6.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBackOnVectorWithLocalStore,
    String,
    LONG_STRING_1, LONG_STRING_2)

INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBackAndThenPopped,
    short,
    5, 6)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBackAndThenPopped,
    double,
    5.0, 6.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBackAndThenPopped,
    String,
    LONG_STRING_1, LONG_STRING_2)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementsIsPushedBackAndThenCleared,
    short,
    5)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementsIsPushedBackAndThenCleared,
    double,
    5.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementsIsPushedBackAndThenCleared,
    String,
    LONG_STRING_1)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBackAndThenCleared,
    short,
    5, 6)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBackAndThenCleared,
    double,
    5.0, 6.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    TwoElementsArePushedBackAndThenCleared,
    String,
    LONG_STRING_1, LONG_STRING_2)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    EmptyVectorIsCopyConstructed,
    short,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    EmptyVectorIsCopyConstructed,
    double,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    EmptyVectorIsCopyConstructed,
    String,
    NO_ARGS)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithOneElementIsCopyConstructed,
    short,
    5)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithOneElementIsCopyConstructed,
    double,
    5.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithOneElementIsCopyConstructed,
    String,
    LONG_STRING_1)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithTwoElementsIsCopyConstructed,
    short,
    5, 6)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithTwoElementsIsCopyConstructed,
    double,
    5.0, 6.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithTwoElementsIsCopyConstructed,
    String,
    LONG_STRING_1, LONG_STRING_2)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    EmptyVectorIsMoveConstructed,
    short,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    EmptyVectorIsMoveConstructed,
    double,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    EmptyVectorIsMoveConstructed,
    String,
    NO_ARGS)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithOneElementIsMoveConstructed,
    short,
    5)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithOneElementIsMoveConstructed,
    double,
    5.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithOneElementIsMoveConstructed,
    String,
    LONG_STRING_1)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithTwoElementsIsMoveConstructed,
    short,
    5, 6)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithTwoElementsIsMoveConstructed,
    double,
    5.0, 6.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithTwoElementsIsMoveConstructed,
    String,
    LONG_STRING_1, LONG_STRING_2)


using InstanceCountedClass2  = InstanceCountedClass<2>;
using InstanceCountedClass12 = InstanceCountedClass<12>;
using InstanceCountedClass20 = InstanceCountedClass<20>;


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass2,
    0,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass12,
    0,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass20,
    0,
    NO_ARGS)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass2,
    1,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass12,
    1,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass20,
    1,
    NO_ARGS)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass2,
    2,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass12,
    2,
    NO_ARGS)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass2,
    4,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass12,
    4,
    NO_ARGS)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted_TrackingAllocator,
    InstanceCountedClass2,
    0,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted_TrackingAllocator,
    InstanceCountedClass12,
    0,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted_TrackingAllocator,
    InstanceCountedClass20,
    0,
    NO_ARGS)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted_TrackingAllocator,
    InstanceCountedClass2,
    1,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted_TrackingAllocator,
    InstanceCountedClass12,
    1,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted_TrackingAllocator,
    InstanceCountedClass20,
    1,
    NO_ARGS)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted_TrackingAllocator,
    InstanceCountedClass2,
    2,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted_TrackingAllocator,
    InstanceCountedClass12,
    2,
    NO_ARGS)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted_TrackingAllocator,
    InstanceCountedClass2,
    4,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST_CAP(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted_TrackingAllocator,
    InstanceCountedClass12,
    4,
    NO_ARGS)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    CopyAssignmentOperatorUsedInMultipleCases_TrackingAllocator,
    short,
    5, 6)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    CopyAssignmentOperatorUsedInMultipleCases_TrackingAllocator,
    double,
    5.0, 6.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    CopyAssignmentOperatorUsedInMultipleCases_TrackingAllocator,
    String,
    LONG_STRING_1, LONG_STRING_2)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    MoveAssignmentOperatorUsedInMultipleCases_TrackingAllocator,
    short,
    5, 6)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    MoveAssignmentOperatorUsedInMultipleCases_TrackingAllocator,
    double,
    5.0, 6.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    MoveAssignmentOperatorUsedInMultipleCases_TrackingAllocator,
    String,
    LONG_STRING_1, LONG_STRING_2)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    ElementIsErasedFromVectorWithOneElement,
    short,
    5)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    ElementIsErasedFromVectorWithOneElement,
    double,
    5.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    ElementIsErasedFromVectorWithOneElement,
    String,
    LONG_STRING_1)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    ElementIsErasedFromVectorWithThreeElements,
    short,
    5, 6)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    ElementIsErasedFromVectorWithThreeElements,
    double,
    5.0, 6.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    ElementIsErasedFromVectorWithThreeElements,
    String,
    LONG_STRING_1, LONG_STRING_2)
// clang-format on

TEST(CompressedSmallVectorTest, VectorWithZeroInPlaceCapacityAllocatesAndDeallocatesWhenExpected) {
    CompressedSmallVector<char, 0, growth_strategy::IncreaseByOne> vec;
    EXPECT_EQ(vec.capacity(), 0);
    EXPECT_EQ(vec.begin(), vec.end());

    vec.push_back('a');
    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec.capacity(), 1);
    EXPECT_NE(static_cast<void*>(&vec), static_cast<void*>(vec.data()));

    vec.push_back('b');
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.capacity(), 2);
    EXPECT_NE(static_cast<void*>(&vec), static_cast<void*>(vec.data()));

    // Check that clearing keeps the heap buffer and capacity
    vec.clear();
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 2);
    EXPECT_NE(static_cast<void*>(&vec), static_cast<void*>(vec.data()));
}

TEST(CompressedSmallVectorTest, VectorWithMultipleInPlaceCapacityAllocatesAndDeallocatesWhenExpected) {
    CompressedSmallVector<char, 3, growth_strategy::IncreaseByOne> vec;
    EXPECT_EQ(vec.capacity(), 3);
    EXPECT_EQ(vec.begin(), vec.end());

    vec.push_back('a');
    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec.capacity(), 3);
    EXPECT_EQ(static_cast<void*>(&vec), static_cast<void*>(vec.data()));

    vec.push_back('b');
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.capacity(), 3);
    EXPECT_EQ(static_cast<void*>(&vec), static_cast<void*>(vec.data()));

    vec.push_back('c');
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec.capacity(), 3);
    EXPECT_EQ(static_cast<void*>(&vec), static_cast<void*>(vec.data()));

    // At this point we expect to allocate space on the heap
    vec.push_back('d');
    EXPECT_EQ(vec.size(), 4);
    EXPECT_EQ(vec.capacity(), 4);
    EXPECT_NE(static_cast<void*>(&vec), static_cast<void*>(vec.data()));

    // Check that clearing keeps the heap buffer and capacity
    vec.clear();
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 4);
    EXPECT_NE(static_cast<void*>(&vec), static_cast<void*>(vec.data()));
}

} // namespace util
} // namespace hobgoblin
} // namespace jbatnozic
