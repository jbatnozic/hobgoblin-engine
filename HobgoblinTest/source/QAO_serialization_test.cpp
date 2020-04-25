
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Exceptions.hpp>
#include <Hobgoblin/Utility/Serialization.hpp>
#include <Hobgoblin/Preprocessor.hpp>

#include <gtest/gtest.h>
#include <vector>

using namespace hg::qao;

#define SELF (*this)
#define TYPEID_SELF typeid(decltype(*this))

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

HG_PP_DO_BEFORE_MAIN(RegisterSerializables) {
    hg::util::RegisterSerializable<Dummy>();
}

class QAO_SerializationTest : public ::testing::Test {
public:

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

    _runtime.eraseAllNonOwnedObjects();

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

    _runtime.eraseAllNonOwnedObjects();

    ASSERT_EQ(_runtime.getObjectCount(), 0);
}