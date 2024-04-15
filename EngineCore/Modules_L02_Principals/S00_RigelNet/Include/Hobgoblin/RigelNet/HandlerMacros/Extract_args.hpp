// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_RN_HANDLER_MACROS_EXTRACT_ARGS_HPP
#define UHOBGOBLIN_RN_HANDLER_MACROS_EXTRACT_ARGS_HPP

#include <Hobgoblin/Preprocessor.hpp>

#include <type_traits>

#define UHOBGOBLIN_RN_EXTRACT_ARGS_0() /* Nothing */

#define UHOBGOBLIN_RN_EXTRACT_ARGS_1(type0) \
    std::remove_reference<type0>::type arg0 = ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ExtractArg<type0>(node);

#define UHOBGOBLIN_RN_EXTRACT_ARGS_2(type0, name0) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_1(type0)

#define UHOBGOBLIN_RN_EXTRACT_ARGS_3(type0, name0, type1) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_2(type0, name0) \
    std::remove_reference<type1>::type arg1 = ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ExtractArg<type1>(node);

#define UHOBGOBLIN_RN_EXTRACT_ARGS_4(type0, name0, type1, name1) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_3(type0, name0, type1)

#define UHOBGOBLIN_RN_EXTRACT_ARGS_5(type0, name0, type1, name1, type2) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_4(type0, name0, type1, name1) \
    std::remove_reference<type2>::type arg2 = ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ExtractArg<type2>(node);

#define UHOBGOBLIN_RN_EXTRACT_ARGS_6(type0, name0, type1, name1, type2, name2) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_5(type0, name0, type1, name1, type2)

#define UHOBGOBLIN_RN_EXTRACT_ARGS_7(type0, name0, type1, name1, type2, name2, type3) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_6(type0, name0, type1, name1, type2, name2) \
    std::remove_reference<type3>::type arg3 = ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ExtractArg<type3>(node);

#define UHOBGOBLIN_RN_EXTRACT_ARGS_8(type0, name0, type1, name1, type2, name2, type3, name3) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_7(type0, name0, type1, name1, type2, name2, type3)

#define UHOBGOBLIN_RN_EXTRACT_ARGS_9(type0, name0, type1, name1, type2, name2, type3, name3, type4) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_8(type0, name0, type1, name1, type2, name2, type3, name3) \
    std::remove_reference<type4>::type arg4 = ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ExtractArg<type4>(node);

#define UHOBGOBLIN_RN_EXTRACT_ARGS_10(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_9(type0, name0, type1, name1, type2, name2, type3, name3, type4)

#define UHOBGOBLIN_RN_EXTRACT_ARGS_11(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_10(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4) \
    std::remove_reference<type5>::type arg5 = ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ExtractArg<type5>(node);

#define UHOBGOBLIN_RN_EXTRACT_ARGS_12(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_11(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5)

#define UHOBGOBLIN_RN_EXTRACT_ARGS_13(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_12(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5) \
    std::remove_reference<type6>::type arg6 = ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ExtractArg<type6>(node);

#define UHOBGOBLIN_RN_EXTRACT_ARGS_14(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_13(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6)

#define UHOBGOBLIN_RN_EXTRACT_ARGS_15(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_14(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6) \
    std::remove_reference<type7>::type arg7 = ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ExtractArg<type7>(node);

#define UHOBGOBLIN_RN_EXTRACT_ARGS_16(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_15(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7)

#define UHOBGOBLIN_RN_EXTRACT_ARGS_17(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, \
                                      type8) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_16(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7) \
    std::remove_reference<type8>::type arg8 = ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ExtractArg<type8>(node);

#define UHOBGOBLIN_RN_EXTRACT_ARGS_18(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, \
                                      type8, name8) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_17(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, \
                                  type8)

#define UHOBGOBLIN_RN_EXTRACT_ARGS_19(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, \
                                      type8, name8, type9) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_18(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, \
                                  type8, name8) \
    std::remove_reference<type9>::type arg9 = ::jbatnozic::hobgoblin::rn::UHOBGOBLIN_RN_ExtractArg<type9>(node);

#define UHOBGOBLIN_RN_EXTRACT_ARGS_20(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, \
                                      type8, name8, type9, name9) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_19(type0, name0, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, \
                                  type8, name8, type9)

///////////////////////////////////////

#if defined(UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR)
#   define UHOBGOBLIN_RN_EXTRACT_ARGS_FINAL(_num_, ...) \
        UHOBGOBLIN_PP_EXPAND(UHOBGOBLIN_RN_EXTRACT_ARGS_##_num_(__VA_ARGS__))
#else
#   define UHOBGOBLIN_RN_EXTRACT_ARGS_FINAL(_num_, ...) \
        UHOBGOBLIN_RN_EXTRACT_ARGS_##_num_(__VA_ARGS__)
#endif

#define UHOBGOBLIN_RN_EXTRACT_ARGS_MIDDLE(_num_, ...) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_FINAL(_num_, __VA_ARGS__)

#define UHOBGOBLIN_RN_EXTRACT_ARGS(...) \
    UHOBGOBLIN_RN_EXTRACT_ARGS_MIDDLE(HG_PP_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

#endif // !UHOBGOBLIN_RN_HANDLER_MACROS_EXTRACT_ARGS_HPP

// clang-format on
