#pragma once

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>

#include "Engine.hpp"

#include <cstdint>
#include <iostream>
#include <ostream>

namespace spe = jbatnozic::spempe;
namespace hg = jbatnozic::hobgoblin;

struct VisibleState {
    float x = 0.f;
    float y = 0.f;
    std::uint32_t color = 0xFF00FFFF;
    std::int8_t index = -1;

    HG_ENABLE_AUTOPACK(VisibleState, x, y, color, index);
};

inline
std::ostream& operator<<(std::ostream& aOS, const VisibleState& aVS) {
    return (aOS << (int)aVS.x);
}

SPEMPE_DEFINE_AUTODIFF_STATE(AutodiffVisibleState,
    SPEMPE_MEMBER(float, x, 0.f),
    SPEMPE_MEMBER(float, y, 0.f),
    SPEMPE_MEMBER(std::uint32_t, color, 0xFF00FFFF),
    SPEMPE_MEMBER(std::int8_t, index, -1)
) {};

inline
std::ostream& operator<<(std::ostream& aOS, const AutodiffVisibleState& aVS) {
    return (aOS << (int)aVS.x);
}

///////////////////////////////////////////////////////////////////////////
// BASIC ACTOR                                                           //
///////////////////////////////////////////////////////////////////////////

class BasicActor : public spe::SynchronizedObject<VisibleState> {
public:
  BasicActor(hg::QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId);
  ~BasicActor();

  void init(float aX, float aY, hg::gr::Color aColor, std::int8_t aIndex);

private:
    enum class State {
        WAIT_LEFT,
        GO_RIGHT,
        WAIT_RIGHT,
        GO_LEFT,

        STATE_COUNT
    };

    static constexpr auto STATE_DURATION_WAIT = 30;
    static constexpr auto STATE_DURATION_GO   = 180;
    static constexpr auto SPEED               = 4.f;

    State _state = State::WAIT_LEFT;
    int _durationCounter = 0;
    
    void _eventUpdate1(spe::IfMaster) override;
    void _eventDraw1() override;

    void _syncCreateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncUpdateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncDestroyImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
};

///////////////////////////////////////////////////////////////////////////
// AUTODIFF ACTOR                                                        //
///////////////////////////////////////////////////////////////////////////

class AutodiffActor : public spe::SynchronizedObject<AutodiffVisibleState> {
public:
  AutodiffActor(hg::QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId);
  ~AutodiffActor();

  void init(float aX, float aY, hg::gr::Color aColor, std::int8_t aIndex);

private:
    enum class State {
        WAIT_LEFT,
        GO_RIGHT,
        WAIT_RIGHT,
        GO_LEFT,

        STATE_COUNT
    };

    static constexpr auto STATE_DURATION_WAIT = 30;
    static constexpr auto STATE_DURATION_GO   = 180;
    static constexpr auto SPEED               = 4.f;

    State _state = State::WAIT_LEFT;
    int _durationCounter = 0;
    
    void _eventUpdate1(spe::IfMaster) override;
    void _eventPostUpdate(spe::IfMaster) override;
    void _eventDraw1() override;

    void _syncCreateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncUpdateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncDestroyImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
};

///////////////////////////////////////////////////////////////////////////
// ALTERNATING ACTOR                                                     //
///////////////////////////////////////////////////////////////////////////

class AlternatingActor : public spe::SynchronizedObject<VisibleState> {
public:
  AlternatingActor(hg::QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId);
  ~AlternatingActor();

  void init(float aX, float aY, hg::gr::Color aColor, std::int8_t aIndex);

private:
    enum class State {
        WAIT_LEFT,
        GO_RIGHT,
        WAIT_RIGHT,
        GO_LEFT,

        STATE_COUNT
    };

    static constexpr auto STATE_DURATION_WAIT = 30;
    static constexpr auto STATE_DURATION_GO   = 180;
    static constexpr auto SPEED               = 4.f;

    State _state = State::WAIT_LEFT;
    int _durationCounter = 0;
    
    void _eventUpdate1(spe::IfMaster) override;
    void _eventDraw1() override;

    void _syncCreateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncUpdateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncDestroyImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
};

///////////////////////////////////////////////////////////////////////////
// ALTERNATING AUTODIFF ACTOR                                            //
///////////////////////////////////////////////////////////////////////////

class AlternatingAutodiffActor : public spe::SynchronizedObject<AutodiffVisibleState> {
public:
  AlternatingAutodiffActor(hg::QAO_RuntimeRef aRuntimeRef, spe::RegistryId aRegId, spe::SyncId aSyncId);
  ~AlternatingAutodiffActor();

  void init(float aX, float aY, hg::gr::Color aColor, std::int8_t aIndex);

private:
    enum class State {
        WAIT_LEFT,
        GO_RIGHT,
        WAIT_RIGHT,
        GO_LEFT,

        STATE_COUNT
    };

    static constexpr auto STATE_DURATION_WAIT = 30;
    static constexpr auto STATE_DURATION_GO   = 180;
    static constexpr auto SPEED               = 4.f;

    State _state = State::WAIT_LEFT;
    int _durationCounter = 0;
    
    void _eventUpdate1(spe::IfMaster) override;
    void _eventPostUpdate(spe::IfMaster) override;
    void _eventDraw1() override;

    void _syncCreateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncUpdateImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
    void _syncDestroyImpl(spe::SyncControlDelegate& aSyncCtrl) const override;
};
