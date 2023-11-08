#ifndef UHOBGOBLIN_RN_HANDLER_MACROS_PASS_COMPOSE_ARGS_HPP
#define UHOBGOBLIN_RN_HANDLER_MACROS_PASS_COMPOSE_ARGS_HPP

#include <Hobgoblin/Preprocessor.hpp>

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_0() /* Nothing */

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_1(type0) /* Nothing */

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_2(type0, name0) \
    , name0

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_3(type0, name0, type1) \
    , name0, name1

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_4(type0, name0, type1, name1) \
    , name0, name1

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_5(type0, name0, type1, name1, type2) \
    , name0, name1, name2

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_6(type0, name0, type1, name1, type2, name2) \
    , name0, name1, name2

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_7(type0, name0, type1, name1, type2, name2, type3) \
    , name0, name1, name2, name3

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_8(type0, name0, type1, name1, type2, name2, type3, name3) \
    , name0, name1, name2, name3

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_9(type0, name0, type1, name1, type2, name2, type3, name3, \
                                          type4) \
    , name0, name1, name2, name3 \
    , name4

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_10(type0, name0, type1, name1, type2, name2, type3, name3, \
                                           type4, name4) \
    , name0, name1, name2, name3 \
    , name4

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_11(type0, name0, type1, name1, type2, name2, type3, name3, \
                                           type4, name4, type5) \
    , name0, name1, name2, name3 \
    , name4, name5

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_12(type0, name0, type1, name1, type2, name2, type3, name3, \
                                           type4, name4, type5, name5) \
    , name0, name1, name2, name3 \
    , name4, name5

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_13(type0, name0, type1, name1, type2, name2, type3, name3, \
                                           type4, name4, type5, name5, type6) \
    , name0, name1, name2, name3 \
    , name4, name5, name6

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_14(type0, name0, type1, name1, type2, name2, type3, name3, \
                                           type4, name4, type5, name5, type6, name6) \
    , name0, name1, name2, name3 \
    , name4, name5, name6

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_15(type0, name0, type1, name1, type2, name2, type3, name3, \
                                           type4, name4, type5, name5, type6, name6, type7) \
    , name0, name1, name2, name3 \
    , name4, name5, name6, name7

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_16(type0, name0, type1, name1, type2, name2, type3, name3, \
                                           type4, name4, type5, name5, type6, name6, type7, name7) \
    , name0, name1, name2, name3 \
    , name4, name5, name6, name7

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_17(type0, name0, type1, name1, type2, name2, type3, name3, \
                                           type4, name4, type5, name5, type6, name6, type7, name7, \
                                           type8) \
    , name0, name1, name2, name3 \
    , name4, name5, name6, name7 \
    , name8

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_18(type0, name0, type1, name1, type2, name2, type3, name3, \
                                           type4, name4, type5, name5, type6, name6, type7, name7, \
                                           type8, name8) \
    , name0, name1, name2, name3 \
    , name4, name5, name6, name7 \
    , name8

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_19(type0, name0, type1, name1, type2, name2, type3, name3, \
                                           type4, name4, type5, name5, type6, name6, type7, name7, \
                                           type8, name8, type9) \
    , name0, name1, name2, name3 \
    , name4, name5, name6, name7 \
    , name8, name9

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_20(type0, name0, type1, name1, type2, name2, type3, name3, \
                                           type4, name4, type5, name5, type6, name6, type7, name7, \
                                           type8, name8, type9, name9) \
    , name0, name1, name2, name3 \
    , name4, name5, name6, name7 \
    , name8, name9

///////////////////////////////////////

#if defined(UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR)
#   define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_FINAL(_num_, ...) \
        UHOBGOBLIN_PP_EXPAND(UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_##_num_(__VA_ARGS__))
#else
#   define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_FINAL(_num_, ...) \
        UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_##_num_(__VA_ARGS__)
#endif

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_MIDDLE(_num_, ...) \
    UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_FINAL(_num_, __VA_ARGS__)

#define UHOBGOBLIN_RN_PASS_COMPOSE_ARGS(...) \
    UHOBGOBLIN_RN_PASS_COMPOSE_ARGS_MIDDLE(HG_PP_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

#endif // !UHOBGOBLIN_RN_HANDLER_MACROS_PASS_COMPOSE_ARGS_HPP
