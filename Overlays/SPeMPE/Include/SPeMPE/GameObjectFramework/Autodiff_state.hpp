#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_AUTODIFF_STATE_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_AUTODIFF_STATE_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Preprocessor.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include <cstdint>
#include <string>
#include <utility>

///////////////////////////////////////////////////////////////////////////
// MACROS: GENERATING BASES                                              //
///////////////////////////////////////////////////////////////////////////

#define USPEMPE_ADS_GENERATE_BASES_ERROR \
    "SPeMPE: Error while generating bases for an AutodiffState struct - invalid argument count"

#define USPEMPE_ADS_GENERATE_BASES_0(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_1(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_2(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_4(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_5(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_7(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_8(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_10(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_11(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_13(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_14(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_16(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_17(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_19(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_20(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_22(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_23(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_25(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_26(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_28(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_29(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_31(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_32(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_34(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_35(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_37(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_38(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_40(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_41(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_43(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_44(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_46(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_47(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_49(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_50(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_52(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_53(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_55(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_56(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_58(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_59(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_61(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_62(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_64(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_65(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_67(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_68(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_70(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_71(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_73(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_74(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_76(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_77(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_79(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_80(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_82(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_83(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_85(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_86(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_88(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_89(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_91(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_92(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_94(...) USPEMPE_ADS_GENERATE_BASES_ERROR
#define USPEMPE_ADS_GENERATE_BASES_95(...) USPEMPE_ADS_GENERATE_BASES_ERROR

#define USPEMPE_ADS_GENERATE_BASES_3(_struct_name_, _type0_, _name0_, _init0_) \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_0 { _type0_ _name0_ {_init0_}; } 

