#ifndef UHOBGOBLIN_PREPROCESSOR_HPP
#define UHOBGOBLIN_PREPROCESSOR_HPP

#if defined(_MSC_VER) && (!defined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL)
#define UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR
#endif

#if defined(UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR)

#define UHOBGOBLIN_PP_EXPAND(...) __VA_ARGS__
#define UHOBGOBLIN_PP_VA_ARGS(_leading_arg_, ...) _leading_arg_, __VA_ARGS__

#define UHOBGOBLIN_PP_COUNT_ARGS_FINAL( \
    _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _N, ...) _N

#define UHOBGOBLIN_PP_COUNT_ARGS_MIDDLE(...) \
    UHOBGOBLIN_PP_EXPAND( \
        UHOBGOBLIN_PP_COUNT_ARGS_FINAL( \
            __VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
             9, 8, 7, 6, 5, 4, 3, 2, 1, 0 \
        ) \
    )

#define HG_PP_COUNT_ARGS(...) \
    UHOBGOBLIN_PP_COUNT_ARGS_MIDDLE(UHOBGOBLIN_PP_VA_ARGS("dummy", __VA_ARGS__))

#elif defined(_MSC_VER)

#define UHOBGOBLIN_PP_COUNT_ARGS_FINAL( \
    _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _N, ...) _N

#define HG_PP_COUNT_ARGS(...) \
    UHOBGOBLIN_PP_COUNT_ARGS_FINAL( \
        "dummy", ##__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, \
         7, 6, 5, 4, 3, 2, 1, 0 \
    )

#else

// If not using MSVC, we just hope the used compiler supports __VA_OPT__ ...

#define UHOBGOBLIN_HG_PP_COUNT_ARGS_FINAL( \
     _0_,  _1_,  _2_,  _3_,  _4_,  _5_,  _6_,  _7_,  _8_,  _9_, \
    _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, \
    _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, \
    _30_, _31_, _32_, _33_, _34_, _35_, _36_, _37_, _38_, _39_, \
    _40_, _41_, _42_, _43_, _44_, _45_, _46_, _47_, _48_, _49_, \
    _50_, _51_, _52_, _53_, _54_, _55_, _56_, _57_, _58_, _59_, \
    _60_, _61_, _62_, _63_, _64_, _65_, _66_, _67_, _68_, _69_, _70, count, ...) count

#define HG_PP_COUNT_ARGS(...) \
    UHOBGOBLIN_HG_PP_COUNT_ARGS_FINAL(0 __VA_OPT__(,) __VA_ARGS__, \
        70, 69, 68, 67, 66, 65, 64, 63, 62, 61, \
        60, 59, 58, 57, 56, 55, 54, 53, 52, 51, \
        50, 49, 48, 47, 46, 45, 44, 43, 42, 41, \
        40, 39, 38, 37, 36, 35, 34, 33, 32, 31, \
        30, 29, 28, 27, 26, 25, 24, 23, 22, 21, \
        20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
        10,  9,  8,  7,  6,  5,  4,  3,  2,  1, 0)

#endif

#define HG_PP_DO_BEFORE_MAIN(_name_) \
    namespace { \
    struct UHOBGOBLIN_DoBeforeMainImpl_##_name_ { \
        explicit UHOBGOBLIN_DoBeforeMainImpl_##_name_(); \
    }; \
    } \
    UHOBGOBLIN_DoBeforeMainImpl_##_name_ UHOBGOBLIN_DoBeforeMainInstance_##_name_{}; \
    UHOBGOBLIN_DoBeforeMainImpl_##_name_::UHOBGOBLIN_DoBeforeMainImpl_##_name_()

#endif // !UHOBGOBLIN_PREPROCESSOR_HPP