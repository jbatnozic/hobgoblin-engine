// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <SPeMPE/SPeMPE.hpp>

#include <gtest/gtest.h>

namespace hg = jbatnozic::hobgoblin;
using namespace jbatnozic::spempe;

namespace {
void SimulateKeyPress(KbInputTracker& aTracker, sf::Keyboard::Key aKey) {
    sf::Event ev;
    ev.type = sf::Event::KeyPressed;
    ev.key.code = aKey;
    aTracker.keyEventOccurred(ev);
}

void SimulateKeyRelease(KbInputTracker& aTracker, sf::Keyboard::Key aKey) {
    sf::Event ev;
    ev.type = sf::Event::KeyReleased;
    ev.key.code = aKey;
    aTracker.keyEventOccurred(ev);
}
} // namespace

TEST(KeyboardInputTest, BasicPressAndReleaseTracking) {
    KbInputTracker tracker;

    {
        const auto in = tracker.getInput();
        EXPECT_FALSE(in.checkPressed(KbKey::A, KbInput::Mode::Default));
        EXPECT_FALSE(in.checkPressed(KbKey::A, KbInput::Mode::Edge));
        EXPECT_FALSE(in.checkPressed(KbKey::A, KbInput::Mode::Repeated));

        EXPECT_TRUE(in.checkReleased(KbKey::A, KbInput::Mode::Default));
        EXPECT_FALSE(in.checkReleased(KbKey::A, KbInput::Mode::Edge));
        EXPECT_FALSE(in.checkReleased(KbKey::A, KbInput::Mode::Repeated));
    }

    tracker.prepForEvents();
    SimulateKeyPress(tracker, sf::Keyboard::A);

    {
        const auto in = tracker.getInput();
        EXPECT_TRUE(in.checkPressed(KbKey::A, KbInput::Mode::Default));
        EXPECT_TRUE(in.checkPressed(KbKey::A, KbInput::Mode::Edge));
        EXPECT_TRUE(in.checkPressed(KbKey::A, KbInput::Mode::Repeated));

        EXPECT_FALSE(in.checkReleased(KbKey::A, KbInput::Mode::Default));
        EXPECT_FALSE(in.checkReleased(KbKey::A, KbInput::Mode::Edge));
        EXPECT_FALSE(in.checkReleased(KbKey::A, KbInput::Mode::Repeated));
    }

    tracker.prepForEvents();

    {
        const auto in = tracker.getInput();
        EXPECT_TRUE(in.checkPressed(KbKey::A, KbInput::Mode::Default));
        EXPECT_FALSE(in.checkPressed(KbKey::A, KbInput::Mode::Edge));
        EXPECT_FALSE(in.checkPressed(KbKey::A, KbInput::Mode::Repeated));

        EXPECT_FALSE(in.checkReleased(KbKey::A, KbInput::Mode::Default));
        EXPECT_FALSE(in.checkReleased(KbKey::A, KbInput::Mode::Edge));
        EXPECT_FALSE(in.checkReleased(KbKey::A, KbInput::Mode::Repeated));
    }

    tracker.prepForEvents();
    SimulateKeyRelease(tracker, sf::Keyboard::A);

    {
        const auto in = tracker.getInput();
        EXPECT_FALSE(in.checkPressed(KbKey::A, KbInput::Mode::Default));
        EXPECT_FALSE(in.checkPressed(KbKey::A, KbInput::Mode::Edge));
        EXPECT_FALSE(in.checkPressed(KbKey::A, KbInput::Mode::Repeated));

        EXPECT_TRUE(in.checkReleased(KbKey::A, KbInput::Mode::Default));
        EXPECT_TRUE(in.checkReleased(KbKey::A, KbInput::Mode::Edge));
        EXPECT_TRUE(in.checkReleased(KbKey::A, KbInput::Mode::Repeated));
    }

    tracker.prepForEvents();

    {
        const auto in = tracker.getInput();
        EXPECT_FALSE(in.checkPressed(KbKey::A, KbInput::Mode::Default));
        EXPECT_FALSE(in.checkPressed(KbKey::A, KbInput::Mode::Edge));
        EXPECT_FALSE(in.checkPressed(KbKey::A, KbInput::Mode::Repeated));

        EXPECT_TRUE(in.checkReleased(KbKey::A, KbInput::Mode::Default));
        EXPECT_FALSE(in.checkReleased(KbKey::A, KbInput::Mode::Edge));
        EXPECT_FALSE(in.checkReleased(KbKey::A, KbInput::Mode::Repeated));
    }
}

TEST(KeyboardInputTest, StringConversions) {
    EXPECT_EQ(StringToKbKey("C"),         KbKey::C);
    EXPECT_EQ(StringToKbKey("F6"),        KbKey::F6);
    EXPECT_EQ(StringToKbKey("Escape"),    KbKey::Escape);
    EXPECT_EQ(StringToKbKey("Backslash"), KbKey::Backslash);
    EXPECT_EQ(StringToKbKey("Pause"),     KbKey::Pause);

    EXPECT_EQ(StringToKbKey("abcdef123"), KbKey::Unknown);
    EXPECT_EQ(StringToKbKey(""), KbKey::Unknown);

    EXPECT_EQ(KbKeyToString(KbKey::C),         "C");
    EXPECT_EQ(KbKeyToString(KbKey::F6),        "F6");
    EXPECT_EQ(KbKeyToString(KbKey::Escape),    "Escape");
    EXPECT_EQ(KbKeyToString(KbKey::Backslash), "Backslash");
    EXPECT_EQ(KbKeyToString(KbKey::Pause),     "Pause");

    EXPECT_EQ(KbKeyToString(KbKey::Unknown), "Unknown");
}

// clang-format on
