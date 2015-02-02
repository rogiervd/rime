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

#define BOOST_TEST_MODULE test_rime
#include "utility/test/boost_unit_test.hpp"

#include "rime/core.hpp"

#include <type_traits>

#include <boost/mpl/int.hpp>
#include <boost/mpl/assert.hpp>

#include "utility/is_assignable.hpp"

struct base {};
struct derived : base {};

BOOST_AUTO_TEST_SUITE(test_rime)

/**
A call to this with constant <...>::value will fail to linke if the value is not
initialised.
*/
void check_static_value_in_object_file (int const & v) {
    BOOST_CHECK_EQUAL (v, v);
}

BOOST_AUTO_TEST_CASE (test_rime_core) {
    rime::int_ <3> rime_three;
    check_static_value_in_object_file (rime::int_ <3>::value);
    int int_three = 3;

    // is_rime_constant
    BOOST_MPL_ASSERT_NOT ((rime::is_rime_constant <std::true_type>));
    BOOST_MPL_ASSERT_NOT ((rime::is_rime_constant <
        std::integral_constant <int, 3> >));
    BOOST_MPL_ASSERT_NOT ((rime::is_rime_constant <boost::mpl::true_>));
    BOOST_MPL_ASSERT_NOT ((rime::is_rime_constant <boost::mpl::int_ <3> >));
    BOOST_MPL_ASSERT ((rime::is_rime_constant <rime::bool_<true> >));
    BOOST_MPL_ASSERT ((rime::is_rime_constant <rime::int_ <3> const>));
    BOOST_MPL_ASSERT ((rime::is_rime_constant <rime::constant <long, 3> &>));

    // is_constant
    BOOST_MPL_ASSERT ((rime::is_constant <std::true_type>));
    BOOST_MPL_ASSERT ((rime::is_constant <std::integral_constant <int, 3> &>));
    BOOST_MPL_ASSERT ((rime::is_constant <std::is_integral <int> const>));
    BOOST_MPL_ASSERT ((rime::is_constant <std::is_same <int, bool> const &>));
    BOOST_MPL_ASSERT ((rime::is_constant <std::is_base_of <base, derived>>));
    BOOST_MPL_ASSERT ((rime::is_constant <std::is_base_of <derived, base>>));

    BOOST_MPL_ASSERT ((rime::is_constant <boost::mpl::true_>));
    BOOST_MPL_ASSERT ((rime::is_constant <boost::mpl::int_ <3> &>));
    BOOST_MPL_ASSERT ((rime::is_constant <rime::bool_<true> >));
    BOOST_MPL_ASSERT ((rime::is_constant <rime::int_ <3> const>));
    BOOST_MPL_ASSERT ((rime::is_constant <rime::constant <long, 3> >));

    BOOST_MPL_ASSERT_NOT ((rime::is_constant <int>));
    BOOST_MPL_ASSERT_NOT ((rime::is_constant <double>));
    BOOST_MPL_ASSERT_NOT ((rime::is_constant <derived>));
    BOOST_MPL_ASSERT_NOT ((rime::is_constant <void>));

    // value
    BOOST_MPL_ASSERT ((std::is_same <
        rime::value <boost::mpl::true_>::type, bool>));
    BOOST_MPL_ASSERT ((std::is_same <
        rime::value <boost::mpl::false_ &>::type, bool>));
    BOOST_MPL_ASSERT ((std::is_same <
        rime::value <rime::int_ <3> >::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <
        rime::value <rime::int_ <0> const>::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <
        rime::value <rime::int_ <5> const &>::type, int>));

    BOOST_MPL_ASSERT ((std::is_same <rime::value <int>::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <rime::value <int const >::type,
        int const>));
    BOOST_MPL_ASSERT ((std::is_same <rime::value <int &>::type, int &>));
    BOOST_MPL_ASSERT ((std::is_same <rime::value <int const &>::type,
        int const &>));

    // get_value
    BOOST_CHECK_EQUAL (rime::get_value (rime::int_ <3>()), 3);
    BOOST_CHECK_EQUAL (rime::get_value (rime::bool_ <true>()), true);
    BOOST_CHECK_EQUAL (rime::get_value (4), 4);
    BOOST_CHECK_EQUAL (rime::get_value (false), false);
    BOOST_CHECK_EQUAL (rime::get_value (4.7), 4.7);

    // equal_constant
    BOOST_MPL_ASSERT ((rime::equal_constant <
        boost::mpl::true_, rime::bool_ <true>>));
    BOOST_MPL_ASSERT ((rime::equal_constant <
        std::false_type, rime::bool_ <false> >));
    BOOST_MPL_ASSERT ((rime::equal_constant <
        boost::mpl::int_ <5>, rime::int_ <5>>));
    BOOST_MPL_ASSERT ((rime::equal_constant <
        rime::bool_ <true>, rime::bool_<true> >));
    BOOST_MPL_ASSERT_NOT ((rime::equal_constant <
        rime::int_ <4>, rime::int_ <5>>));
    BOOST_MPL_ASSERT ((rime::equal_constant <
        rime::constant <unsigned, 5u>, rime::int_ <5>>));
    BOOST_MPL_ASSERT_NOT ((rime::equal_constant <
        boost::mpl::int_ <4>, rime::int_ <5> >));
    BOOST_MPL_ASSERT_NOT ((rime::equal_constant <
        boost::mpl::bool_ <false>, boost::mpl::bool_ <true> >));
    BOOST_MPL_ASSERT_NOT ((rime::equal_constant <
        rime::bool_ <false>, rime::bool_<true>>));
    // This does work, but it's hardly the intended purpose.
    /*BOOST_MPL_ASSERT ((rime::equal_constant <
        rime::int_ <1>, rime::constant <bool, true>>));*/
    BOOST_MPL_ASSERT_NOT ((rime::equal_constant <
        bool, rime::bool_<true> >));
    BOOST_MPL_ASSERT_NOT ((rime::equal_constant <
        bool, bool>));
    BOOST_MPL_ASSERT_NOT ((rime::equal_constant <
        boost::mpl::int_ <4>, int>));
    BOOST_MPL_ASSERT_NOT ((rime::equal_constant <
        int, int>));
    BOOST_MPL_ASSERT_NOT ((rime::equal_constant <
        int, bool>));

    // same_constant
    BOOST_MPL_ASSERT ((rime::same_constant <
        boost::mpl::true_, rime::bool_ <true> >));
    BOOST_MPL_ASSERT ((rime::same_constant <
        std::false_type, rime::bool_ <false> >));
    BOOST_MPL_ASSERT ((rime::same_constant <
        boost::mpl::int_ <5>, rime::int_ <5> >));
    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::bool_ <true>, rime::bool_<true> >));
    BOOST_MPL_ASSERT_NOT ((rime::same_constant <
        rime::int_ <4>, rime::int_ <5> >));
    BOOST_MPL_ASSERT_NOT ((rime::same_constant <
        rime::constant <unsigned, 5u>, rime::int_ <5>>));
    BOOST_MPL_ASSERT_NOT ((rime::same_constant <
        boost::mpl::int_ <4>, rime::int_ <5> >));
    BOOST_MPL_ASSERT_NOT ((rime::same_constant <
        boost::mpl::bool_ <false>, boost::mpl::bool_ <true>>));
    BOOST_MPL_ASSERT_NOT ((rime::same_constant <
        rime::bool_ <false>, rime::bool_<true>>));
    BOOST_MPL_ASSERT_NOT ((rime::same_constant <
        rime::int_ <5>, rime::constant <unsigned, 5u>>));
    BOOST_MPL_ASSERT_NOT ((rime::same_constant <
        bool, rime::bool_<true> >));
    BOOST_MPL_ASSERT_NOT ((rime::same_constant <
        bool, bool>));
    BOOST_MPL_ASSERT_NOT ((rime::same_constant <
        boost::mpl::int_ <4>, int>));
    BOOST_MPL_ASSERT_NOT ((rime::same_constant <
        int, int>));
    BOOST_MPL_ASSERT_NOT ((rime::same_constant <
        int, bool>));

    // Predefined types.
    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::bool_ <false>, rime::constant <bool, false>>));
    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::false_type, rime::bool_ <false> >));
    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::true_type, rime::bool_ <true> >));
    BOOST_MPL_ASSERT ((rime::same_constant <
        decltype (rime::false_), rime::bool_ <false> >));
    BOOST_MPL_ASSERT ((rime::same_constant <
        decltype (rime::true_), rime::bool_ <true> >));

    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::int_ <7>, rime::constant <int, 7>>));
    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::size_t <8u>, rime::constant <std::size_t, 8u>>));

    // as_rime_constant
    BOOST_MPL_ASSERT ((std::is_same <
        rime::as_rime_constant <boost::mpl::int_ <7>>::type,
        rime::constant <int, 7>>));
    BOOST_MPL_ASSERT ((std::is_same <
        rime::as_rime_constant <std::integral_constant <bool, true>>::type,
        rime::constant <bool, true>>));
    BOOST_MPL_ASSERT ((std::is_same <
        rime::as_rime_constant <rime::constant <unsigned, 45u>>::type,
        rime::constant <unsigned, 45u>>));

    // Convert to run-time value
    BOOST_CHECK_EQUAL (rime_three, int_three);
    BOOST_CHECK_EQUAL (rime_three + 2, 5);
    BOOST_CHECK (rime_three < 5);
    BOOST_CHECK (rime_three == 3);
}

