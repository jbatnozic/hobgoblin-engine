// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

// Note: Header guard omitted on purpose.
// IMPORTANT: Always include this as the last header file

///////////////////////////////////////////////////////////////////////////
// COMPILER-SPECIFIC                                                     //
///////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#define CURRENT_FUNCTION __FUNCSIG__
#define SWITCH_FALLTHROUGH (void)0
#else
#define CURRENT_FUNCTION __PRETTY_FUNCTION__
#define SWITCH_FALLTHROUGH (void)0 /* TEMP */
#endif

///////////////////////////////////////////////////////////////////////////
// DYNAMIC LIB API                                                       //
///////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
    #ifdef UHOBGOBLIN_EXPORT
        #define HG_DYNAPI __declspec(dllexport)
    #else
        #define HG_DYNAPI __declspec(dllimport)
    #endif
    #define HGCALL __cdecl
#else
    #define HG_DYNAPI
    #define HGCALL
#endif

///////////////////////////////////////////////////////////////////////////
// GENERAL                                                               //
///////////////////////////////////////////////////////////////////////////

// Namespace:
#define HOBGOBLIN_NAMESPACE_BEGIN namespace jbatnozic { namespace hobgoblin {
#define HOBGOBLIN_NAMESPACE_END }}

// Preprocessor magic:
#define MDEFER(_x_) _x_
#define MSTRINGIFY(_x_) #_x_

// Utility:
#define CURRENT_FUNCTION_STR std::string{CURRENT_FUNCTION}

#define HG_NODISCARD [[nodiscard]]
#define NO_COPY public util::NonCopyable
#define NO_MOVE public util::NonMoveable
#define NO_OP() (void)0
#define SELF (*this)

// SFINAE:
#define T_ENABLE_IF(...)     typename ::std::enable_if<__VA_ARGS__, bool>::type = true

// clang-format on
