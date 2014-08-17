/*
Copyright 2011, 2012, 2014 Rogier van Dalen.

This file is part of Rogier van Dalen's Rime library for C++.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RIME_CORE_HPP_INCLUDED
#define RIME_CORE_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/integral_c_tag.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/equal_to.hpp>

#include <boost/utility/enable_if.hpp>

#include "utility/overload_order.hpp"

namespace rime {

namespace mpl = boost::mpl;

struct constant_base { constant_base () {} };

/**
A compile-time constant that behaves like a normal type ("value").
The advantage is that code can mix it with normal types, and that generic
routines can work with both compile-time constants and values.
They have an implicit conversion to the value type.

Implementation of the Rime compile-time constant concept:
\li value_type is the type of the value, Type;
\li value is a static const value_type equal to content;
\li is_constant returns true for this class and derived classes.

Rime constants adhere to the Boost MPL Integral Constant concept.

Rime constants are very similar to MPL integral constants.
The difference is their behaviour with operators.
Operators are overloaded (in operators.hpp) to use compile-time knowledge in
Rime constants.
For example, (rime::true_ || rime::true_) is known at compile time to be true,
and it returns a compile-time constant.
The behaviour of (boost::mpl::true_() || boost::mpl::true_()) is not changed.

\internal
It is necessary to derive from constant_base so that is_decayed_constant can
also pick up types derived from this.
*/
template <class Type, Type content> class constant
: public constant_base {
public:
    typedef mpl::integral_c_tag tag;
    typedef constant type;
    typedef typename std::decay <Type>::type value_type;

    static constexpr value_type value = content;

    constexpr operator value_type() const { return content; }
};

// Out-of-class definition is required to allow references to constant<>::value.
template <class Type, Type content>
    constexpr typename std::decay <Type>::type constant <Type, content>::value;

template <int content> struct int_ : constant <int, content> {};

template <std::size_t content> struct size_t
: constant <std::size_t, content> {};

template <bool content> struct bool_ : constant <bool, content> {};

typedef bool_<false> false_type;
typedef bool_<true> true_type;

static const auto false_ = false_type();
static const auto true_ = true_type();

/**
Specialise this for Rime constants that don't derive from constant_base.
*/
template <class Type> struct is_decayed_rime_constant
: std::is_base_of <constant_base, Type> {};

/**
True iff Type is a Rime constant.
*/
template <class Type> struct is_rime_constant
: is_decayed_rime_constant <typename std::decay <Type>::type> {};

namespace is_constant_detail {

    struct take_std_integral_constant {
        // Standard library constants derive from std::integral_constant.
        template <class Type, Type value> take_std_integral_constant (
            std::integral_constant <Type, value> const &);
    };

    template <class Type> struct is_std_integral_constant
    : std::is_convertible <Type, take_std_integral_constant> {};

    struct take_mpl_integral_constant {
        // MPL constants have a typedef integral_c_tag tag.
        template <typename Constant> take_mpl_integral_constant (
            Constant const &,
            typename Constant::tag = boost::mpl::integral_c_tag());
    };

    template <class Type> struct is_mpl_integral_constant
    : std::is_convertible <Type, take_mpl_integral_constant> {};

} // namespace is_constant_detail

/**
True iff Type can be used as a compile-time constant.
I.e. iff Type is an integral constant from C++11 or Boost.MPL,
or a Rime constant.
*/
template <class Type> struct is_constant
: mpl::or_ <is_rime_constant <Type>,
    is_constant_detail::is_mpl_integral_constant <Type>,
    is_constant_detail::is_std_integral_constant <Type>> {};

/**
Convert any constant into a rime::constant<>.
*/
template <class Constant> struct as_rime_constant {
    static_assert (is_constant <Constant>::value,
        "Type argument to as_rime_constant must be a constant.");

    typedef constant <typename Constant::value_type, Constant::value> type;
};

/**
Return the type of a compile-time constant.
Compile-time function.
*/
template <class Type> struct constant_value
{ typedef typename std::decay <Type>::type::value_type type; };

template <class Type> struct runtime_value
{ typedef Type type; };

/**
Return the type of a compile-time constant or normal type.
Compile-time function.
*/
template <class Type> struct value
: mpl::if_ <is_constant <Type>,
    constant_value <Type>,
    runtime_value <Type>
