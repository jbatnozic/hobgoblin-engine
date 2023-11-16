#if defined(_MSC_VER) // MSVC
#define UHOBGOBLIN_HGEXCEPT_FUNC __FUNCSIG__
#else // CLANG & GCC
#define UHOBGOBLIN_HGEXCEPT_FUNC __PRETTY_FUNCTION__
#endif

#define UHOBGOBLIN_THROW_TRACED_0(_type_, _code_, _comment_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), "<no message provided>", _comment_})

#define UHOBGOBLIN_THROW_TRACED_1(_type_, _code_, _comment_, _message_) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), (_message_), _comment_})

#define UHOBGOBLIN_THROW_TRACED_2(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_3(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_4(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_5(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_6(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_7(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_8(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_9(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_10(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_11(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_12(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_13(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_14(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_15(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_16(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_17(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_18(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_19(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_20(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_21(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_22(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_23(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#define UHOBGOBLIN_THROW_TRACED_24(_type_, _code_, _comment_, _format_, ...) \
    (throw _type_{#_type_, UHOBGOBLIN_HGEXCEPT_FUNC, __FILE__, __LINE__, (_code_), ::fmt::format(FMT_STRING(_format_), __VA_ARGS__), _comment_})

#if defined(UHOBGOBLIN_USING_TRADITIONAL_MSVC_PREPROCESSOR)
#   define UHOBGOBLIN_THROW_TRACED_FINAL(_type_, _code_, _comment_, _argcount_, ...) \
    UHOBGOBLIN_PP_EXPAND(UHOBGOBLIN_THROW_TRACED_##_argcount_(_type_, _code_, _comment_, __VA_ARGS__))
#else
#   define UHOBGOBLIN_THROW_TRACED_FINAL(_type_, _code_, _comment_, _argcount_, ...) \
    UHOBGOBLIN_THROW_TRACED_##_argcount_(_type_, _code_, _comment_, __VA_ARGS__)
#endif

#define UHOBGOBLIN_THROW_TRACED_MIDDLE(_type_, _code_, _comment_, _argcount_, ...) \
    UHOBGOBLIN_THROW_TRACED_FINAL(_type_, _code_, _comment_, _argcount_, __VA_ARGS__)
