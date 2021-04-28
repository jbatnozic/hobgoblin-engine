
// IMPORTANT: Always include this as the last header file

// General:
#define HOBGOBLIN_NAMESPACE_START namespace jbatnozic { namespace hobgoblin {
#define HOBGOBLIN_NAMESPACE_END }}
#define NO_COPY public util::NonCopyable
#define NO_MOVE public util::NonMoveable
#define SELF (*this)
#define NO_OP() (void)0
#define CRTP_METHOD(_type_param_, _name_, ...) { return static_cast<_type_param_*>(this)->_name_(__VA_ARGS__); }
#define MSTRINGIFY(_x_) #_x_
#define MDEFER(_x_) _x_
#define CURRENT_FUNCTION_STR std::string{CURRENT_FUNCTION}

// Compiler-specific:
#if defined(_MSC_VER)
#define CURRENT_FUNCTION __FUNCSIG__
#define SWITCH_FALLTHROUGH (void)0
#else
#define CURRENT_FUNCTION __PRETTY_FUNCTION__
#define SWITCH_FALLTHROUGH (void)0 /* TEMP */
#endif