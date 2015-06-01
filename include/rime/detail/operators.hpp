/*
Copyright 2011, 2012 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/** \file
Define operators for Rime constants.

The named functions in rime:: will return a compile-time constant if the
result of the calculation is known at compile-time.
The overloaded operators only apply to Rime constants, and not, for example,
MPL constants: the behaviour of existing code does not change.
*/

#ifndef RIME_OPERATORS_HPP_INCLUDED
#define RIME_OPERATORS_HPP_INCLUDED

#include <type_traits>

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/and.hpp>

#include "rime/core.hpp"

namespace rime {

/*
Produce something like the following:

namespace callable
{
    struct plus
    {
        template <class Left, class Right>
            auto operator() (Left const & left, Right const & right) const
        -> typename boost::enable_if <
            mpl::and_ <is_constant <Left>, is_constant <Right> >,
            constant <
                typename std::decay <decltype (Left::value + Right::value)
                    >::type,
                (Left::value + Right::value)
            >
        >::type
        {
            typedef typename std::decay <decltype (
                Left::value + Right::value)>::type ValueType;
            return constant <ValueType, Left::value + Right::value>();
        }

        template <class Left, class Right>
            auto operator() (Left const & left, Right const & right) const
        -> typename boost::disable_if <
            mpl::and_ <is_constant <Left>, is_constant <Right> >,
            decltype (get_value (left) + get_value (right))
        >::type
        {
            return get_value (left) + get_value (right);
        }
    };
}

static const callable::plus plus = {};

template <class Left, class Right>
    auto operator + (Left const & left, Right const & right)
-> typename boost::enable_if <
    mpl::and_ <is_constant <Left>, is_rime_<Right>,
        mpl::or_ <is_rime_constant <Left>, is_rime_constant <Right> > >,
    constant <
        typename std::decay <decltype (Left::value + Right::value)>::type,
            (Left::value + Right::value)
    >
>::type
{
    typedef typename std::decay <decltype (
        Left::value + Right::value)>::type ValueType;
    return constant <ValueType, Left::value + Right::value>();
}*/

#define RIME_DEFINE_BINARY_OPERATOR(name, operation) \
namespace callable \
{ \
    struct name \
    { \
        template <class Left, class Right> \
            auto operator() (Left const & left, Right const & right) const \
        -> typename boost::enable_if < \
            mpl::and_ <is_constant <Left>, is_constant <Right> >, \
            constant < \
                typename std::decay <decltype (( \
                    Left::value operation Right::value))>::type, \
                (Left::value operation Right::value) \
            > \
        >::type \
        { \
            typedef typename std::decay <decltype (( \
                Left::value operation Right::value))>::type ValueType; \
            return constant <ValueType, \
                (Left::value operation Right::value)>(); \
        } \
\
        template <class Left, class Right> \
            auto operator() (Left const & left, Right const & right) const \
        -> typename boost::disable_if < \
            mpl::and_ <is_constant <Left>, is_constant <Right> >, \
            decltype ((get_value (left) operation get_value (right))) \
        >::type \
        { \
            return get_value (left) operation get_value (right); \
        } \
    }; \
} \
\
static const callable::name name = callable::name(); \
\
template <class Left, class Right> \
    auto operator operation (Left const & left, Right const & right) \
-> typename boost::enable_if < \
    mpl::and_ <is_constant <Left>, is_constant <Right>, \
        mpl::or_ <is_rime_constant <Left>, is_rime_constant <Right> > >, \
    constant < \
        typename std::decay <decltype (( \
            Left::value operation Right::value))>::type, \
            (Left::value operation Right::value) \
    > \
>::type \
{ \
    typedef typename std::decay <decltype (( \
        Left::value operation Right::value))>::type ValueType; \
    return constant <ValueType, (Left::value operation Right::value)>(); \
}

RIME_DEFINE_BINARY_OPERATOR(plus, +)
RIME_DEFINE_BINARY_OPERATOR(minus, -)
RIME_DEFINE_BINARY_OPERATOR(times, *)
RIME_DEFINE_BINARY_OPERATOR(divides, /)
RIME_DEFINE_BINARY_OPERATOR(modulo, %)

RIME_DEFINE_BINARY_OPERATOR(bitwise_and, &)
RIME_DEFINE_BINARY_OPERATOR(bitwise_or, |)
RIME_DEFINE_BINARY_OPERATOR(bitwise_xor, ^)
RIME_DEFINE_BINARY_OPERATOR(shift_left, <<)
RIME_DEFINE_BINARY_OPERATOR(shift_right, >>)

RIME_DEFINE_BINARY_OPERATOR(equal_to, ==)
RIME_DEFINE_BINARY_OPERATOR(not_equal_to, !=)
RIME_DEFINE_BINARY_OPERATOR(less, <)
RIME_DEFINE_BINARY_OPERATOR(less_equal, <=)
RIME_DEFINE_BINARY_OPERATOR(greater, >)
RIME_DEFINE_BINARY_OPERATOR(greater_equal, >=)

#undef RIME_DEFINE_BINARY_OPERATOR

#define RIME_DEFINE_UNARY_OPERATOR(name, operation) \
namespace callable \
{ \
    struct name \
    { \
        template <class Argument> \
            auto operator() (Argument const & argument) const \
        -> typename boost::enable_if < \
            is_constant <Argument>, \
            constant < \
                typename std::decay <decltype ( \
                    operation Argument::value)>::type, \
                (operation Argument::value) \
            > \
        >::type \
        { \
            typedef typename std::decay <decltype ( \
                operation Argument::value)>::type ValueType; \
            return constant <ValueType, (operation Argument::value)>(); \
        } \
\
        template <class Argument> \
            auto operator() (Argument const & argument) const \
        -> typename boost::disable_if < \
            is_constant <Argument>, \
            decltype (operation get_value (argument)) \
        >::type \
        { \
            return operation get_value (argument); \
        } \
    }; \
} \
\
static const callable::name name = {}; \
\
template <class Argument> \
    auto operator operation (Argument const & argument) \
-> typename boost::enable_if < \
    is_constant <Argument>, \
    constant < \
        typename std::decay <decltype (operation Argument::value)>::type, \
            (operation Argument::value) \
    > \
>::type \
{ \
    typedef typename std::decay <decltype ( \
        operation Argument::value)>::type ValueType; \
    return constant <ValueType, (operation Argument::value)>(); \
}

RIME_DEFINE_UNARY_OPERATOR(negate, -)
RIME_DEFINE_UNARY_OPERATOR(not_, !)
RIME_DEFINE_UNARY_OPERATOR(bitwise_not, ~)

#undef RIME_DEFINE_UNARY_OPERATOR

/* "and" and "or" */
namespace callable {

