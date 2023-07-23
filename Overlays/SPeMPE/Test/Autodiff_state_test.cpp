
#include <SPeMPE/SPeMPE.hpp>

#include <gtest/gtest.h>

#include <cstdint>

namespace jbatnozic {
namespace spempe {
namespace test {

SPEMPE_DEFINE_AUTODIFF_STATE(TestAutodiff1,
    SPEMPE_MEMBER(std::int32_t, i, 123)
) {};

SPEMPE_DEFINE_AUTODIFF_STATE(TestAutodiff2,
    SPEMPE_MEMBER(std::int32_t, i, 123),
    SPEMPE_MEMBER(std::int8_t, c, 'a')
) {};

TEST(AutodiffStateTest, IllegalStateTest) {
    TestAutodiff1 ad1;
    hg::util::Packet packet;

    EXPECT_THROW({ ad1.cmp(); }, detail::AutodiffStateIllegalStateError);
    EXPECT_THROW({ ad1.commit(); }, detail::AutodiffStateIllegalStateError);
    EXPECT_THROW({ ad1.packDiff(packet); }, detail::AutodiffStateIllegalStateError);
}

TEST(AutodiffStateTest, CommitAndCmpTest) {
    TestAutodiff1 ad1;
    ad1.initMirror();
    EXPECT_EQ(ad1.i, 123);
    EXPECT_EQ(ad1.cmp(), AUTODIFF_STATE_NO_CHANGE);
    ad1.i = 456;
    EXPECT_EQ(ad1.cmp(), AUTODIFF_STATE_HAS_CHANGE);
    ad1.commit();
    EXPECT_EQ(ad1.cmp(), AUTODIFF_STATE_NO_CHANGE);
}

TEST(AutodiffStateTest, PackAllTest) {
    TestAutodiff2 ad2;
    ad2.initMirror();
    ad2.i = 456;
    ad2.c = 'b';

    hg::util::Packet packet;
    ad2.packAll(packet);

    TestAutodiff2 ad2_other;
    EXPECT_EQ(ad2_other.i, 123);
    EXPECT_EQ(ad2_other.c, 'a');

    ad2_other.unpack(packet);
    EXPECT_EQ(ad2_other.i, 456);
    EXPECT_EQ(ad2_other.c, 'b');
}

TEST(AutodiffStateTest, PackEmptyDiffTest) {
    TestAutodiff2 ad2;
    ad2.initMirror();
    EXPECT_EQ(ad2.cmp(), AUTODIFF_STATE_NO_CHANGE);

    hg::util::Packet packet;
    ad2.packDiff(packet);

    EXPECT_EQ(packet.getDataSize(), 1);
    EXPECT_EQ(packet.extract<std::int8_t>(), 0);
}

TEST(AutodiffStateTest, PackDiffTest) {
    {
        TestAutodiff2 ad2;
        ad2.initMirror();
        ad2.i = 456;
        hg::util::Packet packet;
        ad2.packDiff(packet);
        EXPECT_EQ(packet.getDataSize(), 1 + sizeof(ad2.i));
        EXPECT_EQ(packet.extract<std::int8_t>(), 1);
        EXPECT_EQ(packet.extract<std::int32_t>(), 456);
    }
    {
        TestAutodiff2 ad2;
        ad2.initMirror();
        ad2.c = 'b';
        hg::util::Packet packet;
        ad2.packDiff(packet);
        EXPECT_EQ(packet.getDataSize(), 1 + sizeof(ad2.c));
        EXPECT_EQ(packet.extract<std::int8_t>(), 2);
        EXPECT_EQ(packet.extract<std::int8_t>(), 'b');
    }
    {
        TestAutodiff2 ad2;
        ad2.initMirror();
        ad2.i = 456;
        ad2.c = 'b';
        hg::util::Packet packet;
        ad2.packDiff(packet);
        EXPECT_EQ(packet.getDataSize(), 1 + sizeof(ad2.i) + sizeof(ad2.c));
        EXPECT_EQ(packet.extract<std::int8_t>(), 3);
        EXPECT_EQ(packet.extract<std::int8_t>(), 'b');
        EXPECT_EQ(packet.extract<std::int32_t>(), 456);
    }
}

TEST(AutodiffStateTest, UnpackOperator) {
    TestAutodiff2 ad2;
    ad2.i = 1000;
    ad2.c = '!';
    hg::util::Packet packet;
    ad2.packAll(packet);

    {
        TestAutodiff2 ad2_other;
        packet >> ad2_other;
        EXPECT_EQ(ad2_other.i, 1000);
        EXPECT_EQ(ad2_other.c,  '!');
    }
}

TEST(AutodiffStateTest, ApplyDiff) {
    TestAutodiff2 ad2;
    ad2.i = 1000;
    ad2.c = '!';
    hg::util::Packet packet;

    {
        TestAutodiff2 ad2_other;
        ad2_other.initMirror();
        ad2_other.i = 1000;
        ad2_other.c = '!';
        ad2_other.commit();
        ad2_other.c = 'D';
        ad2_other.packDiff(packet);
    }

    {
        TestAutodiff2 ad2_other;
        ad2_other.unpack(packet);

        ad2.applyDiff(ad2_other);
    }

    EXPECT_EQ(ad2.i, 1000);
    EXPECT_EQ(ad2.c,  'D');
}

SPEMPE_DEFINE_AUTODIFF_STATE(TestAutodiff32,
    SPEMPE_MEMBER(std::int32_t,  i0, 0),
    SPEMPE_MEMBER(std::int32_t,  i1, 0),
    SPEMPE_MEMBER(std::int32_t,  i2, 0),
    SPEMPE_MEMBER(std::int32_t,  i3, 0),
    SPEMPE_MEMBER(std::int32_t,  i4, 0),
    SPEMPE_MEMBER(std::int32_t,  i5, 0),
    SPEMPE_MEMBER(std::int32_t,  i6, 0),
    SPEMPE_MEMBER(std::int32_t,  i7, 0),
    SPEMPE_MEMBER(std::int32_t,  i8, 0),
    SPEMPE_MEMBER(std::int32_t,  i9, 0),
    SPEMPE_MEMBER(std::int32_t, i10, 0),
    SPEMPE_MEMBER(std::int32_t, i11, 0),
    SPEMPE_MEMBER(std::int32_t, i12, 0),
    SPEMPE_MEMBER(std::int32_t, i13, 0),
    SPEMPE_MEMBER(std::int32_t, i14, 0),
    SPEMPE_MEMBER(std::int32_t, i15, 0),
    SPEMPE_MEMBER(std::int32_t, i16, 0),
    SPEMPE_MEMBER(std::int32_t, i17, 0),
    SPEMPE_MEMBER(std::int32_t, i18, 0),
    SPEMPE_MEMBER(std::int32_t, i19, 0),
    SPEMPE_MEMBER(std::int32_t, i20, 0),
    SPEMPE_MEMBER(std::int32_t, i21, 0),
    SPEMPE_MEMBER(std::int32_t, i22, 0),
    SPEMPE_MEMBER(std::int32_t, i23, 0),
    SPEMPE_MEMBER(std::int32_t, i24, 0),
    SPEMPE_MEMBER(std::int32_t, i25, 0),
    SPEMPE_MEMBER(std::int32_t, i26, 0),
    SPEMPE_MEMBER(std::int32_t, i27, 0),
    SPEMPE_MEMBER(std::int32_t, i28, 0),
    SPEMPE_MEMBER(std::int32_t, i29, 0),
    SPEMPE_MEMBER(std::int32_t, i30, 0),
    SPEMPE_MEMBER(std::int32_t, i31, 0)
) {};

TEST(AutodiffStateTest, AutodiffState_32Members) {
    TestAutodiff32 ad32;
    ad32.initMirror();
    EXPECT_EQ(ad32.cmp(), AUTODIFF_STATE_NO_CHANGE);

    {
        hg::util::Packet packet;
        ad32.packDiff(packet);
        EXPECT_EQ(packet.getDataSize(), 4);
        EXPECT_EQ(packet.extract<std::int32_t>(), 0);
    }
    {
        hg::util::Packet packet;
        ad32.packAll(packet);
        EXPECT_EQ(packet.getDataSize(), 4 + 32 * sizeof(std::int32_t));
        EXPECT_EQ(packet.extract<std::int32_t>(), 0xFFFFFFFF);
    }
    {
        ad32.i4 = 0xDEADBEEF;
        EXPECT_EQ(ad32.cmp(), AUTODIFF_STATE_HAS_CHANGE);
        hg::util::Packet packet;
        ad32.packDiff(packet);
        EXPECT_EQ(packet.getDataSize(), 4 + sizeof(std::int32_t));
        EXPECT_EQ(packet.extract<std::int8_t>(), 16);
        EXPECT_EQ(packet.extract<std::int8_t>(), 0);
        EXPECT_EQ(packet.extract<std::int8_t>(), 0);
        EXPECT_EQ(packet.extract<std::int8_t>(), 0);
        EXPECT_EQ(packet.extract<std::int32_t>(), 0xDEADBEEF);
    }
}

} // namespace test
} // namespace spempe
} // namespace jbatnozic
