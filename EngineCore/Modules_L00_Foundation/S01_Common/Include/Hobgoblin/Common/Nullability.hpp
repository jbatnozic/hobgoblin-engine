// The code in this file is adapted from the code at https://github.com/microsoft/GSL,
// which was originally provided under the MIT licence. The original copyright notice
// is presented below:

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef UHOBGOBLIN_COMMON_NULLABILITY_HPP
#define UHOBGOBLIN_COMMON_NULLABILITY_HPP

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN

//! Exception that's thrown when an attempt is made to assign NULL to one
//! of the pointer-like classes that's supposed to guard against NULL values.
//! \see NeverNull, AvoidNull
class NullPointerException : public std::logic_error {
public:
    using std::logic_error::logic_error;
};

///////////////////////////////////////////////////////////////////////////
// NEVERNULL                                                             //
///////////////////////////////////////////////////////////////////////////

//! Restricts a raw pointer or smart pointer to only hold non-NULL values.
//!
//! Has zero size overhead over `taPointer`.
//!
//! If `taPointer` is a pointer (i.e. `taPointer` == `U*`) then
//! - allow construction from `U*`
//! - disallow construction from `nullptr`
//! - disallow default construction
//! - ensure construction from NULL `U*` fails
//! - allow implicit conversion to `U*`
//!
template <class taPointer>
class NeverNull {
public:
    //! Construct from a value of type `T` which is convertible to `taPointer`.
    //!
    //! If `aValue` compares equal to `nullptr`, then:
    //! 1) If standard assertions are enabled, an assertion failure will be generated.
    //! 2) A `NullPointerException` will be thrown.
    template <typename T, typename = std::enable_if_t<std::is_convertible<T, taPointer>::value>>
    constexpr NeverNull(T&& aValue)
        : _ptr{std::forward<T>(aValue)} {
        assert(_ptr != nullptr);
        if (_ptr == nullptr) {
            throw NullPointerException{
                "Attempting to construct an instance of NeverNull with a NULL pointer! (function: " +
                CURRENT_FUNCTION_STR + ")"};
        }
    }

    //! Construct from a value of type `NeverNull<U>` where `U` is convertible to `taPointer`.
    template <typename U, typename = std::enable_if_t<std::is_convertible<U, taPointer>::value>>
    constexpr NeverNull(const NeverNull<U>& aOther)
        : NeverNull{aOther.get()} {}

    //! ???
    template <typename = std::enable_if_t<!std::is_same<std::nullptr_t, taPointer>::value>>
    constexpr NeverNull(taPointer aValue)
        : _ptr{std::move(aValue)} {
        assert(_ptr != nullptr);
        if (_ptr == nullptr) {
            throw NullPointerException{
                "Attempting to construct an instance of NeverNull with a NULL pointer! (function: " +
                CURRENT_FUNCTION_STR + ")"};
        }
    }

    //! Default copy constructor.
    NeverNull(const NeverNull& aOther) = default;

    //! Default copy assignment operator.
    NeverNull& operator=(const NeverNull& aOther) = default;

    //! Returns the underlying pointer (or a const reference to it if it's not copyable).
    constexpr std::
        conditional_t< std::is_copy_constructible<taPointer>::value, taPointer, const taPointer& >
        get() const {
        return _ptr;
    }

    //! Implicit conversion operator to `taPointer`.
    constexpr operator taPointer() const {
        return get();
    }

    //! Operator `->`.
    constexpr decltype(auto) operator->() const {
        return get();
    }

    //! Operator `*`.
    constexpr decltype(auto) operator*() const {
        return *get();
    }

public:
    // prevents compilation when someone attempts to assign a null pointer constant
    NeverNull(std::nullptr_t)            = delete;
    NeverNull& operator=(std::nullptr_t) = delete;

    // unwanted operators...pointers only point to single objects!
    NeverNull& operator++()                     = delete;
    NeverNull& operator--()                     = delete;
    NeverNull  operator++(int)                  = delete;
    NeverNull  operator--(int)                  = delete;
    NeverNull& operator+=(std::ptrdiff_t)       = delete;
    NeverNull& operator-=(std::ptrdiff_t)       = delete;
    void       operator[](std::ptrdiff_t) const = delete;

protected:
    taPointer _ptr;

public:
    // TODO
    // static_assert(details::is_comparable_to_nullptr<taPointer>::value, "T cannot be compared to
    // nullptr.");
};

#define HG_NEVER_NULL(...) ::jbatnozic::hobgoblin::NeverNull<__VA_ARGS__>

