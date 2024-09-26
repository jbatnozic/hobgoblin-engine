// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Compressed_small_vector.hpp>

#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector> // TODO: remove

#include <gtest/gtest.h>

namespace jbatnozic {
namespace hobgoblin {
namespace util {

// clang-format off
static_assert(std::is_same_v<detail::CompressedSmallVectorStorageSelector<int>::type,
              detail::CompressedSmallVectorStorage_1<int>>);
static_assert(sizeof(CompressedSmallVector<int>) == sizeof(void*));

static_assert(std::is_same_v<detail::CompressedSmallVectorStorageSelector<double>::type,
              detail::CompressedSmallVectorStorage_2<double>>);
static_assert(sizeof(CompressedSmallVector<double>) == sizeof(void*) * 2);

static_assert(std::is_same_v<detail::CompressedSmallVectorStorageSelector<std::string>::type,
              detail::CompressedSmallVectorStorage_3<std::string>>);
static_assert(sizeof(CompressedSmallVector<std::string>) <= sizeof(std::string) + sizeof(void*));
// clang-format on

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

    static PZInteger getInstanceCount() {
        return _instanceCount;
    }

private:
    inline static PZInteger _instanceCount = 0;

    char _data[taSize] = {};
};

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

TEST(CompressedSmallVectorTest, Endianess) {
    {
        const auto host     = detail::BigEndianToHost64(0x12345678ABCD1234ULL);
        const auto expected = 0x3412CDAB78563412ULL;
        EXPECT_EQ(host, expected);
    }
    {
        const auto bigEndian = detail::HostToBigEndian64(0x3412CDAB78563412ULL);
        const auto expected  = 0x12345678ABCD1234ULL;
        EXPECT_EQ(bigEndian, expected);
    }
}

class CompressedSmallVectorTest {
public:
    template <class T>
    void VectorIsDefaultConstructed() {
        CompressedSmallVector<T> vec;
        EXPECT_EQ(vec.size(), 0);
        EXPECT_GE(vec.capacity(), 1);
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.begin(), vec.end());
    }

    template <class T>
    void OneElementIsPushedBack(const T& aValue) {
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
    void OneElementIsPushedBackAsRValueRef(const T& aValue) {
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
    void OneElementIsEmplacedBack(const taFieldType& aField1, const taFieldType& aField2) {
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
    void OneElementIsPushedBackAndThenPopped(const T& aValue) {
        CompressedSmallVector<T, growth_strategy::IncreaseByOne> vec;
        vec.push_back(aValue);
        vec.pop_back();

        EXPECT_EQ(vec.size(), 0);
        EXPECT_EQ(vec.capacity(), 1);
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.begin(), vec.end());
    }

    template <class T>
    void TwoElementsArePushedBack(const T& aValue1, const T& aValue2) {
        CompressedSmallVector<T> vec;
        vec.push_back(aValue1);
        vec.push_back(aValue2);

        EXPECT_EQ(vec.size(), 2);
        EXPECT_GE(vec.capacity(), 1);
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
    void TwoElementsArePushedBackAndThenPopped(const T& aValue1, const T& aValue2) {
        CompressedSmallVector<T, growth_strategy::IncreaseByOne> vec;
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
    void TwoElementsArePushedBackAndThenCleared(const T& aValue1, const T& aValue2) {
        CompressedSmallVector<T, growth_strategy::IncreaseByOne> vec;
        vec.push_back(aValue1);
        vec.push_back(aValue2);

        vec.clear();

        EXPECT_EQ(vec.size(), 0);
        EXPECT_EQ(vec.capacity(), 2);
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.begin(), vec.end());
    }

    template <class T>
    void EmptyVectorIsCopyConstructed() {
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
    void VectorWithOneElementIsCopyConstructed(const T& aValue1) {
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
    void VectorWithTwoElementsIsCopyConstructed(const T& aValue1, const T& aValue2) {
        CompressedSmallVector<T, growth_strategy::IncreaseByOne> vec1;
        vec1.push_back(aValue1);
        vec1.push_back(aValue2);

        CompressedSmallVector<T, growth_strategy::IncreaseByOne> vec2{vec1};

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
    void EmptyVectorIsMoveConstructed() {
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
    void VectorWithOneElementIsMoveConstructed(const T& aValue1) {
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
    void VectorWithTwoElementsIsMoveConstructed(const T& aValue1, const T& aValue2) {
        CompressedSmallVector<T, growth_strategy::IncreaseByOne> vec1;
        vec1.push_back(aValue1);
        vec1.push_back(aValue2);

        CompressedSmallVector<T, growth_strategy::IncreaseByOne> vec2{std::move(vec1)};

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

    template <class T>
    void VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted() {
        ASSERT_EQ(T::getInstanceCount(), 0);

        {
            CompressedSmallVector<T> vec1;
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

            CompressedSmallVector<T> vec2 = vec1;
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

            CompressedSmallVector<T> vec3 = std::move(vec2);
            EXPECT_EQ(T::getInstanceCount(), 3);
        }

        ASSERT_EQ(T::getInstanceCount(), 0);
    }

    template <class T>
    void ElementIsErasedFromVectorWithOneElement(const T& aValue) {
        CompressedSmallVector<T> vec;
        vec.push_back(aValue);

        vec.erase(vec.begin());

        EXPECT_EQ(vec.size(), 0);
        EXPECT_GE(vec.capacity(), 1);
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.begin(), vec.end());
    }

    template <class T>
    void ElementIsErasedFromVectorWithThreeElements(const T& aValue1, const T& aValue2) {
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

#define NO_ARGS

using String = std::string;

const String LONG_STRING_1 = " Hawk the Slayer (1980)";
const String LONG_STRING_2 = " Adventures of Buckaroo Bansai Across The 8th Dimension";

// clang-format off
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorIsDefaultConstructed,
    int,
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
    OneElementIsPushedBack,
    int,
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
    int,
    5)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBackAsRValueRef,
    double,
    5.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    OneElementIsPushedBackAsRValueRef,
    String,
    LONG_STRING_1)


using ClassWithTwoFields_char = ClassWithTwoFields<char>;
using ClassWithTwoFields_float = ClassWithTwoFields<float>;
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
    int,
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
    int,
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
    TwoElementsArePushedBackAndThenPopped,
    int,
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
    TwoElementsArePushedBackAndThenCleared,
    int,
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
    int,
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
    int,
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
    int,
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
    int,
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
    int,
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
    int,
    5, 6)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithTwoElementsIsMoveConstructed,
    double,
    5.0, 6.0)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorWithTwoElementsIsMoveConstructed,
    String,
    LONG_STRING_1, LONG_STRING_2)

using InstanceCountedClass4 = InstanceCountedClass<4>;
using InstanceCountedClass12 = InstanceCountedClass<12>;
using InstanceCountedClass20 = InstanceCountedClass<20>;
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass4,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass12,
    NO_ARGS)
INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    VectorIsMutatedManyTimesThenDestroyed_InstancesOfElementTypeCounted,
    InstanceCountedClass20,
    NO_ARGS)


INSTANTIATE_COMPRESSEDSMALLVECTOR_TEST(
    ElementIsErasedFromVectorWithOneElement,
    int,
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
    int,
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

} // namespace util
} // namespace hobgoblin
} // namespace jbatnozic