#define USPEMPE_ADS_GENERATE_BASES_6(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_) \
    USPEMPE_ADS_GENERATE_BASES_3(_struct_name_, _type0_, _name0_, _init0_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_1 { _type1_ _name1_ {_init1_}; } 

#define USPEMPE_ADS_GENERATE_BASES_9(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_) \
    USPEMPE_ADS_GENERATE_BASES_6(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_2 { _type2_ _name2_ {_init2_}; } 

#define USPEMPE_ADS_GENERATE_BASES_12(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_) \
    USPEMPE_ADS_GENERATE_BASES_9(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_3 { _type3_ _name3_ {_init3_}; } 

#define USPEMPE_ADS_GENERATE_BASES_15(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_) \
    USPEMPE_ADS_GENERATE_BASES_12(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_4 { _type4_ _name4_ {_init4_}; } 

#define USPEMPE_ADS_GENERATE_BASES_18(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_) \
    USPEMPE_ADS_GENERATE_BASES_15(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_5 { _type5_ _name5_ {_init5_}; } 

#define USPEMPE_ADS_GENERATE_BASES_21(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_) \
    USPEMPE_ADS_GENERATE_BASES_18(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_6 { _type6_ _name6_ {_init6_}; } 

#define USPEMPE_ADS_GENERATE_BASES_24(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_) \
    USPEMPE_ADS_GENERATE_BASES_21(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_7 { _type7_ _name7_ {_init7_}; } 

#define USPEMPE_ADS_GENERATE_BASES_27(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_) \
    USPEMPE_ADS_GENERATE_BASES_24(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_8 { _type8_ _name8_ {_init8_}; } 

#define USPEMPE_ADS_GENERATE_BASES_30(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_) \
    USPEMPE_ADS_GENERATE_BASES_27(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_9 { _type9_ _name9_ {_init9_}; } 

#define USPEMPE_ADS_GENERATE_BASES_33(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_) \
    USPEMPE_ADS_GENERATE_BASES_30(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_10 { _type10_ _name10_ {_init10_}; } 

#define USPEMPE_ADS_GENERATE_BASES_36(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_) \
    USPEMPE_ADS_GENERATE_BASES_33(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_11 { _type11_ _name11_ {_init11_}; } 

#define USPEMPE_ADS_GENERATE_BASES_39(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_) \
    USPEMPE_ADS_GENERATE_BASES_36(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_12 { _type12_ _name12_ {_init12_}; } 

#define USPEMPE_ADS_GENERATE_BASES_42(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_) \
    USPEMPE_ADS_GENERATE_BASES_39(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_13 { _type13_ _name13_ {_init13_}; } 

#define USPEMPE_ADS_GENERATE_BASES_45(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_) \
    USPEMPE_ADS_GENERATE_BASES_42(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_14 { _type14_ _name14_ {_init14_}; } 

#define USPEMPE_ADS_GENERATE_BASES_48(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_) \
    USPEMPE_ADS_GENERATE_BASES_45(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_15 { _type15_ _name15_ {_init15_}; } 

#define USPEMPE_ADS_GENERATE_BASES_51(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_) \
    USPEMPE_ADS_GENERATE_BASES_48(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_16 { _type16_ _name16_ {_init16_}; } 

#define USPEMPE_ADS_GENERATE_BASES_54(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_) \
    USPEMPE_ADS_GENERATE_BASES_51(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_17 { _type17_ _name17_ {_init17_}; } 

#define USPEMPE_ADS_GENERATE_BASES_57(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_) \
    USPEMPE_ADS_GENERATE_BASES_54(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_18 { _type18_ _name18_ {_init18_}; } 

#define USPEMPE_ADS_GENERATE_BASES_60(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_) \
    USPEMPE_ADS_GENERATE_BASES_57(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_19 { _type19_ _name19_ {_init19_}; } 

#define USPEMPE_ADS_GENERATE_BASES_63(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_) \
    USPEMPE_ADS_GENERATE_BASES_60(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_20 { _type20_ _name20_ {_init20_}; } 

#define USPEMPE_ADS_GENERATE_BASES_66(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_) \
    USPEMPE_ADS_GENERATE_BASES_63(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_21 { _type21_ _name21_ {_init21_}; } 

#define USPEMPE_ADS_GENERATE_BASES_69(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_) \
    USPEMPE_ADS_GENERATE_BASES_66(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_22 { _type22_ _name22_ {_init22_}; } 

#define USPEMPE_ADS_GENERATE_BASES_72(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_) \
    USPEMPE_ADS_GENERATE_BASES_69(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_23 { _type23_ _name23_ {_init23_}; } 

#define USPEMPE_ADS_GENERATE_BASES_75(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_) \
    USPEMPE_ADS_GENERATE_BASES_72(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_24 { _type24_ _name24_ {_init24_}; } 

#define USPEMPE_ADS_GENERATE_BASES_78(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_) \
    USPEMPE_ADS_GENERATE_BASES_75(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_25 { _type25_ _name25_ {_init25_}; } 

#define USPEMPE_ADS_GENERATE_BASES_81(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_) \
    USPEMPE_ADS_GENERATE_BASES_78(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_26 { _type26_ _name26_ {_init26_}; } 

#define USPEMPE_ADS_GENERATE_BASES_84(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_) \
    USPEMPE_ADS_GENERATE_BASES_81(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_27 { _type27_ _name27_ {_init27_}; } 

#define USPEMPE_ADS_GENERATE_BASES_87(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_) \
    USPEMPE_ADS_GENERATE_BASES_84(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_28 { _type28_ _name28_ {_init28_}; } 

#define USPEMPE_ADS_GENERATE_BASES_90(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_, _type29_, _name29_, _init29_) \
    USPEMPE_ADS_GENERATE_BASES_87(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_29 { _type29_ _name29_ {_init29_}; } 

#define USPEMPE_ADS_GENERATE_BASES_93(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_, _type29_, _name29_, _init29_, _type30_, _name30_, _init30_) \
    USPEMPE_ADS_GENERATE_BASES_90(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_, _type29_, _name29_, _init29_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_30 { _type30_ _name30_ {_init30_}; } 

#define USPEMPE_ADS_GENERATE_BASES_96(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_, _type29_, _name29_, _init29_, _type30_, _name30_, _init30_, _type31_, _name31_, _init31_) \
    USPEMPE_ADS_GENERATE_BASES_93(_struct_name_, _type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_, _type29_, _name29_, _init29_, _type30_, _name30_, _init30_); \
    struct USPEMPE_AutodiffState_##_struct_name_##_Base_31 { _type31_ _name31_ {_init31_}; } 


#if defined(UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR)

#define USPEMPE_ADS_GENERATE_BASES_FINAL(_struct_name_, _num_, ...) \
    UHOBGOBLIN_PP_EXPAND(USPEMPE_ADS_GENERATE_BASES_##_num_(_struct_name_, __VA_ARGS__))

#else

#define USPEMPE_ADS_GENERATE_BASES_FINAL(_struct_name_, _num_, ...) \
    USPEMPE_ADS_GENERATE_BASES_##_num_(_struct_name_, __VA_ARGS__)

#endif

#define USPEMPE_ADS_GENERATE_BASES_MIDDLE(_struct_name_, _num_, ...) \
    USPEMPE_ADS_GENERATE_BASES_FINAL(_struct_name_, _num_, __VA_ARGS__)

#define USPEMPE_ADS_GENERATE_BASES(_struct_name_, ...) \
    USPEMPE_ADS_GENERATE_BASES_MIDDLE(_struct_name_, HG_PP_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////
// MACROS: PASSING BASES                                                 //
///////////////////////////////////////////////////////////////////////////

#define USPEMPE_ADS_PASS_BASES_ERROR \
    "SPeMPE: Error while passing bases for an AutodiffState struct - invalid argument count"

#define USPEMPE_ADS_PASS_BASES_0(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_1(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_2(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_4(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_5(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_7(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_8(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_10(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_11(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_13(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_14(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_16(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_17(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_19(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_20(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_22(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_23(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_25(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_26(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_28(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_29(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_31(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_32(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_34(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_35(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_37(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_38(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_40(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_41(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_43(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_44(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_46(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_47(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_49(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_50(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_52(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_53(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_55(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_56(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_58(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_59(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_61(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_62(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_64(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_65(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_67(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_68(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_70(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_71(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_73(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_74(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_76(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_77(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_79(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_80(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_82(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_83(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_85(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_86(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_88(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_89(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_91(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_92(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_94(...) USPEMPE_ADS_PASS_BASES_ERROR
#define USPEMPE_ADS_PASS_BASES_95(...) USPEMPE_ADS_PASS_BASES_ERROR

#define USPEMPE_ADS_PASS_BASES_3(_struct_name_) \
    USPEMPE_AutodiffState_##_struct_name_##_Base_0

#define USPEMPE_ADS_PASS_BASES_6(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_3(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_1

#define USPEMPE_ADS_PASS_BASES_9(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_6(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_2

#define USPEMPE_ADS_PASS_BASES_12(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_9(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_3

#define USPEMPE_ADS_PASS_BASES_15(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_12(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_4

#define USPEMPE_ADS_PASS_BASES_18(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_15(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_5

#define USPEMPE_ADS_PASS_BASES_21(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_18(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_6

#define USPEMPE_ADS_PASS_BASES_24(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_21(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_7

#define USPEMPE_ADS_PASS_BASES_27(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_24(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_8

#define USPEMPE_ADS_PASS_BASES_30(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_27(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_9

#define USPEMPE_ADS_PASS_BASES_33(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_30(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_10

#define USPEMPE_ADS_PASS_BASES_36(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_33(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_11

#define USPEMPE_ADS_PASS_BASES_39(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_36(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_12

#define USPEMPE_ADS_PASS_BASES_42(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_39(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_13

#define USPEMPE_ADS_PASS_BASES_45(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_42(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_14

#define USPEMPE_ADS_PASS_BASES_48(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_45(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_15

#define USPEMPE_ADS_PASS_BASES_51(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_48(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_16

#define USPEMPE_ADS_PASS_BASES_54(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_51(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_17

#define USPEMPE_ADS_PASS_BASES_57(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_54(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_18

#define USPEMPE_ADS_PASS_BASES_60(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_57(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_19

#define USPEMPE_ADS_PASS_BASES_63(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_60(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_20

#define USPEMPE_ADS_PASS_BASES_66(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_63(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_21

#define USPEMPE_ADS_PASS_BASES_69(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_66(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_22

#define USPEMPE_ADS_PASS_BASES_72(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_69(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_23

#define USPEMPE_ADS_PASS_BASES_75(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_72(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_24

#define USPEMPE_ADS_PASS_BASES_78(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_75(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_25

#define USPEMPE_ADS_PASS_BASES_81(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_78(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_26

#define USPEMPE_ADS_PASS_BASES_84(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_81(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_27

#define USPEMPE_ADS_PASS_BASES_87(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_84(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_28

#define USPEMPE_ADS_PASS_BASES_90(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_87(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_29

#define USPEMPE_ADS_PASS_BASES_93(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_90(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_30

#define USPEMPE_ADS_PASS_BASES_96(_struct_name_) \
    USPEMPE_ADS_PASS_BASES_93(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_31


#if defined(UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR)

#define USPEMPE_ADS_PASS_BASES_FINAL(_struct_name_, _num_) \
    UHOBGOBLIN_PP_EXPAND(USPEMPE_ADS_PASS_BASES_##_num_(_struct_name_))

#else

#define USPEMPE_ADS_PASS_BASES_FINAL(_struct_name_, _num_) \
    USPEMPE_ADS_PASS_BASES_##_num_(_struct_name_)

#endif

#define USPEMPE_ADS_PASS_BASES_MIDDLE(_struct_name_, _num_) \
    USPEMPE_ADS_PASS_BASES_FINAL(_struct_name_, _num_)

#define USPEMPE_ADS_PASS_BASES(_struct_name_, ...) \
    USPEMPE_ADS_PASS_BASES_MIDDLE(_struct_name_, HG_PP_COUNT_ARGS(__VA_ARGS__))

///////////////////////////////////////////////////////////////////////////
// MACROS: PASSING MEMBER REFERENCES                                     //
///////////////////////////////////////////////////////////////////////////

#define USPEMPE_ADS_MIRROR_OBJECT_TYPE __spempeimpl_mirror_t
#define USPEMPE_ADS_MIRROR_OBJECT_NAME __spempeimpl_mirrorObject

#define USPEMPE_ADS_PASS_MEMBER_REFS_ERROR \
    "SPeMPE: Error while passing member references for an AutodiffState struct - invalid argument count"

#define USPEMPE_ADS_PASS_MEMBER_REFS_0(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_1(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_2(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_4(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_5(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_7(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_8(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_10(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_11(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_13(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_14(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_16(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_17(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_19(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_20(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_22(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_23(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_25(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_26(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_28(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_29(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_31(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_32(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_34(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_35(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_37(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_38(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_40(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_41(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_43(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_44(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_46(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_47(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_49(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_50(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_52(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_53(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_55(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_56(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_58(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_59(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_61(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_62(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_64(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_65(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_67(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_68(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_70(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_71(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_73(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_74(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_76(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_77(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_79(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_80(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_82(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_83(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_85(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_86(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_88(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_89(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_91(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_92(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_94(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR
#define USPEMPE_ADS_PASS_MEMBER_REFS_95(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR

#define USPEMPE_ADS_PASS_MEMBER_REFS_3(_type0_, _name0_, _init0_) \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name0_, _name0_

#define USPEMPE_ADS_PASS_MEMBER_REFS_6(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_3(_type0_, _name0_, _init0_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name1_, _name1_

#define USPEMPE_ADS_PASS_MEMBER_REFS_9(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_6(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name2_, _name2_

#define USPEMPE_ADS_PASS_MEMBER_REFS_12(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_9(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name3_, _name3_

#define USPEMPE_ADS_PASS_MEMBER_REFS_15(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_12(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name4_, _name4_

#define USPEMPE_ADS_PASS_MEMBER_REFS_18(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_15(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name5_, _name5_

#define USPEMPE_ADS_PASS_MEMBER_REFS_21(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_18(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name6_, _name6_

#define USPEMPE_ADS_PASS_MEMBER_REFS_24(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_21(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name7_, _name7_

#define USPEMPE_ADS_PASS_MEMBER_REFS_27(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_24(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name8_, _name8_

#define USPEMPE_ADS_PASS_MEMBER_REFS_30(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_27(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name9_, _name9_

#define USPEMPE_ADS_PASS_MEMBER_REFS_33(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_30(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name10_, _name10_

#define USPEMPE_ADS_PASS_MEMBER_REFS_36(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_33(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name11_, _name11_

#define USPEMPE_ADS_PASS_MEMBER_REFS_39(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_36(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name12_, _name12_

#define USPEMPE_ADS_PASS_MEMBER_REFS_42(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_39(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name13_, _name13_

#define USPEMPE_ADS_PASS_MEMBER_REFS_45(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_42(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name14_, _name14_

#define USPEMPE_ADS_PASS_MEMBER_REFS_48(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_45(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name15_, _name15_

#define USPEMPE_ADS_PASS_MEMBER_REFS_51(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_48(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name16_, _name16_

#define USPEMPE_ADS_PASS_MEMBER_REFS_54(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_51(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name17_, _name17_

#define USPEMPE_ADS_PASS_MEMBER_REFS_57(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_54(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name18_, _name18_

#define USPEMPE_ADS_PASS_MEMBER_REFS_60(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_57(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name19_, _name19_

#define USPEMPE_ADS_PASS_MEMBER_REFS_63(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_60(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name20_, _name20_

#define USPEMPE_ADS_PASS_MEMBER_REFS_66(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_63(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name21_, _name21_

#define USPEMPE_ADS_PASS_MEMBER_REFS_69(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_66(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name22_, _name22_

#define USPEMPE_ADS_PASS_MEMBER_REFS_72(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_69(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name23_, _name23_

#define USPEMPE_ADS_PASS_MEMBER_REFS_75(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_72(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name24_, _name24_

#define USPEMPE_ADS_PASS_MEMBER_REFS_78(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_75(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name25_, _name25_

#define USPEMPE_ADS_PASS_MEMBER_REFS_81(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_78(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name26_, _name26_

#define USPEMPE_ADS_PASS_MEMBER_REFS_84(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_81(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name27_, _name27_

#define USPEMPE_ADS_PASS_MEMBER_REFS_87(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_84(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name28_, _name28_

#define USPEMPE_ADS_PASS_MEMBER_REFS_90(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_, _type29_, _name29_, _init29_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_87(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name29_, _name29_

#define USPEMPE_ADS_PASS_MEMBER_REFS_93(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_, _type29_, _name29_, _init29_, _type30_, _name30_, _init30_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_90(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_, _type29_, _name29_, _init29_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name30_, _name30_

#define USPEMPE_ADS_PASS_MEMBER_REFS_96(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_, _type29_, _name29_, _init29_, _type30_, _name30_, _init30_, _type31_, _name31_, _init31_) \
    USPEMPE_ADS_PASS_MEMBER_REFS_93(_type0_, _name0_, _init0_, _type1_, _name1_, _init1_, _type2_, _name2_, _init2_, _type3_, _name3_, _init3_, _type4_, _name4_, _init4_, _type5_, _name5_, _init5_, _type6_, _name6_, _init6_, _type7_, _name7_, _init7_, _type8_, _name8_, _init8_, _type9_, _name9_, _init9_, _type10_, _name10_, _init10_, _type11_, _name11_, _init11_, _type12_, _name12_, _init12_, _type13_, _name13_, _init13_, _type14_, _name14_, _init14_, _type15_, _name15_, _init15_, _type16_, _name16_, _init16_, _type17_, _name17_, _init17_, _type18_, _name18_, _init18_, _type19_, _name19_, _init19_, _type20_, _name20_, _init20_, _type21_, _name21_, _init21_, _type22_, _name22_, _init22_, _type23_, _name23_, _init23_, _type24_, _name24_, _init24_, _type25_, _name25_, _init25_, _type26_, _name26_, _init26_, _type27_, _name27_, _init27_, _type28_, _name28_, _init28_, _type29_, _name29_, _init29_, _type30_, _name30_, _init30_), \
    USPEMPE_ADS_MIRROR_OBJECT_NAME._name31_, _name31_


#if defined(UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR)

#define USPEMPE_ADS_PASS_MEMBER_REFS_FINAL(_num_, ...) \
    UHOBGOBLIN_PP_EXPAND(USPEMPE_ADS_PASS_MEMBER_REFS_##_num_(__VA_ARGS__))

#else

#define USPEMPE_ADS_PASS_MEMBER_REFS_FINAL(_num_, ...) \
    USPEMPE_ADS_PASS_MEMBER_REFS_##_num_(__VA_ARGS__)

#endif

#define USPEMPE_ADS_PASS_MEMBER_REFS_MIDDLE(_num_, ...) \
    USPEMPE_ADS_PASS_MEMBER_REFS_FINAL(_num_, __VA_ARGS__)

#define USPEMPE_ADS_PASS_MEMBER_REFS(...) \
    USPEMPE_ADS_PASS_MEMBER_REFS_MIDDLE(HG_PP_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////
// C++                                                                   //
///////////////////////////////////////////////////////////////////////////

#define USPEMPE_ADS_BITS_TYPE __spempeimpl_bits_t

namespace jbatnozic {
namespace spempe {
namespace detail {

namespace hg = jbatnozic::hobgoblin;

template <int taBitCount>
class StaticPackableBitset {
public:
    StaticPackableBitset() {
        clearAll();
    }

    void setBit(hg::PZInteger aByteIdx, bool aValue) {
        if (aValue) {
            _bytes[aByteIdx / CHAR_BIT] |= (1 << (aByteIdx % CHAR_BIT));
        }
        else {
            _bytes[aByteIdx / CHAR_BIT] &= ~(1 << (aByteIdx % CHAR_BIT));
        }
    }

    bool getBit(hg::PZInteger aByteIdx) {
        return (_bytes[aByteIdx / CHAR_BIT] & (1 << (aByteIdx % CHAR_BIT))) != 0;
    }

    void clearAll() {
        std::memset(_bytes, 0x00, BYTE_COUNT);
    }

    friend hg::util::PacketBase& operator<<(hg::util::PacketBase& aPacket, const StaticPackableBitset& aBitset) {
        for (auto byte : aBitset._bytes) {
            aPacket << byte;
        }
        return aPacket;
    }

    friend hg::util::PacketBase& operator>>(hg::util::PacketBase& aPacket, StaticPackableBitset& aBitset) {
        for (auto& byte : aBitset._bytes) {
            aPacket >> byte;
        }
        return aPacket;
    }

private:
    static_assert(sizeof(char) == sizeof(std::int8_t), "");
    static constexpr auto BYTE_COUNT = hg::math::IntegralCeilDiv<int>(taBitCount, CHAR_BIT);
    std::int8_t _bytes[BYTE_COUNT];
};

template <int taBaseCount, class... taBases>
class AutodiffStateUpperBase : public taBases... {
protected:
    struct USPEMPE_ADS_MIRROR_OBJECT_TYPE : public taBases... {} USPEMPE_ADS_MIRROR_OBJECT_NAME;
    using  USPEMPE_ADS_BITS_TYPE = StaticPackableBitset<taBaseCount>;
};

constexpr int CountBases(int argc) {
    // Note: It doesn't matter which type we throw in the else branch, because
    // throwing exceptions is not constexpr and will fail to compile.
    return (argc > 0 && argc % 3 == 0) ? (argc / 3) : (throw new void*);
}

//! Commits all changes in the autodiff object, meaning that future diffs
//! will be compared to the new state.
template <class taMember, class... taRest>
void AutodiffStateCommit(taMember& aMirror, const taMember& aReal, taRest&&... aRest) {
    aMirror = aReal;
    if constexpr (sizeof...(taRest) > 0) {
        AutodiffStateCommit(std::forward<taRest>(aRest)...);
    }
}

//! Packs the diff of this object into the packet.
template <class taBits, class taMember, class... taRest>
void AutodiffStatePackDiff(
    hg::util::PacketBase& aPacket,
    taBits& aBits,
    hg::PZInteger aDepth,
    const taMember& aMirror,
    const taMember& aReal, 
    taRest&&... aRest)
{
    if constexpr (sizeof...(taRest) == 0) { // Is last recursion?
        if (aReal != aMirror) {
            aBits.setBit(aDepth, true);
        }
        aPacket << aBits;
        if (aReal != aMirror) {
            aPacket << aReal;
        }
    } else {
        if (aReal != aMirror) {
            aBits.setBit(aDepth, true);
        }
        AutodiffStatePackDiff(aPacket, aBits, aDepth + 1, std::forward<taRest>(aRest)...);
        if (aReal != aMirror) {
            aPacket << aReal;
        }
    }
}

template <class taBits, class taMember, class... taRest>
void AutodiffStatePackAll(
    hg::util::PacketBase& aPacket,
    taBits& aBits,
    hg::PZInteger aDepth,
    const taMember& aMirror,
    const taMember& aReal, 
    taRest&&... aRest)
{
    if constexpr (sizeof...(taRest) == 0) { // Is last recursion?
        aBits.setBit(aDepth, true);
        aPacket << aBits;
        aPacket << aReal;
    } else {
        aBits.setBit(aDepth, true);
        AutodiffStatePackAll(aPacket, aBits, aDepth + 1, std::forward<taRest>(aRest)...);
        aPacket << aReal;
    }
}

template <class taBits, class taMember, class... taRest>
void AutodiffStateUnpack(
    hg::util::PacketBase& aPacket,
    taBits& aBits,
    hg::PZInteger aDepth,
    const taMember& aMirror,
    taMember& aReal, 
    taRest&&... aRest)
{
    if constexpr (sizeof...(taRest) == 0) { // Is last recursion?
        aPacket >> aBits;
        if (aBits.getBit(aDepth)) {
            aPacket >> aReal;
            (void)aMirror;
        }
    } else {
        AutodiffStateUnpack(aPacket, aBits, aDepth + 1, std::forward<taRest>(aRest)...);
        if (aBits.getBit(aDepth)) {
            aPacket >> aReal;
            (void)aMirror;
        }
    }
}

constexpr bool NO_CHANGE  = false;
constexpr bool HAS_CHANGE = true;

template <class taMember, class... taRest>
bool AutodiffStateCmp(
    const taMember& aMirror,
    const taMember& aReal, 
    taRest&&... aRest)
{
    if (aReal != aMirror) {
        return HAS_CHANGE;
    }
    if constexpr (sizeof...(taRest) > 0) {
        return AutodiffStateCmp(std::forward<taRest>(aRest)...);
    } else {
        return NO_CHANGE;
    }
}

} // namespace detail
} // namespace spempe
} // namespace jbatnozic

///////////////////////////////////////////////////////////////////////////
// MACROS: USER                                                          //
///////////////////////////////////////////////////////////////////////////

#define SPEMPE_DEFINE_AUTODIFF_STATE(_struct_name_, ...) \
    USPEMPE_ADS_GENERATE_BASES(_struct_name_, __VA_ARGS__); \
    struct USPEMPE_AutodiffState_##_struct_name_##_LowerBase \
        : public ::jbatnozic::spempe::detail::AutodiffStateUpperBase< \
            ::jbatnozic::spempe::detail::CountBases(HG_PP_COUNT_ARGS(__VA_ARGS__)), \
            USPEMPE_ADS_PASS_BASES(_struct_name_, __VA_ARGS__) \
        > { \
        using Super = USPEMPE_AutodiffState_##_struct_name_##_LowerBase; \
        \
        void commit() { \
            ::jbatnozic::spempe::detail::AutodiffStateCommit(USPEMPE_ADS_PASS_MEMBER_REFS(__VA_ARGS__)); \
        } \
        void packDiff(::jbatnozic::hobgoblin::util::PacketBase& aPacket) const { \
            USPEMPE_ADS_BITS_TYPE bits; \
            ::jbatnozic::spempe::detail::AutodiffStatePackDiff(aPacket, bits, 0, USPEMPE_ADS_PASS_MEMBER_REFS(__VA_ARGS__)); \
        } \
        void packAll(::jbatnozic::hobgoblin::util::PacketBase& aPacket) const { \
            USPEMPE_ADS_BITS_TYPE bits; \
            ::jbatnozic::spempe::detail::AutodiffStatePackAll(aPacket, bits, 0, USPEMPE_ADS_PASS_MEMBER_REFS(__VA_ARGS__)); \
        } \
        void unpack(::jbatnozic::hobgoblin::util::PacketBase& aPacket) { \
            USPEMPE_ADS_BITS_TYPE bits; \
            ::jbatnozic::spempe::detail::AutodiffStateUnpack(aPacket, bits, 0, USPEMPE_ADS_PASS_MEMBER_REFS(__VA_ARGS__)); \
        }; \
        bool cmp() const { \
            return ::jbatnozic::spempe::detail::AutodiffStateCmp(USPEMPE_ADS_PASS_MEMBER_REFS(__VA_ARGS__)); \
        }; \
        \
        friend \
        ::jbatnozic::hobgoblin::util::PacketBase& operator>>(::jbatnozic::hobgoblin::util::PacketBase& aPacket, Super& aSelf) { \
            aSelf.unpack(aPacket); return aPacket; \
        }; \
    }; \
    struct _struct_name_ : public USPEMPE_AutodiffState_##_struct_name_##_LowerBase

#define SPEMPE_MEMBER(_type_, _name_, _default_value_) \
    _type_, _name_, _default_value_

#define SPEMPE_AUTODIFF_STATE_NO_CHANGE \
    ::jbatnozic::spempe::detail::NO_CHANGE

#define SPEMPE_AUTODIFF_STATE_HAS_CHANGE \
    ::jbatnozic::spempe::detail::HAS_CHANGE

///////////////////////////////////////////////////////////////////////////
// GENERATOR: GENERATE_BASES                                             //
///////////////////////////////////////////////////////////////////////////

#if 0
#include <iostream>
#include <sstream>
#include <string>
#include <functional>

int main() {
    for (int argc = 0; argc <= 96; argc += 1) {
        if (argc == 0 || argc % 3 != 0) {
            std::cout <<
                "#define USPEMPE_ADS_GENERATE_BASES_" << argc << "(...) USPEMPE_ADS_GENERATE_BASES_ERROR\n";
        }    
    }

    for (int argc = 0; argc <= 96; argc += 1) {
        if (argc == 0 || argc % 3 != 0) {
            continue;
        }

        // Params
        const std::string nm3params = [limit = argc / 3]() {
            std::stringstream ss;
            for (int i = 0; i < limit - 1; i += 1) {
                ss << "_type" << i << "_, "
                    << "_name" << i << "_, "
                    << "_init" << i << "_";
                if (i < limit - 2) {
                    ss << ", ";
                }
            }
            return ss.str();
        }();
        const std::string last3params = [limit = argc / 3]() {
            const auto i = limit - 1;
            std::stringstream ss;
            ss << "_type" << i << "_, "
                << "_name" << i << "_, "
                << "_init" << i << "_";
            return ss.str();
        }();

        // Declaration
        std::cout
            << "#define USPEMPE_ADS_GENERATE_BASES_" << argc << "(_struct_name_, "
            << nm3params + ", " + last3params << ") \\\n";

        // Body
        std::cout
            << "    USPEMPE_ADS_GENERATE_BASES_" << (argc - 3)
            << "(_struct_name_, " << nm3params << "); \\\n";
        std::printf(
            "    struct USPEMPE_AutodiffState_##_struct_name_##_Base_%d { _type%d_ _name%d_ {_init%d_}; } \n",
            argc / 3 - 1, argc / 3 - 1, argc / 3 - 1, argc / 3 - 1
        );
        std::cout << "\n";
    }
}
#endif

///////////////////////////////////////////////////////////////////////////
// GENERATOR: PASS_BASES                                                 //
///////////////////////////////////////////////////////////////////////////

#if 0
#include <iostream>
#include <string>

int main() {
    for (int argc = 0; argc <= 96; argc += 1) {
        if (argc == 0 || argc % 3 != 0) {
            std::cout <<
                "#define USPEMPE_ADS_PASS_BASES_" << argc << "(...) USPEMPE_ADS_PASS_BASES_ERROR\n";
        }    
    }

    for (int argc = 0; argc <= 96; argc += 1) {
        if (argc == 0 || argc % 3 != 0) {
            continue;
        }

        // Declaration
        std::cout
            << "#define USPEMPE_ADS_PASS_BASES_" << argc << "(_struct_name_) \\\n";

        // Body
        std::printf(
            "    PASS_BASES_%d(_struct_name_), USPEMPE_AutodiffState_##_struct_name_##_Base_%d",
            argc - 3, argc / 3 - 1
        );
        std::cout << "\n";
    }
}
#endif

///////////////////////////////////////////////////////////////////////////
// GENERATOR: PASS_MEMBER_REFS                                           //
///////////////////////////////////////////////////////////////////////////

#if 0
#include <iostream>
#include <sstream>
#include <string>
#include <functional>

int main() {
    for (int argc = 0; argc <= 96; argc += 1) {
        if (argc == 0 || argc % 3 != 0) {
            std::cout <<
                "#define USPEMPE_ADS_PASS_MEMBER_REFS_" << argc << "(...) USPEMPE_ADS_PASS_MEMBER_REFS_ERROR\n";
        }    
    }

    for (int argc = 0; argc <= 96; argc += 1) {
        if (argc == 0 || argc % 3 != 0) {
            continue;
        }

        // Params
        const std::string nm3params = [limit = argc / 3]() {
            std::stringstream ss;
            for (int i = 0; i < limit - 1; i += 1) {
                ss << "_type" << i << "_, "
                    << "_name" << i << "_, "
                    << "_init" << i << "_";
                if (i < limit - 2) {
                    ss << ", ";
                }
            }
            return ss.str();
        }();
        const std::string last3params = [limit = argc / 3]() {
            const auto i = limit - 1;
            std::stringstream ss;
            ss << "_type" << i << "_, "
                << "_name" << i << "_, "
                << "_init" << i << "_";
            return ss.str();
        }();

        // Declaration
        std::cout
            << "#define USPEMPE_ADS_PASS_MEMBER_REFS_"
            << argc << "(" << nm3params << ", " << last3params << ") \\\n";

        // Body
        std::cout
            << "    USPEMPE_ADS_PASS_MEMBER_REFS_" << (argc - 3) << "(" << nm3params << "), \\\n";

        std::printf(
            "    USPEMPE_ADS_MIRROR_OBJECT_NAME._name%d_, _name%d_\n",
            argc / 3 - 1, argc / 3 - 1
        );
        std::cout << "\n";
    }
}
#endif

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_AUTODIFF_STATE_HPP
