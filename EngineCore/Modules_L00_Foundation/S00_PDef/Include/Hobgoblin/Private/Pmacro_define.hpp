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
#define CRTP_METHOD(_type_param_, _name_, ...) { return static_cast<_type_param_*>(this)->_name_(__VA_ARGS__); }
#define CURRENT_FUNCTION_STR std::string{CURRENT_FUNCTION}

#define NO_COPY public util::NonCopyable
#define NO_MOVE public util::NonMoveable
#define NO_OP() (void)0
#define SELF (*this)

// SFINAE:
#define T_ENABLE_IF(...) typename std::enable_if<__VA_ARGS__, bool>::type = true