/* Check conversion from another integral constant with the same value. */

BOOST_AUTO_TEST_CASE (test_rime_constant_convert) {
    // All sorts of bools.
    static_assert (std::is_convertible <
        rime::constant <bool, true>, rime::bool_ <true>>::value, "");
    static_assert (!std::is_convertible <
        rime::constant <bool, true>, rime::bool_ <false>>::value, "");

    static_assert (std::is_convertible <
        rime::true_type, rime::bool_ <true>>::value, "");
    static_assert (!std::is_convertible <
        rime::true_type, rime::bool_ <false>>::value, "");

    static_assert (std::is_convertible <
        std::true_type, rime::bool_ <true>>::value, "");
    static_assert (!std::is_convertible <
        std::true_type, rime::bool_ <false>>::value, "");

    static_assert (!std::is_convertible <
        boost::mpl::false_, rime::bool_ <true>>::value, "");
    static_assert (std::is_convertible <
        boost::mpl::false_, rime::bool_ <false>>::value, "");

    static_assert (!std::is_convertible <
        boost::mpl::false_, rime::bool_ <true>>::value, "");
    static_assert (std::is_convertible <
        boost::mpl::false_, rime::bool_ <false>>::value, "");

    static_assert (std::is_convertible <
        std::integral_constant <int, 5>, rime::int_ <5>>::value, "");
    static_assert (!std::is_convertible <
        std::integral_constant <int, 5>, rime::int_ <7>>::value, "");

    // No conversion between types.
    static_assert (!std::is_convertible <
        std::integral_constant <unsigned, 5>, rime::int_ <5>>::value, "");
    static_assert (!std::is_convertible <
        boost::mpl::integral_c <short, 5>, rime::size_t <5>>::value, "");

    static_assert (!std::is_convertible <
        std::integral_constant <int, 0>, rime::false_type>::value, "");
    static_assert (!std::is_convertible <
        boost::mpl::integral_c <short, 0>, rime::false_type>::value, "");
}

