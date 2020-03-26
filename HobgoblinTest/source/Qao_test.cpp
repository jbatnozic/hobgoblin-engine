
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/Utility/Exceptions.hpp>

#include <gtest/gtest.h>
#include <vector>

using namespace hg::qao;

#define TYPEID_SELF typeid(decltype(*this))

class QAO_Test : public ::testing::Test {
protected:
    QAO_Test()
        : _runtime{}
    {
    }

    void performStep() {
        _runtime.startStep();
        bool done = false;
        _runtime.advanceStep(done);
        ASSERT_TRUE(done);
    }

    std::vector<int> _numbers;
    QAO_Runtime _runtime;
};

class SimpleActiveObject : public QAO_Base {
public:
    SimpleActiveObject(QAO_Runtime* runtime, std::vector<int>& vec, int number)
        : QAO_Base{runtime, TYPEID_SELF, 0, "SimpleActiveObject"}
        , _myVec{vec}
        , _myNumber{number}
    {
    }

    using QAO_Base::setExecutionPriority;

    void eventUpdate() override {
        _myVec.push_back(_myNumber);
    }

private:
    std::vector<int>& _myVec;
    int _myNumber;
};

TEST_F(QAO_Test, ObjectCount) {
    auto obj = QAO_PCreate<SimpleActiveObject>(&_runtime, _numbers, 0);
    ASSERT_EQ(_runtime.getObjectCount(), 1);
    QAO_PDestroy(obj);
    ASSERT_EQ(_runtime.getObjectCount(), 0);
}

TEST_F(QAO_Test, SimpleEvent) {
    constexpr int VALUE_0 = 1;
    auto obj = QAO_PCreate<SimpleActiveObject>(&_runtime, _numbers, VALUE_0);
    performStep();
    ASSERT_EQ(_numbers.size(), 1u);
    ASSERT_EQ(_numbers[0], VALUE_0);
}

TEST_F(QAO_Test, Ordering) {
    constexpr int VALUE_0 = 1;
    constexpr int VALUE_1 = 2;
    constexpr int VALUE_2 = 3;
    auto obj0 = QAO_PCreate<SimpleActiveObject>(&_runtime, _numbers, VALUE_0);
    auto obj1 = QAO_PCreate<SimpleActiveObject>(&_runtime, _numbers, VALUE_1);
    auto obj2 = QAO_PCreate<SimpleActiveObject>(&_runtime, _numbers, VALUE_2);

    obj0->setExecutionPriority(80);
    obj1->setExecutionPriority(70);
    obj2->setExecutionPriority(60);

    performStep();
    ASSERT_EQ(_numbers.size(), 3u);
    ASSERT_EQ(_numbers[0], VALUE_0);
    ASSERT_EQ(_numbers[1], VALUE_1);
    ASSERT_EQ(_numbers[2], VALUE_2);

    _numbers.clear();

    // Reverse order
    obj0->setExecutionPriority(60);
    obj1->setExecutionPriority(70);
    obj2->setExecutionPriority(80);

    performStep();
    ASSERT_EQ(_numbers.size(), 3u);
    ASSERT_EQ(_numbers[0], VALUE_2);
    ASSERT_EQ(_numbers[1], VALUE_1);
    ASSERT_EQ(_numbers[2], VALUE_0);
}

TEST_F(QAO_Test, PCreatePDestroy) {
    auto obj = QAO_PCreate<SimpleActiveObject>(&_runtime, _numbers, 0);
    ASSERT_EQ(_runtime.getObjectCount(), 1);

    QAO_PDestroy(obj);
    ASSERT_EQ(_runtime.getObjectCount(), 0);
}

TEST_F(QAO_Test, PCreateFails) {
    EXPECT_THROW(QAO_PCreate<SimpleActiveObject>(nullptr, _numbers, 0), hg::util::TracedLogicError);
}

TEST_F(QAO_Test, UPCreateTest) {
    auto obj = QAO_UPCreate<SimpleActiveObject>(nullptr, _numbers, 0);
    ASSERT_EQ(_runtime.getObjectCount(), 0);
    ASSERT_EQ(obj->getRuntime(), nullptr);

    _runtime.addObjectNoOwn(*obj);
    ASSERT_EQ(_runtime.getObjectCount(), 1);
    ASSERT_EQ(obj->getRuntime(), &_runtime);

    QAO_UPDestroy(std::move(obj));
    ASSERT_EQ(_runtime.getObjectCount(), 0);
}