    namespace detail {
        template <typename Constant, typename... Arguments>
            struct any_equal_constant;
        template <typename Constant>
            struct any_equal_constant <Constant> : mpl::false_ {};
        template <typename Constant, typename Head, typename...Tail>
            struct any_equal_constant <Constant, Head, Tail...>
        : mpl::or_ <
            equal_constant <Constant, Head>,
            any_equal_constant <Constant, Tail...>
        > {};

        template <typename Constant, typename... Arguments>
            struct all_equal_constant;
        template <typename Constant>
            struct all_equal_constant <Constant> : mpl::true_ {};
        template <typename Constant, typename Head, typename...Tail>
            struct all_equal_constant <Constant, Head, Tail...>
        : mpl::and_ <
            equal_constant <Constant, Head>,
            all_equal_constant <Constant, Tail...>
        > {};
    } // namespace detail

    template <typename Type> struct return_ {
        template <typename... Arguments>
            Type operator() (Arguments const &...) const
        { return Type(); }
    };

} // namespace callable


// If any one of the parameters is "overrideValue" (true or false), then the
// whole expression equals "overrideValue".
// If all parameters are "defaultValue" (or if there are zero parameters), then
// the whole expression equals "defaultValue".
#define RIME_DEFINE_BINARY_BOOLEAN_OPERATOR( \
    name, operation, overrideValue, defaultValue) \
namespace callable \
{ \
    namespace detail \
    { \
        template <typename Void, typename... Arguments> struct name##runtime; \
        template <> struct name##runtime <void> \
        { \
            bool operator() () const { return defaultValue; } \
        }; \
        \
        template <typename Head, typename... Tail> \
            struct name##runtime < \
                typename boost::disable_if <is_constant <Head> >::type, \
                Head, Tail...> \
        { \
            bool operator() (Head const & head, Tail const & ... tail) const \
            { \
                name##runtime <void, Tail...> child; \
                return head operation child (tail...); \
            } \
        }; \
        \
        template <typename Head, typename... Tail> \
            struct name##runtime < \
                typename boost::enable_if <is_constant <Head> >::type, \
                Head, Tail...> \
        { \
            bool operator() (Head const &, Tail const & ... tail) const \
            { \
                name##runtime <void, Tail...> child; \
                return child (tail...); \
            } \
        }; \
    } /* namespace detail */ \
    \
    class name \
    { \
        template <typename... Arguments> struct implementation \
        : mpl::if_ < \
            detail::any_equal_constant <bool_ <overrideValue>, Arguments...>, \
            return_ <bool_ <overrideValue> >, \
            typename mpl::if_ < \
                detail::all_equal_constant <bool_ <defaultValue>, \
                    Arguments...>, \
                return_ <bool_ <defaultValue> >, \
                detail::name##runtime <void, Arguments...> \
            >::type \
        >::type {}; \
        \
    public: \
        template <typename... Arguments> \
            auto operator() (Arguments const & ... arguments) const \
        -> decltype (implementation <Arguments...>() (arguments...)) \
        { \
            return implementation <Arguments...>() (arguments...); \
        } \
    }; \
} \
\
static const callable::name name = {}; \
\
template <class Left, class Right> \
    auto operator operation (Left const & left, Right const & right) \
-> typename boost::enable_if < \
    mpl::and_ <is_constant <Left>, is_constant <Right>, \
        mpl::or_ <is_rime_constant <Left>, is_rime_constant <Right> > >, \
    bool_ <(Left::value operation Right::value)> \
>::type \
{ \
    return bool_ <(Left::value operation Right::value)>(); \
}

RIME_DEFINE_BINARY_BOOLEAN_OPERATOR(or_, ||, true, false)
RIME_DEFINE_BINARY_BOOLEAN_OPERATOR(and_, &&, false, true)

#undef RIME_DEFINE_BINARY_BOOLEAN_OPERATOR

} // namespace rime

#endif  // RIME_OPERATORS_HPP_INCLUDED

