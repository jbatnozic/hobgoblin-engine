// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_COMMON_NULLABILITY_HPP
#define UHOBGOBLIN_COMMON_NULLABILITY_HPP

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

class NullPointerException : public std::logic_error {
public:
    using std::logic_error::logic_error;
};

///////////////////////////////////////////////////////////////////////////
// NEVERNULL                                                             //
///////////////////////////////////////////////////////////////////////////

//
// not_null
//
// Restricts a pointer or smart pointer to only hold non-null values.
//
// Has zero size overhead over T.
//
// If T is a pointer (i.e. T == U*) then
// - allow construction from U*
// - disallow construction from null_ptrt
// - disallow default construction
// - ensure construction from null U* fails
// - allow implicit conversion to U*
//
template <class taPointer>
class NeverNull {
public:
    // static_assert(details::is_comparable_to_nullptr<T>::value, "T cannot be compared to nullptr.");

    //! Construct from a value of type `T` which is convertible to `taPointer`.
    template <typename T,
              typename = std::enable_if_t<std::is_convertible<T, taPointer>::value>>
    constexpr NeverNull(T&& aValue)
        : _ptr{std::forward<T>(aValue)}
    {
        assert(_ptr != nullptr);
        if (_ptr == nullptr) {
            throw NullPointerException{"TODO"};
        }
    }

    //! Construct from a value of type `NeverNull<U>` where `U` is convertible to `taPointer`.
    template <typename U,
              typename = std::enable_if_t<std::is_convertible<U, taPointer>::value>>
    constexpr NeverNull(const NeverNull<U>& aOther)
        : NeverNull{aOther.get()}
    {
    }

    //! ???
    template <typename = std::enable_if_t<!std::is_same<std::nullptr_t, taPointer>::value>>
    constexpr NeverNull(taPointer aValue)
        : _ptr{std::move(aValue)}
    {
        assert(_ptr != nullptr);
        if (_ptr == nullptr) {
            throw NullPointerException{"TODO"};
        }
    }

    //! Default copy constructor.
    NeverNull(const NeverNull& aOther) = default;

    //! Default copy assignment operator.
    NeverNull& operator=(const NeverNull& aOther) = default;

    //! Returns the underlying pointer (or a const reference to it if it's not copyable).
    constexpr
    std::conditional_t<
        std::is_copy_constructible<taPointer>::value, taPointer, const taPointer&
    > get() const
    {
        return _ptr;
    }

    //! Implicit conversion operator to `taPointer`.
    constexpr operator taPointer() const { return get(); }

    //! Operator `->`.
    constexpr decltype(auto) operator->() const { return get(); }

    //! Operator `*`.
    constexpr decltype(auto) operator*() const { return *get(); }

public:
    // prevents compilation when someone attempts to assign a null pointer constant
    NeverNull(std::nullptr_t) = delete;
    NeverNull& operator=(std::nullptr_t) = delete;

    // unwanted operators...pointers only point to single objects!
    NeverNull& operator++() = delete;
    NeverNull& operator--() = delete;
    NeverNull operator++(int) = delete;
    NeverNull operator--(int) = delete;
    NeverNull& operator+=(std::ptrdiff_t) = delete;
    NeverNull& operator-=(std::ptrdiff_t) = delete;
    void operator[](std::ptrdiff_t) const = delete;

private:
    taPointer _ptr;
};

#define HG_NEVER_NULL(...) ::jbatnozic::hobgoblin::NeverNull<__VA_ARGS__>

template <class T>
auto make_not_null(T&& t) noexcept
{
    return NeverNull<std::remove_cv_t<std::remove_reference_t<T>>>{std::forward<T>(t)};
}

#if !defined(GSL_NO_IOSTREAMS)
template <class T>
std::ostream& operator<<(std::ostream& os, const NeverNull<T>& val)
{
    os << val.get();
    return os;
}
#endif // !defined(GSL_NO_IOSTREAMS)

template <class T, class U>
auto operator==(const NeverNull<T>& lhs,
                const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() == rhs.get()))
    -> decltype(lhs.get() == rhs.get())
{
    return lhs.get() == rhs.get();
}

template <class T>
auto operator==(const NeverNull<T>& lhs,
                std::nullptr_t) noexcept(noexcept(lhs.get() == nullptr))
    -> decltype(lhs.get() == nullptr)
{
    return false;
}

template <class T, class U>
auto operator!=(const NeverNull<T>& lhs,
                const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() != rhs.get()))
    -> decltype(lhs.get() != rhs.get())
{
    return lhs.get() != rhs.get();
}

template <class T>
auto operator!=(const NeverNull<T>& lhs,
                std::nullptr_t) noexcept(noexcept(lhs.get() != nullptr))
    -> decltype(lhs.get() != nullptr)
{
    return true;
}

template <class T, class U>
auto operator<(const NeverNull<T>& lhs,
    const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() < rhs.get()))
    -> decltype(lhs.get() < rhs.get())
{
    return lhs.get() < rhs.get();
}

template <class T, class U>
auto operator<=(const NeverNull<T>& lhs,
                const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() <= rhs.get()))
    -> decltype(lhs.get() <= rhs.get())
{
    return lhs.get() <= rhs.get();
}

template <class T, class U>
auto operator>(const NeverNull<T>& lhs,
               const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() > rhs.get()))
    -> decltype(lhs.get() > rhs.get())
{
    return lhs.get() > rhs.get();
}

template <class T, class U>
auto operator>=(const NeverNull<T>& lhs,
                const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() >= rhs.get()))
    -> decltype(lhs.get() >= rhs.get())
{
    return lhs.get() >= rhs.get();
}

// more unwanted operators
template <class T, class U>
std::ptrdiff_t operator-(const NeverNull<T>&, const NeverNull<U>&) = delete;

template <class T>
NeverNull<T> operator-(const NeverNull<T>&, std::ptrdiff_t) = delete;

template <class T>
NeverNull<T> operator+(const NeverNull<T>&, std::ptrdiff_t) = delete;

template <class T>
NeverNull<T> operator+(std::ptrdiff_t, const NeverNull<T>&) = delete;

HOBGOBLIN_NAMESPACE_END

namespace std {

template <class taPointer>
struct hash<jbatnozic::hobgoblin::NeverNull<taPointer>>
{
    std::size_t operator()(const jbatnozic::hobgoblin::NeverNull<taPointer>& aValue) const {
        return hash<taPointer>{}(aValue.get());
    }
};

} // namespace std

HOBGOBLIN_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////
// AVOIDNULL                                                             //
///////////////////////////////////////////////////////////////////////////

template <class taPointer>
class AvoidNull : public NeverNull<taPointer> {
public:
    using NeverNull<taPointer>::NeverNull;
};

#define HG_AVOID_NULL(...) ::jbatnozic::hobgoblin::AvoidNull<__VA_ARGS__>

HOBGOBLIN_NAMESPACE_END

namespace std {

template <class taPointer>
struct hash<jbatnozic::hobgoblin::AvoidNull<taPointer>>
{
    std::size_t operator()(const jbatnozic::hobgoblin::AvoidNull<taPointer>& aValue) const {
        return hash<taPointer>{}(aValue.get());
    }
};

} // namespace std

HOBGOBLIN_NAMESPACE_BEGIN

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_COMMON_NULLABILITY_HPP
