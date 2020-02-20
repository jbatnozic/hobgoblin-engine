#ifndef UHOBGOBLIN_RN_MACRO_NORMALIZE_ARGS_HPP
#define UHOBGOBLIN_RN_MACRO_NORMALIZE_ARGS_HPP

#include <Hobgoblin/Preprocessor.hpp>

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_0(_cvspec_, dummy) /* Nothing */

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_1(_cvspec_, type0) \
    , _cvspec_ type0

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_2(_cvspec_, type0, name0) \
    , _cvspec_ type0 name0

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_3(_cvspec_, type0, name0, type1) \
    , _cvspec_ type0 name0, _cvspec_ type1

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_4(_cvspec_, type0, name0, type1, name1) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_5(_cvspec_, type0, name0, type1, name1, type2) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_6(_cvspec_, type0, name0, type1, name1, type2, name2) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_7(_cvspec_, type0, name0, type1, name1, type2, name2, type3) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_8(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_9(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                 type4) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_10(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                  type4, name4) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4 name4

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_11(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                  type4, name4, type5) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4 name4, _cvspec_ type5

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_12(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                  type4, name4, type5, name5) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4 name4, _cvspec_ type5 name5

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_13(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                  type4, name4, type5, name5, type6) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4 name4, _cvspec_ type5 name5, _cvspec_ type6

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_14(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                  type4, name4, type5, name5, type6, name6) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4 name4, _cvspec_ type5 name5, _cvspec_ type6 name6

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_15(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                  type4, name4, type5, name5, type6, name6, type7) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4 name4, _cvspec_ type5 name5, _cvspec_ type6 name6, _cvspec_ type7

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_16(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                  type4, name4, type5, name5, type6, name6, type7, name7) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4 name4, _cvspec_ type5 name5, _cvspec_ type6 name6, _cvspec_ type7 name7

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_17(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                  type4, name4, type5, name5, type6, name6, type7, name7, \
                                                  type8) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4 name4, _cvspec_ type5 name5, _cvspec_ type6 name6, _cvspec_ type7 name7 \
    , _cvspec_ type8

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_18(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                  type4, name4, type5, name5, type6, name6, type7, name7, \
                                                  type8, name8) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4 name4, _cvspec_ type5 name5, _cvspec_ type6 name6, _cvspec_ type7 name7 \
    , _cvspec_ type8 name8

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_19(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                  type4, name4, type5, name5, type6, name6, type7, name7, \
                                                  type8, name8, type9) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4 name4, _cvspec_ type5 name5, _cvspec_ type6 name6, _cvspec_ type7 name7 \
    , _cvspec_ type8 name8, _cvspec_ type9

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_20(_cvspec_, type0, name0, type1, name1, type2, name2, type3, name3, \
                                                  type4, name4, type5, name5, type6, name6, type7, name7, \
                                                  type8, name8, type9, name9) \
    , _cvspec_ type0 name0, _cvspec_ type1 name1, _cvspec_ type2 name2, _cvspec_ type3 name3 \
    , _cvspec_ type4 name4, _cvspec_ type5 name5, _cvspec_ type6 name6, _cvspec_ type7 name7 \
    , _cvspec_ type8 name8, _cvspec_ type9 name9

///////////////////////////////////////

#if defined(UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR)
#   define UHOBGOBLIN_RN_NORMALIZE_ARGS_FINAL(_cvspec_, _num_, ...) \
        UHOBGOBLIN_PP_EXPAND(UHOBGOBLIN_RN_NORMALIZE_ARGS_##_num_(_cvspec_, __VA_ARGS__))
#else
#   define UHOBGOBLIN_RN_NORMALIZE_ARGS_FINAL(_cvspec_, _num_, ...) \
        UHOBGOBLIN_RN_NORMALIZE_ARGS_##_num_(_cvspec_, __VA_ARGS__)
#endif

#define UHOBGOBLIN_RN_NORMALIZE_ARGS_MIDDLE(_cvspec_, _num_, ...) \
    UHOBGOBLIN_RN_NORMALIZE_ARGS_FINAL(_cvspec_, _num_, __VA_ARGS__)

#define UHOBGOBLIN_RN_NORMALIZE_ARGS(_cvspec_, ...) \
    UHOBGOBLIN_RN_NORMALIZE_ARGS_MIDDLE(_cvspec_, HG_PP_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

#endif // !UHOBGOBLIN_RN_MACRO_NORMALIZE_ARGS_HPP