// This is useful particularly to call functions based on a compile-time
// predicate.

int return_int (int i, rime::constant <bool, true>) { return i; }
int return_int (int i, rime::constant <bool, false>) { return -1; }
int return_int_2 (int i, rime::true_type) { return i; }
int return_int_2 (int i, rime::false_type) { return -1; }

BOOST_AUTO_TEST_CASE (test_rime_constant_convert_bool) {
    BOOST_CHECK_EQUAL (return_int (5, std::false_type()), -1);
    BOOST_CHECK_EQUAL (return_int (5, std::is_same <int, short>()), -1);
    BOOST_CHECK_EQUAL (return_int (5, boost::mpl::false_()), -1);
    BOOST_CHECK_EQUAL (return_int (5, rime::false_), -1);
    BOOST_CHECK_EQUAL (return_int (5, rime::constant <bool, false>()), -1);

    BOOST_CHECK_EQUAL (return_int (5, std::true_type()), 5);
    BOOST_CHECK_EQUAL (return_int (5, std::is_same <long, long>()), 5);
    BOOST_CHECK_EQUAL (return_int (5, boost::mpl::true_()), 5);
    BOOST_CHECK_EQUAL (return_int (5, rime::true_), 5);
    BOOST_CHECK_EQUAL (return_int (5, rime::constant <bool, true>()), 5);

    BOOST_CHECK_EQUAL (return_int_2 (27, std::false_type()), -1);
    BOOST_CHECK_EQUAL (return_int_2 (27, std::is_same <int, short>()), -1);
    BOOST_CHECK_EQUAL (return_int_2 (27, boost::mpl::false_()), -1);
    BOOST_CHECK_EQUAL (return_int_2 (27, rime::false_), -1);
    BOOST_CHECK_EQUAL (return_int_2 (27, rime::constant <bool, false>()), -1);

    BOOST_CHECK_EQUAL (return_int_2 (27, std::true_type()), 27);
    BOOST_CHECK_EQUAL (return_int_2 (27, std::is_same <long, long>()), 27);
    BOOST_CHECK_EQUAL (return_int_2 (27, boost::mpl::true_()), 27);
    BOOST_CHECK_EQUAL (return_int_2 (27, rime::true_), 27);
    BOOST_CHECK_EQUAL (return_int_2 (27, rime::constant <bool, true>()), 27);
}

