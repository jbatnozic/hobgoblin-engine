// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_RN_HANDLER_MACROS_PASS_ARGS_HPP
#define UHOBGOBLIN_RN_HANDLER_MACROS_PASS_ARGS_HPP

#include <Hobgoblin/Preprocessor.hpp>

#define UHOBGOBLIN_RN_PASS_ARGS_0() /* Nothing */

#define UHOBGOBLIN_RN_PASS_ARGS_1() \
    , arg0

#define UHOBGOBLIN_RN_PASS_ARGS_2() \
    UHOBGOBLIN_RN_PASS_ARGS_1()

#define UHOBGOBLIN_RN_PASS_ARGS_3() \
    UHOBGOBLIN_RN_PASS_ARGS_2(), arg1

#define UHOBGOBLIN_RN_PASS_ARGS_4() \
    UHOBGOBLIN_RN_PASS_ARGS_3()

#define UHOBGOBLIN_RN_PASS_ARGS_5() \
    UHOBGOBLIN_RN_PASS_ARGS_4(), arg2

#define UHOBGOBLIN_RN_PASS_ARGS_6() \
    UHOBGOBLIN_RN_PASS_ARGS_5()

#define UHOBGOBLIN_RN_PASS_ARGS_7() \
    UHOBGOBLIN_RN_PASS_ARGS_6(), arg3

#define UHOBGOBLIN_RN_PASS_ARGS_8() \
    UHOBGOBLIN_RN_PASS_ARGS_7()

#define UHOBGOBLIN_RN_PASS_ARGS_9() \
    UHOBGOBLIN_RN_PASS_ARGS_8(), arg4

#define UHOBGOBLIN_RN_PASS_ARGS_10() \
    UHOBGOBLIN_RN_PASS_ARGS_9()

#define UHOBGOBLIN_RN_PASS_ARGS_11() \
    UHOBGOBLIN_RN_PASS_ARGS_10(), arg5

#define UHOBGOBLIN_RN_PASS_ARGS_12() \
    UHOBGOBLIN_RN_PASS_ARGS_11()

#define UHOBGOBLIN_RN_PASS_ARGS_13() \
    UHOBGOBLIN_RN_PASS_ARGS_12(), arg6

#define UHOBGOBLIN_RN_PASS_ARGS_14() \
    UHOBGOBLIN_RN_PASS_ARGS_13()

#define UHOBGOBLIN_RN_PASS_ARGS_15() \
    UHOBGOBLIN_RN_PASS_ARGS_14(), arg7

#define UHOBGOBLIN_RN_PASS_ARGS_16() \
    UHOBGOBLIN_RN_PASS_ARGS_15()

#define UHOBGOBLIN_RN_PASS_ARGS_17() \
    UHOBGOBLIN_RN_PASS_ARGS_16(), arg8

#define UHOBGOBLIN_RN_PASS_ARGS_18() \
    UHOBGOBLIN_RN_PASS_ARGS_17()

#define UHOBGOBLIN_RN_PASS_ARGS_19() \
    UHOBGOBLIN_RN_PASS_ARGS_18(), arg9

#define UHOBGOBLIN_RN_PASS_ARGS_20() \
    UHOBGOBLIN_RN_PASS_ARGS_19()

///////////////////////////////////////

#if defined(UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR)
#   define UHOBGOBLIN_RN_PASS_ARGS_FINAL(_num_) \
        UHOBGOBLIN_PP_EXPAND(UHOBGOBLIN_RN_PASS_ARGS_##_num_())
#else
#   define UHOBGOBLIN_RN_PASS_ARGS_FINAL(_num_) \
        UHOBGOBLIN_RN_PASS_ARGS_##_num_()
#endif

#define UHOBGOBLIN_RN_PASS_ARGS_MIDDLE(_num_) \
    UHOBGOBLIN_RN_PASS_ARGS_FINAL(_num_)

#define UHOBGOBLIN_RN_PASS_ARGS(...) \
    UHOBGOBLIN_RN_PASS_ARGS_MIDDLE(HG_PP_COUNT_ARGS(__VA_ARGS__))

#endif // !UHOBGOBLIN_RN_HANDLER_MACROS_PASS_ARGS_HPP

// clang-format on
