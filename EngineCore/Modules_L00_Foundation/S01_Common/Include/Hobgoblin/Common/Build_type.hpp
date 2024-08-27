// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_COMMON_BUILD_TYPE_HPP
#define UHOBGOBLIN_COMMON_BUILD_TYPE_HPP

#define HG_DEBUG   0 //!< Debug mode.
#define HG_RELEASE 1 //!< Release mode.

#ifdef UHOBGOBLIN_DEBUG
//! When the program is being compiled in Debug mode, HG_BUILD_TYPE will be
//! equal to 0 (or HG_DEBUG). Otherwise, when the program is being compiled
//! in Release mode (including RelWithDebInfo and MinSizeRel), HG_BUILD_TYPE
//! will be equal to 1 (or HG_RELEASE).
#define HG_BUILD_TYPE HG_DEBUG
#else
//! When the program is being compiled in Debug mode, HG_BUILD_TYPE will be
//! equal to 0 (or HG_DEBUG). Otherwise, when the program is being compiled
//! in Release mode (including RelWithDebInfo and MinSizeRel), HG_BUILD_TYPE
//! will be equal to 1 (or HG_RELEASE).
#define HG_BUILD_TYPE HG_RELEASE
#endif

#endif // !UHOBGOBLIN_COMMON_BUILD_TYPE_HPP
