
#include <Hobgoblin_include/QAO.hpp>

#include <gtest/gtest.h>
#include <vector>

using namespace jbatnozic::hobgoblin::qao;

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
    SimpleActiveObject(std::vector<int>& vec, int number)
        : QAO_Base{0, 0, "SimpleActiveObject"}
        , _myVec{vec}
        , _myNumber{number}
    {
    }

    using QAO_Base::setExecutionPriority;

    void eventUpdate(QAO_Runtime& rt) override {
        _myVec.push_back(_myNumber);
    }

private:
    std::vector<int>& _myVec;
    int _myNumber;
};

TEST_F(QAO_Test, ObjectCountTest) {
    auto obj = QAO_Create<SimpleActiveObject>(_runtime, _numbers, 0);
    ASSERT_EQ(_runtime.getObjectCount(), 1);
    QAO_Destroy(obj);
    ASSERT_EQ(_runtime.getObjectCount(), 0);
}

TEST_F(QAO_Test, SimpleEventTest) {
    constexpr int VALUE_0 = 1;
    auto obj = QAO_Create<SimpleActiveObject>(_runtime, _numbers, VALUE_0);
    performStep();
    ASSERT_EQ(_numbers.size(), 1u);
    ASSERT_EQ(_numbers[0], VALUE_0);
}

TEST_F(QAO_Test, OrderingTest) {
    constexpr int VALUE_0 = 1;
    constexpr int VALUE_1 = 2;
    constexpr int VALUE_2 = 3;
    auto obj0 = QAO_Create<SimpleActiveObject>(_runtime, _numbers, VALUE_0);
    auto obj1 = QAO_Create<SimpleActiveObject>(_runtime, _numbers, VALUE_1);
    auto obj2 = QAO_Create<SimpleActiveObject>(_runtime, _numbers, VALUE_2);

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

TEST_F(QAO_Test, DestroyTest) {
    auto obj = QAO_Create<SimpleActiveObject>(_runtime, _numbers, 0);
    ASSERT_EQ(_runtime.getObjectCount(), 1);

    QAO_Destroy(obj);
    ASSERT_EQ(_runtime.getObjectCount(), 0);
}