>::type {};

/**
Return the value of a compile-time constant of normal type.
\return If the parameter is a compile-time constant, Type::value.
    Otherwise, the parameter itself.
*/
template <class Type> inline
    typename boost::enable_if <
        is_constant <Type>, typename value <Type>::type>::type
    get_value (Type const &)
{ return Type::value; }
template <class Type> inline
    typename boost::disable_if <
        is_constant <Type>, Type const &>::type
    get_value (Type const & value)
{ return value; }

namespace detail {

    // Left and Right must be constants
    template <class Left, class Right> struct constant_equals
    : bool_ <(std::decay <Left>::type::value
        == std::decay <Right>::type::value)> {};

    template <class Left, class Right> struct constant_same
    : mpl::if_ <std::is_same <typename constant_value <Left>::type,
            typename constant_value <Right>::type>,
        constant_equals <Left, Right>, false_type>::type {};

} // namespace detail

/**
Returns true iff both Left and Right are compile-time constants, and their
value types are the same, and their values compare equal.
*/
template <class Left, class Right> struct same_constant
: as_rime_constant <boost::mpl::and_ <is_constant <Left>, is_constant <Right>,
    detail::constant_same <Left, Right>>>::type {};

/**
Returns true iff both Left and Right are compile-time constants, and their
values compare equal.
Note that this does not check whether the values are comparable.
Comparing constants with signed and unsigned integers is therefore possible,
but may yield compiler errors.
*/
template <class Left, class Right> struct equal_constant
: as_rime_constant <boost::mpl::and_ <is_constant <Left>, is_constant <Right>,
    detail::constant_equals <Left, Right>>>::type {};

namespace merge_policy {

    /**
    Merge two types, special-casing constants.
    If both types are constants, then an attempt is made to unify their value
    types (using merge policy \a Base) and then to unify their values.
    int_ <5> and constant <unsigned, 5> could therefore become
    constant <unsigned, 5> is Base merges int and unsigned to int.
    */
    template <class Base> class constant {
        template <class Type1, class Type2> struct merge_value_type
        : Base::template apply <
            typename rime::value <Type1>::type,
            typename rime::value <Type2>::type> {};

        template <class Type> struct has_type_implementation {
            template <class Contained> struct yes : true_type {};
            struct no : false_type {};

            template <class TypeAgain>
                static yes <typename TypeAgain::type>
                check_has_type (utility::overload_order <1> *);

            template <class TypeAgain>
                static no check_has_type (utility::overload_order <2> *);

            typedef decltype (check_has_type <Type> (utility::pick_overload()))
                answer;
        };

        /**
        Constant with boolean value which is \c true iff \a Type has a type
        member \c type.
        */
        template <class Type> struct has_type
        : has_type_implementation <Type>::answer {};

        /**
        Merge two constants.
        First merge the value types.
        If the values in this merged value type turn out to be the same, return
        a rime::constant.
        If not, return the merged value type.
        */
        template <class Type1, class Type2> struct merge_constant {
            typedef typename merge_value_type <Type1, Type2>::type value_type;
            static const value_type value1 = std::decay <Type1>::type::value;
            static const value_type value2 = std::decay <Type2>::type::value;
            typedef typename std::conditional <(value1 == value2),
                rime::constant <value_type, value1>, value_type>::type type;
        };

        // Law of least surprise: two constants with the exact same type are
        // merged to the same type.
        template <class Type> struct merge_constant <Type, Type>
        { typedef Type type; };

    public:
        template <class Type1, class Type2, class Enable = void> struct apply
        : merge_value_type <Type1, Type2> {};

        // Specialisation only for when both are constants and the value types
        // can be merged.
        // Then, try to turn this into one constant.
        template <class Type1, class Type2>
            struct apply <Type1, Type2, typename boost::enable_if <
                mpl::and_ <is_constant <Type1>, is_constant <Type2>,
                    has_type <merge_value_type <Type1, Type2>>>>::type>
        : merge_constant <Type1, Type2> {};
    };

} // namespace merge_policy

} // namespace rime

#include "detail/operators.hpp"

#endif // RIME_CORE_HPP_INCLUDED