int return_int_3 (int i, rime::constant <int, -7>) { return i; }
int return_int_3 (int i, rime::constant <int, 81>) { return -1; }
int return_int_4 (int i, rime::int_ <-7>) { return i; }
int return_int_4 (int i, rime::int_ <81>) { return -1; }

BOOST_AUTO_TEST_CASE (test_rime_constant_convert_int) {
    BOOST_CHECK_EQUAL (
        return_int_3 (5, std::integral_constant <int, 81>()), -1);
    BOOST_CHECK_EQUAL (return_int_3 (5, boost::mpl::int_ <81>()), -1);
    BOOST_CHECK_EQUAL (return_int_3 (5, rime::int_ <81>()), -1);
    BOOST_CHECK_EQUAL (return_int_3 (5, rime::constant <int, 81>()), -1);

    BOOST_CHECK_EQUAL (return_int_3 (5, std::integral_constant <int, -7>()), 5);
    BOOST_CHECK_EQUAL (return_int_3 (5, boost::mpl::int_ <-7>()), 5);
    BOOST_CHECK_EQUAL (return_int_3 (5, rime::int_ <-7>()), 5);
    BOOST_CHECK_EQUAL (return_int_3 (5, rime::constant <int, -7>()), 5);

    BOOST_CHECK_EQUAL (
        return_int_4 (27, std::integral_constant <int, 81>()), -1);
    BOOST_CHECK_EQUAL (return_int_4 (27, boost::mpl::int_ <81>()), -1);
    BOOST_CHECK_EQUAL (return_int_4 (27, rime::int_ <81>()), -1);
    BOOST_CHECK_EQUAL (return_int_4 (27, rime::constant <int, 81>()), -1);

    BOOST_CHECK_EQUAL (
        return_int_4 (27, std::integral_constant <int, -7>()), 27);
    BOOST_CHECK_EQUAL (return_int_4 (27, boost::mpl::int_ <-7>()), 27);
    BOOST_CHECK_EQUAL (return_int_4 (27, rime::int_ <-7>()), 27);
    BOOST_CHECK_EQUAL (return_int_4 (27, rime::constant <int, -7>()), 27);
}