template <class T>
auto MakeNeverNull(T&& t) {
    return NeverNull<std::remove_cv_t<std::remove_reference_t<T>>>{std::forward<T>(t)};
}

// TODO
#if !defined(GSL_NO_IOSTREAMS)
template <class T>
std::ostream& operator<<(std::ostream& os, const NeverNull<T>& val) {
    os << val.get();
    return os;
}
#endif // !defined(GSL_NO_IOSTREAMS)

template <class T, class U>
auto operator==(const NeverNull<T>& lhs,
                const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() == rhs.get()))
    -> decltype(lhs.get() == rhs.get()) {
    return lhs.get() == rhs.get();
}

template <class T>
auto operator==(const NeverNull<T>& lhs, std::nullptr_t) noexcept(noexcept(lhs.get() == nullptr))
    -> decltype(lhs.get() == nullptr) {
    return false;
}

template <class T, class U>
auto operator!=(const NeverNull<T>& lhs,
                const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() != rhs.get()))
    -> decltype(lhs.get() != rhs.get()) {
    return lhs.get() != rhs.get();
}

template <class T>
auto operator!=(const NeverNull<T>& lhs, std::nullptr_t) noexcept(noexcept(lhs.get() != nullptr))
    -> decltype(lhs.get() != nullptr) {
    return true;
}

template <class T, class U>
auto operator<(const NeverNull<T>& lhs,
               const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() < rhs.get()))
    -> decltype(lhs.get() < rhs.get()) {
    return lhs.get() < rhs.get();
}

template <class T, class U>
auto operator<=(const NeverNull<T>& lhs,
                const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() <= rhs.get()))
    -> decltype(lhs.get() <= rhs.get()) {
    return lhs.get() <= rhs.get();
}

template <class T, class U>
auto operator>(const NeverNull<T>& lhs,
               const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() > rhs.get()))
    -> decltype(lhs.get() > rhs.get()) {
    return lhs.get() > rhs.get();
}

template <class T, class U>
auto operator>=(const NeverNull<T>& lhs,
                const NeverNull<U>& rhs) noexcept(noexcept(lhs.get() >= rhs.get()))
    -> decltype(lhs.get() >= rhs.get()) {
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

//! Enable `std::hash` for `NeverNull`.
template <class taPointer>
struct hash<jbatnozic::hobgoblin::NeverNull<taPointer>> {
    std::size_t operator()(const jbatnozic::hobgoblin::NeverNull<taPointer>& aValue) const {
        return hash<taPointer>{}(aValue.get());
    }
};

} // namespace std

HOBGOBLIN_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////
// AVOIDNULL                                                             //
///////////////////////////////////////////////////////////////////////////

//! `AvoidNull` class template is very similar to `NeverNull` class template.
//!
//! The only difference is that an instance of `AvoidNull<T>` can be moved from,
//! making `AvoidNull` more flexible for use with move-only pointer types, such
//! as `std::unique_ptr`.
//!
//! \warning After an instance of `AvoidNull<T>` is moved from, YOU MUST NOT USE IT
//!          ANYMORE. Consider the move as ending its lifetime.
template <class taPointer>
class AvoidNull : public NeverNull<taPointer> {
public:
    using NeverNull<taPointer>::NeverNull;

    constexpr AvoidNull(AvoidNull&& aOther)
        : NeverNull<taPointer>{std::move(aOther._ptr)} {}

    constexpr AvoidNull& operator=(AvoidNull&& aOther) {
        if (&aOther != this) {
            NeverNull<taPointer>::_ptr = std::move(aOther._ptr);
        }
        return SELF;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible<U, taPointer>::value>>
    constexpr AvoidNull(const NeverNull<U>& aOther)
        : NeverNull<taPointer>{aOther.get()} {}
};

#define HG_AVOID_NULL(...) ::jbatnozic::hobgoblin::AvoidNull<__VA_ARGS__>

template <class T>
auto MakeAvoidNull(T&& t) {
    return AvoidNull<std::remove_cv_t<std::remove_reference_t<T>>>{std::forward<T>(t)};
}

HOBGOBLIN_NAMESPACE_END

namespace std {

//! Enable `std::hash` for `AvoidNull`.
template <class taPointer>
struct hash<jbatnozic::hobgoblin::AvoidNull<taPointer>> {
    std::size_t operator()(const jbatnozic::hobgoblin::AvoidNull<taPointer>& aValue) const {
        return hash<taPointer>{}(aValue.get());
    }
};

} // namespace std

// HOBGOBLIN_NAMESPACE_BEGIN
// HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_COMMON_NULLABILITY_HPP
