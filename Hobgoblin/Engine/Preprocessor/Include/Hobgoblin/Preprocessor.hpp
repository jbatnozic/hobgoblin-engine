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

#else 

#define UHOBGOBLIN_PP_COUNT_ARGS_FINAL( \
    _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, _8, \
    _9, _10, _11, _12, _13, _14, _15, _16, _N, ...) _N

#define HG_PP_COUNT_ARGS(...) \
    UHOBGOBLIN_PP_COUNT_ARGS_FINAL( \
        "dummy", ##__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, \
         7, 6, 5, 4, 3, 2, 1, 0 \
    )

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