int return_int_5 (int i, rime::constant <std::size_t, 43>) { return i; }
int return_int_5 (int i, rime::constant <std::size_t, 83>) { return -1; }
int return_int_6 (int i, rime::size_t <43>) { return i; }
int return_int_6 (int i, rime::size_t <83>) { return -1; }

BOOST_AUTO_TEST_CASE (test_rime_constant_convert_size_t) {
    BOOST_CHECK_EQUAL (
        return_int_5 (5, std::integral_constant <std::size_t, 83>()), -1);
    BOOST_CHECK_EQUAL (return_int_5 (5, boost::mpl::size_t <83>()), -1);
    BOOST_CHECK_EQUAL (return_int_5 (5, rime::size_t <83>()), -1);
    BOOST_CHECK_EQUAL (
        return_int_5 (5, rime::constant <std::size_t, 83>()), -1);

    BOOST_CHECK_EQUAL (
        return_int_5 (5, std::integral_constant <std::size_t, 43>()), 5);
    BOOST_CHECK_EQUAL (return_int_5 (5, boost::mpl::size_t <43>()), 5);
    BOOST_CHECK_EQUAL (return_int_5 (5, rime::size_t <43>()), 5);
    BOOST_CHECK_EQUAL (return_int_5 (5, rime::constant <std::size_t, 43>()), 5);

    BOOST_CHECK_EQUAL (
        return_int_6 (27, std::integral_constant <std::size_t, 83>()), -1);
    BOOST_CHECK_EQUAL (return_int_6 (27, boost::mpl::size_t <83>()), -1);
    BOOST_CHECK_EQUAL (return_int_6 (27, rime::size_t <83>()), -1);
    BOOST_CHECK_EQUAL (
        return_int_6 (27, rime::constant <std::size_t, 83>()), -1);

    BOOST_CHECK_EQUAL (
        return_int_6 (27, std::integral_constant <std::size_t, 43>()), 27);
    BOOST_CHECK_EQUAL (return_int_6 (27, boost::mpl::size_t <43>()), 27);
    BOOST_CHECK_EQUAL (return_int_6 (27, rime::size_t <43>()), 27);
    BOOST_CHECK_EQUAL (
        return_int_6 (27, rime::constant <std::size_t, 43>()), 27);
}

/* Example with dispatching based on traits. */

template <class Unsigned>
    Unsigned add_check_overflow_impl (Unsigned i, Unsigned j, rime::false_type)
{
    Unsigned result = i + j;
    if (result < i)
        throw std::overflow_error ("Overflow in addition");
    else
        return result;
}

template <class Signed>
    Signed add_check_overflow_impl (Signed i, Signed j, rime::true_type)
{
    if ((0 < j) && (std::numeric_limits <Signed>::max() - j < i))
        throw std::overflow_error ("Overflow in signed addition");
    if ((j < 0) && (i < std::numeric_limits <Signed>::min() - j))
        throw std::overflow_error ("Overflow in signed addition");
    return i + j;
}

template <class Type> Type add_check_overflow (Type i, Type j) {
    return add_check_overflow_impl (i, j, std::is_signed <Type>());
}