TEST_F(QAO_Test, UPCreateFails) {
    EXPECT_THROW(QAO_UPCreate<SimpleActiveObject>(&_runtime, _numbers, 0), hg::util::TracedLogicError);
}

TEST_F(QAO_Test, ICreate) {
    auto id = QAO_ICreate<SimpleActiveObject>(&_runtime, _numbers, 0);
    ASSERT_EQ(_runtime.getObjectCount(), 1);

    auto* const obj = _runtime.find(id);
    ASSERT_NE(obj, nullptr);
    ASSERT_EQ(obj->getRuntime(), &_runtime);

    QAO_IDestroy(_runtime, id);
    ASSERT_EQ(_runtime.getObjectCount(), 0);
}

TEST_F(QAO_Test, ICreateFails) {
    EXPECT_THROW(QAO_ICreate<SimpleActiveObject>(nullptr, _numbers, 0), hg::util::TracedLogicError);
}

TEST_F(QAO_Test, ReleaseObject) {
    auto obj = QAO_PCreate<SimpleActiveObject>(&_runtime, _numbers, 0);
    auto upObj = _runtime.releaseObject(obj);
    ASSERT_EQ(_runtime.getObjectCount(), 0);
    ASSERT_NE(upObj.get(), nullptr);
    ASSERT_EQ(upObj->getRuntime(), nullptr);
    ASSERT_NO_THROW(upObj.reset());
}

TEST_F(QAO_Test, ReleaseObjectFails) {
    auto upObj = QAO_UPCreate<SimpleActiveObject>(nullptr, _numbers, 0);
    _runtime.addObjectNoOwn(*upObj);
    
    auto upReleased = _runtime.releaseObject(upObj.get());
    ASSERT_EQ(upReleased.get(), nullptr);
}

TEST_F(QAO_Test, NullIdEquality) {
    QAO_GenericId id1{};
    QAO_GenericId id2{nullptr};
    ASSERT_EQ(id1, id2);
}

TEST_F(QAO_Test, NullIdFindsNullptr) {
    QAO_GenericId nullId{};
    ASSERT_EQ(_runtime.find(nullId), nullptr);
}

TEST_F(QAO_Test, PriorityResolverBasics) {
    enum Categories {
        A, B, C
    };
    
    QAO_PriorityResolver resolver;
    resolver.category(A);
    resolver.category(B).dependsOn(A);
    resolver.category(C).dependsOn(A, B);
    resolver.resolveAll();

    ASSERT_GT(resolver.getPriorityOf(A), resolver.getPriorityOf(B)); // A before B
    ASSERT_GT(resolver.getPriorityOf(B), resolver.getPriorityOf(C)); // B before C
}

TEST_F(QAO_Test, PriorityResolverReversedInput) {
    enum Categories {
        A, B, C
    };

    QAO_PriorityResolver resolver;
    resolver.category(C).dependsOn(A, B);
    resolver.category(B).dependsOn(A);
    resolver.category(A);
    
    resolver.resolveAll();

    ASSERT_GT(resolver.getPriorityOf(A), resolver.getPriorityOf(B)); // A before B
    ASSERT_GT(resolver.getPriorityOf(B), resolver.getPriorityOf(C)); // B before C
}

TEST_F(QAO_Test, PriorityResolverImpossibleCycle) {
    enum Categories {
        A, B, C
    };

    QAO_PriorityResolver resolver;
    resolver.category(A).dependsOn(C);
    resolver.category(B).dependsOn(A);
    resolver.category(C).dependsOn(B);

    ASSERT_THROW(resolver.resolveAll(), hg::util::TracedLogicError);
}

TEST_F(QAO_Test, PriorityResolverMissingDefinition) {
    enum Categories {
        A, B
    };

    QAO_PriorityResolver resolver;
    resolver.category(A).dependsOn(B);

    ASSERT_THROW(resolver.resolveAll(), hg::util::TracedLogicError);
}

TEST_F(QAO_Test, PriorityResolverComplexResolve) {
    enum Categories {
        A, B, C, D
    };

    QAO_PriorityResolver resolver;
    resolver.category(B);
    resolver.category(C).dependsOn(B);
    resolver.category(A).dependsOn(B, C);
    resolver.category(D).dependsOn(C, A);

    resolver.resolveAll();

    ASSERT_GT(resolver.getPriorityOf(B), resolver.getPriorityOf(C)); // B before C
    ASSERT_GT(resolver.getPriorityOf(C), resolver.getPriorityOf(A)); // C before A
    ASSERT_GT(resolver.getPriorityOf(A), resolver.getPriorityOf(D)); // A before D
}