
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Serialization.hpp>
#include <Hobgoblin/Preprocessor.hpp>

#include <gtest/gtest.h>
#include <vector>

using namespace hg::qao;

#define SELF (*this)
#define TYPEID_SELF typeid(decltype(*this))
#define MESSAGE_SERIALIZE 1

class Dummy : public QAO_Base, public hg::util::PolymorphicSerializable {
public:
    static constexpr auto SERIALIZABLE_TAG = "ST_DUMMY";

    Dummy(QAO_RuntimeRef rtRef, int value1, int value2)
        : QAO_Base{rtRef, TYPEID_SELF, 0, "Dummy"}
        , _value1{value1}
        , _value2{value2}
    {
    }

    Dummy(QAO_RuntimeRef rtRef, hg::util::PacketBase& packet)
        : QAO_Base{rtRef, TYPEID_SELF, packet}
    {
        packet >> SELF;
    }

    void serialize(hg::util::PacketBase& packet) const override {
        packet << static_cast<const QAO_Base&>(SELF);
        packet << SELF;
    }

    static void deserialize(hg::util::PacketBase& packet, hg::util::AnyPtr context, int /*contextTag*/) {
        QAO_Runtime* runtime = context.getOrThrow<QAO_Runtime>();

        QAO_PCreate<Dummy>(runtime, packet);
    }

    virtual std::string getSerializableTag() const {
        return SERIALIZABLE_TAG;
    }

    HG_ENABLE_AUTOPACK(Dummy, _value1, _value2);

//private:
    int _value1, _value2;
};

HG_PP_DO_BEFORE_MAIN(RegisterDemmySerializable) {
    hg::util::RegisterSerializable<Dummy>();
}

class QAO_SerializationTest : public ::testing::Test {
public:

    void performStep() {
        _runtime.startStep();
        bool done = false;
        _runtime.advanceStep(done);
        ASSERT_TRUE(done);
    }

    void continueStep() {
        bool done = false;
        _runtime.advanceStep(done);
        ASSERT_TRUE(done);
    }

protected:
    QAO_Runtime _runtime;
    hg::util::Packet _packet;
};