BOOST_AUTO_TEST_CASE (test_rime_constant_convert_example) {
    // Unsigned.
    BOOST_CHECK_EQUAL (
        add_check_overflow (uint8_t (3), uint8_t (3)), uint8_t (6));
    BOOST_CHECK_EQUAL (
        add_check_overflow (uint8_t (0x80), uint8_t (0x7F)), uint8_t (0xFF));
    BOOST_CHECK_EQUAL (
        add_check_overflow (uint8_t (0x90), uint8_t (0x6F)), uint8_t (0xFF));

    BOOST_CHECK_THROW (
        add_check_overflow (uint8_t (0x80), uint8_t (0x80)), std::exception);
    BOOST_CHECK_THROW (
        add_check_overflow (uint8_t (0x90), uint8_t (0x71)), std::exception);
    BOOST_CHECK_THROW (
        add_check_overflow (uint8_t (0xFF), uint8_t (0xFF)), std::exception);

    // Unsigned.
    BOOST_CHECK_EQUAL (
        add_check_overflow (int8_t (3), int8_t (3)), int8_t (6));
    BOOST_CHECK_EQUAL (
        add_check_overflow (int8_t (0x7F), int8_t (0x00)), int8_t (0x7F));
    BOOST_CHECK_EQUAL (
        add_check_overflow (int8_t (0x3F), int8_t (0x40)), int8_t (0x7F));
    BOOST_CHECK_EQUAL (
        add_check_overflow (int8_t (0xA0), int8_t (0xE0)), int8_t (0x80));
    BOOST_CHECK_EQUAL (
        add_check_overflow (int8_t (0x80), int8_t (0x00)), int8_t (0x80));
    BOOST_CHECK_EQUAL (
        add_check_overflow (int8_t (0x80), int8_t (0x05)), int8_t (0x85));

    BOOST_CHECK_THROW (
        add_check_overflow (int8_t (0x7F), int8_t (1)), std::exception);
    BOOST_CHECK_THROW (
        add_check_overflow (int8_t (0x80), int8_t (-1)), std::exception);
    BOOST_CHECK_THROW (
        add_check_overflow (int8_t (0x7F), int8_t (0x7F)), std::exception);
    BOOST_CHECK_THROW (
        add_check_overflow (int8_t (0x80), int8_t (0x80)), std::exception);
}

BOOST_AUTO_TEST_CASE (test_rime_core_constant_assign) {
    // Constants should be assignable to themselves.
    {
        rime::true_type t;
        t = rime::true_type();
        rime::false_type f;
        f = rime::false_type();

        rime::int_ <3> three;
        three = three;

        rime::size_t <5> five;
        five = five;

        static_assert (utility::is_assignable <
            rime::true_type &, rime::true_type>::value, "");
        static_assert (!utility::is_assignable <
            rime::true_type &, rime::false_type>::value, "");
        static_assert (utility::is_assignable <
            rime::int_ <5> &, rime::int_ <5>>::value, "");
        static_assert (!utility::is_assignable <
            rime::int_ <5> &, rime::int_ <7>>::value, "");
    }
    // It should be possible to assign other constants to rime::constant.
    {
        rime::true_type t;
        t = std::true_type();
        rime::false_type f;
        f = boost::mpl::false_();

        rime::int_ <3> three;
        three = boost::mpl::int_ <3>();

        rime::size_t <5> five;
        five = std::integral_constant <size_t, 5>();


        static_assert (utility::is_assignable <
            rime::true_type &, std::true_type>::value, "");
        static_assert (!utility::is_assignable <
            rime::true_type &, boost::mpl::false_>::value, "");
        static_assert (utility::is_assignable <
            rime::int_ <5> &, boost::mpl::int_ <5>>::value, "");
        static_assert (!utility::is_assignable <
            rime::int_ <5> &, std::integral_constant <int, 7>>::value, "");
    }
}

struct merge_same {
    template <class Type1, class Type2> struct apply {};

    template <class Type> struct apply <Type, Type> { typedef Type type; };
};

BOOST_AUTO_TEST_CASE (test_rime_core_merge_constants) {
    typedef rime::merge_policy::constant <merge_same> constant;

    BOOST_MPL_ASSERT ((std::is_same <
        constant::apply <int, int>::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <
        constant::apply <rime::int_ <5>, int>::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <
        constant::apply <unsigned, rime::constant <unsigned, 12>>::type,
        unsigned>));
    BOOST_MPL_ASSERT ((std::is_same <
        constant::apply <rime::int_ <6>, rime::constant <int, 6>>::type,
        rime::constant <int, 6>>));
}

BOOST_AUTO_TEST_SUITE_END()
