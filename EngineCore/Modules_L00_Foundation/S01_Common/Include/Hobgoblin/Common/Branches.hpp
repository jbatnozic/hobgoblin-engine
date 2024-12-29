// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_COMMON_BRANCHES_HPP
#define UHOBGOBLIN_COMMON_BRANCHES_HPP

//! Put this as the first statement in the then-body or an else-body of an `if` statement, if that is
//! the MOST LIKELY outcome, to help the compiler optimize the generated assembly.
//!
//! Example:
//!     if (ConditionThatIsAlmostAlwaysTrue()) {
//!         HG_LIKELY_BRANCH;
//!         // do something...
//!     } else {
//!         HG_UNLIKELY_BRANCH;
//!         // do something else...
//!     }
#define HG_LIKELY_BRANCH [[likely]]

//! Put this as the first statement in the then-body or an else-body of an `if` statement, if that is
//! the UNLIKELY outcome, to help the compiler optimize the generated assembly.
//! (see description of `HG_LIKELY_BRANCH`)
#define HG_UNLIKELY_BRANCH [[unlikely]]

//! Wrap the condition of an `if` statement in this macro if it is LIKELY to be true, to help the
//! compiler optimize the generated assembly.
//!
//! Example:
//!     if (HG_LIKELY_CONDITION(ConditionThatIsAlmostAlwaysTrue())) {
//!         // do something...
//!     }
#define HG_LIKELY_CONDITION(...) UHOBGOBLIN_LIKELY_CONDITION_IMPL(__VA_ARGS__)

//! Wrap the condition of an `if` statement in this macro if it is UNLIKELY to be true, to help the
//! compiler optimize the generated assembly.
//!
//! Example:
//!     if (HG_UNLIKELY_CONDITION(!ConditionThatIsAlmostAlwaysTrue())) {
//!         // do something...
//!     }
#define HG_UNLIKELY_CONDITION(...) UHOBGOBLIN_UNLIKELY_CONDITION_IMPL(__VA_ARGS__)

// Implementation detail
#if defined(__GNUC__) && !defined(__clang__)
#define UHOBGOBLIN_LIKELY_CONDITION_IMPL(...)   __builtin_expect(!!(__VA_ARGS__), 1)
#define UHOBGOBLIN_UNLIKELY_CONDITION_IMPL(...) __builtin_expect(!!(__VA_ARGS__), 0)
#else
#define UHOBGOBLIN_LIKELY_CONDITION_IMPL(...)   __VA_ARGS__
#define UHOBGOBLIN_UNLIKELY_CONDITION_IMPL(...) __VA_ARGS__
#endif

#endif // !UHOBGOBLIN_COMMON_BRANCHES_HPP