TEST_F(QAO_SerializationTest, TestWithoutEvents) {
    auto dummy1 = QAO_PCreate<Dummy>(&_runtime, 1, 1001);
    auto dummy2 = QAO_PCreate<Dummy>(&_runtime, 2, 1002);

    QAO_Id<Dummy> id1{dummy1};
    QAO_Id<Dummy> id2{dummy2};

    ASSERT_EQ(_runtime.getObjectCount(), 2);

    _packet << _runtime;
    hg::util::Serialize(_packet, *dummy1);
    hg::util::Serialize(_packet, *dummy2);

    _runtime.destroyAllOwnedObjects();

    ASSERT_EQ(_runtime.getObjectCount(), 0);

    _packet >> _runtime;
    hg::util::Deserialize(_packet, &_runtime);

    ASSERT_EQ(_runtime.getObjectCount(), 2);

    auto dummy1New = _runtime.find(id1);
    auto dummy2New = _runtime.find(id2);

    ASSERT_NE(dummy1New, nullptr);
    ASSERT_NE(dummy2New, nullptr);

    ASSERT_EQ(dummy1New->_value1, 1);
    ASSERT_EQ(dummy1New->_value2, 1001);

    ASSERT_EQ(dummy2New->_value1, 2);
    ASSERT_EQ(dummy2New->_value2, 1002);

    _runtime.destroyAllOwnedObjects();

    ASSERT_EQ(_runtime.getObjectCount(), 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct DeserializationContext {
    QAO_Runtime* runtime;
    std::vector<int>* reportVector;
};

class EventReporter : public QAO_Base, public hg::util::PolymorphicSerializable {
public:
    static constexpr auto SERIALIZABLE_TAG = "ST_EVENT_REPORTER";

    EventReporter(QAO_RuntimeRef rtRef, std::vector<int>& reportVec) 
        : QAO_Base{rtRef, TYPEID_SELF, 0, "EventReporter"}
        , _reportVector{reportVec}
    {
    }

    EventReporter(QAO_RuntimeRef rtRef, std::vector<int>& reportVec, hg::util::PacketBase& packet)
        : QAO_Base{rtRef, TYPEID_SELF, packet}
        , _reportVector{reportVec}
    {
    }

    // Serialization

    void serialize(hg::util::PacketBase& packet) const override {
        packet << static_cast<const QAO_Base&>(SELF);
    }

    static void deserialize(hg::util::PacketBase& packet, hg::util::AnyPtr context, int /*contextTag*/) {
        auto* ctx = context.getOrThrow<DeserializationContext>();

        QAO_PCreate<EventReporter>(ctx->runtime, *ctx->reportVector, packet);
    }

    virtual std::string getSerializableTag() const {
        return SERIALIZABLE_TAG;
    }

    bool message(int tag, hg::util::AnyPtr context) override {
        if (tag == MESSAGE_SERIALIZE) {
            auto packet = context.getOrThrow<hg::util::PacketBase>();
            hg::util::Serialize(*packet, SELF);
            return true;
        }
        return false;
    }

    // Events

    void _eventStartFrame() override {
        _reportVector.push_back(QAO_Event::StartFrame);
        _reportVector.push_back(getExecutionPriority());
    }

    void _eventPreUpdate() override { 
        _reportVector.push_back(QAO_Event::PreUpdate);
        _reportVector.push_back(getExecutionPriority());
    }

    void _eventUpdate1() override {
        _reportVector.push_back(QAO_Event::Update1);
        _reportVector.push_back(getExecutionPriority());
    }

    void _eventUpdate2() override {
        _reportVector.push_back(QAO_Event::Update2);
        _reportVector.push_back(getExecutionPriority());
    }

    void _eventPostUpdate() override {
        _reportVector.push_back(QAO_Event::PostUpdate);
        _reportVector.push_back(getExecutionPriority());
    }

    void _eventDraw1() override {
        _reportVector.push_back(QAO_Event::Draw1);
        _reportVector.push_back(getExecutionPriority());
    }

    void _eventDraw2() override {
        _reportVector.push_back(QAO_Event::Draw2);
        _reportVector.push_back(getExecutionPriority());
    }

    void _eventDrawGUI() override {
        _reportVector.push_back(QAO_Event::DrawGUI);
        _reportVector.push_back(getExecutionPriority());
    }

    void _eventFinalizeFrame() override {
        _reportVector.push_back(QAO_Event::FinalizeFrame);
        _reportVector.push_back(getExecutionPriority());
    }

//private:
    std::vector<int>& _reportVector;
};

class SerializeAtDrawEvent : public QAO_Base {
public:
    SerializeAtDrawEvent(QAO_RuntimeRef rtRef, hg::util::PacketBase& packet)
        : QAO_Base{rtRef, TYPEID_SELF, 10, "n/a"}
        , _packet{packet}
    {
    }

    void _eventDraw1() override {
        _packet << *getRuntime();

        for (auto object : *getRuntime()) {
            object->message(MESSAGE_SERIALIZE, &_packet);
        }

        QAO_Base* objectToDestroy;
        while ((objectToDestroy = getRuntime()->find("EventReporter")) != nullptr) {
            QAO_PDestroy(objectToDestroy);
        }

        ASSERT_EQ(getRuntime()->getObjectCount(), 1);
    }

private:
    hg::util::PacketBase& _packet;
};

class DeserializeAtDrawEvent : public QAO_Base {
public:
    DeserializeAtDrawEvent(QAO_RuntimeRef rtRef, hg::util::Packet& packet, DeserializationContext& deserCtx)
        : QAO_Base{rtRef, TYPEID_SELF, 10, "n/a"}
        , _packet{packet}
        , _deserCtx{deserCtx}
    {
    }

    void _eventDraw1() override {
        _packet >> *getRuntime();
        hg::util::Deserialize(_packet, &_deserCtx);
    }

private:
    hg::util::Packet& _packet;
    DeserializationContext& _deserCtx;
};

HG_PP_DO_BEFORE_MAIN(RegisterEventReporterSerializable) {
    hg::util::RegisterSerializable<EventReporter>();
}

TEST_F(QAO_SerializationTest, TestEventRestoration) {
    std::vector<int> reportVector;
    DeserializationContext deserCtx{&_runtime, &reportVector};

    auto obj1 = QAO_PCreate<EventReporter>(&_runtime, reportVector);
    obj1->setExecutionPriority(15);

    auto obj2 = QAO_PCreate<EventReporter>(&_runtime, reportVector);
    obj2->setExecutionPriority(5);

    QAO_PCreate<SerializeAtDrawEvent>(&_runtime, _packet);

    ASSERT_EQ(_runtime.getObjectCount(), 3);

    performStep();

    ASSERT_NE(_packet.getDataSize(), 0);

    _runtime.destroyAllOwnedObjects();

    QAO_PCreate<DeserializeAtDrawEvent>(&_runtime, _packet, deserCtx);

    performStep();

    for (int i = QAO_Event::StartFrame; i < QAO_Event::Count; i += 1) {
        // 1 (priority 15)
        ASSERT_EQ(reportVector[i * 4 + 0], i);
        ASSERT_EQ(reportVector[i * 4 + 1], 15);

        // 2 (priority 5)
        ASSERT_EQ(reportVector[i * 4 + 2], i);
        ASSERT_EQ(reportVector[i * 4 + 3], 5);
    }

    ASSERT_EQ(reportVector.size(), QAO_Event::Count * 4);
}
