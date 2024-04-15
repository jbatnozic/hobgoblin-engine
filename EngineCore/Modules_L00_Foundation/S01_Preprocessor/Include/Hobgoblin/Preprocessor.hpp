// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_PREPROCESSOR_HPP
#define UHOBGOBLIN_PREPROCESSOR_HPP

///////////////////////////////////////////////////////////////////////////
// INFRASTRUCTURE                                                        //
///////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && (!defined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL)

#define UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR
#define UHOBGOBLIN_PP_EXPAND(...) __VA_ARGS__
#define UHOBGOBLIN_PP_VA_ARGS(_leading_arg_, ...) _leading_arg_, __VA_ARGS__

#endif

///////////////////////////////////////////////////////////////////////////
// COUNTING MACRO ARGUMENTS                                              //
///////////////////////////////////////////////////////////////////////////

#define UHOBGOBLIN_PP_COUNT_ARGS_FINAL( \
     _0_,  _1_,  _2_,  _3_,  _4_,  _5_,  _6_,  _7_,  _8_,  _9_, \
    _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, \
    _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, \
    _30_, _31_, _32_, _33_, _34_, _35_, _36_, _37_, _38_, _39_, \
    _40_, _41_, _42_, _43_, _44_, _45_, _46_, _47_, _48_, _49_, \
    _50_, _51_, _52_, _53_, _54_, _55_, _56_, _57_, _58_, _59_, \
    _60_, _61_, _62_, _63_, _64_, _65_, _66_, _67_, _68_, _69_, \
    _70_, _71_, _72_, _73_, _74_, _75_, _76_, _77_, _78_, _79_, \
    _80_, _81_, _82_, _83_, _84_, _85_, _86_, _87_, _88_, _89_, \
    _90_, _91_, _92_, _93_, _94_, _95_, _96_, _97_, _98_, _99_, \
    _100, _N_, ...) _N_

#if defined(UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR)

#define UHOBGOBLIN_PP_COUNT_ARGS_MIDDLE(...) \
    UHOBGOBLIN_PP_EXPAND( \
        UHOBGOBLIN_PP_COUNT_ARGS_FINAL(__VA_ARGS__, 100, \
            99, 98, 97, 96, 95, 94, 93, 92, 91, 90, \
            89, 88, 87, 86, 85, 84, 83, 82, 81, 80, \
            79, 78, 77, 76, 75, 74, 73, 72, 71, 70, \
            69, 68, 67, 66, 65, 64, 63, 62, 61, 60, \
            59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
            49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
            39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
            29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
            19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
             9,  8,  7,  6,  5,  4,  3,  2,  1,  0  \
        ) \
    )

#define HG_PP_COUNT_ARGS(...) \
    UHOBGOBLIN_PP_COUNT_ARGS_MIDDLE(UHOBGOBLIN_PP_VA_ARGS("dummy", __VA_ARGS__))

#elif defined(_MSC_VER)

#define HG_PP_COUNT_ARGS(...) \
    UHOBGOBLIN_PP_COUNT_ARGS_FINAL("dummy", ##__VA_ARGS__, 100, \
        99, 98, 97, 96, 95, 94, 93, 92, 91, 90, \
        89, 88, 87, 86, 85, 84, 83, 82, 81, 80, \
        79, 78, 77, 76, 75, 74, 73, 72, 71, 70, \
        69, 68, 67, 66, 65, 64, 63, 62, 61, 60, \
        59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
        49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
        39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
        29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
        19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
         9,  8,  7,  6,  5,  4,  3,  2,  1,  0  \
    )

#else

// If not using MSVC, we just hope the used compiler supports __VA_OPT__ ...

#define HG_PP_COUNT_ARGS(...) \
    UHOBGOBLIN_PP_COUNT_ARGS_FINAL(0 __VA_OPT__(,) __VA_ARGS__, \
       100, 99, 98, 97, 96, 95, 94, 93, 92, 91, \
        90, 89, 88, 87, 86, 85, 84, 83, 82, 81, \
        80, 79, 78, 77, 76, 75, 74, 73, 72, 71, \
        70, 69, 68, 67, 66, 65, 64, 63, 62, 61, \
        60, 59, 58, 57, 56, 55, 54, 53, 52, 51, \
        50, 49, 48, 47, 46, 45, 44, 43, 42, 41, \
        40, 39, 38, 37, 36, 35, 34, 33, 32, 31, \
        30, 29, 28, 27, 26, 25, 24, 23, 22, 21, \
        20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
        10,  9,  8,  7,  6,  5,  4,  3,  2,  1, 0)

#endif

///////////////////////////////////////////////////////////////////////////
// DO BEFORE MAIN                                                        //
///////////////////////////////////////////////////////////////////////////

#define HG_PP_DO_BEFORE_MAIN(_name_) \
    namespace { \
    struct UHOBGOBLIN_DoBeforeMainImpl_##_name_ { \
        explicit UHOBGOBLIN_DoBeforeMainImpl_##_name_(); \
    }; \
    } \
    UHOBGOBLIN_DoBeforeMainImpl_##_name_ UHOBGOBLIN_DoBeforeMainInstance_##_name_{}; \
    UHOBGOBLIN_DoBeforeMainImpl_##_name_::UHOBGOBLIN_DoBeforeMainImpl_##_name_()

#endif // !UHOBGOBLIN_PREPROCESSOR_HPP

// clang-format on
