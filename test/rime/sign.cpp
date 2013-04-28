/*
Copyright 2013 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_sign
#include "../boost_unit_test.hpp"

#include "rime/sign.hpp"

#include <string>
#include <type_traits>

#include <boost/mpl/assert.hpp>
#include <boost/mpl/int.hpp>

#include "rime/core.hpp"

#include "check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_sign)

BOOST_AUTO_TEST_CASE (test_rime_sign_types) {
    // is_signed.
    static_assert (rime::types::is_signed <int>::value, "");
    static_assert (rime::types::is_signed <short>::value, "");
    static_assert (rime::types::is_signed <rime::int_ <-1>>::value, "");
    static_assert (rime::types::is_signed <boost::mpl::int_ <0>>::value, "");
    static_assert (rime::types::is_signed
        <std::integral_constant <int, 1>>::value, "");

    static_assert (!rime::types::is_signed <unsigned>::value, "");
    static_assert (!rime::types::is_signed <unsigned short>::value, "");
    static_assert (!rime::types::is_signed <std::size_t>::value, "");
    static_assert (!rime::types::is_signed <rime::size_t <0>>::value, "");
    static_assert (!rime::types::is_signed <rime::size_t <2>>::value, "");
    static_assert (!rime::types::is_signed <void>::value, "");
    static_assert (!rime::types::is_signed <std::string>::value, "");

    // is_unsigned.
    static_assert (rime::types::is_unsigned <unsigned>::value, "");
    static_assert (rime::types::is_unsigned <unsigned short>::value, "");
    static_assert (rime::types::is_unsigned <std::size_t>::value, "");
    static_assert (rime::types::is_unsigned <rime::size_t <0>>::value, "");
    static_assert (rime::types::is_unsigned <rime::size_t <2>>::value, "");

    static_assert (!rime::types::is_unsigned <int>::value, "");
    static_assert (!rime::types::is_unsigned <short>::value, "");
    static_assert (!rime::types::is_unsigned <rime::int_ <-1>>::value, "");
    static_assert (!rime::types::is_unsigned <boost::mpl::int_ <0>>::value, "");
    static_assert (!rime::types::is_unsigned <
        std::integral_constant <int, 1>>::value, "");

    static_assert (!rime::types::is_unsigned <void>::value, "");
    static_assert (!rime::types::is_unsigned <std::string>::value, "");

    // make_signed.
    BOOST_MPL_ASSERT ((std::is_same <
        typename rime::types::make_signed <int>::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename rime::types::make_signed <unsigned>::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename rime::types::make_signed <short>::type, short>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename rime::types::make_signed <unsigned short>::type, short>));

    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::types::make_signed <rime::int_ <1>>::type, rime::int_ <1>>));
    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::types::make_signed <rime::constant <unsigned, 1>>::type,
        rime::int_ <1>>));
    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::types::make_signed <std::integral_constant <unsigned, 2>>::type,
        rime::int_ <2>>));

    // make_unsigned.
    BOOST_MPL_ASSERT ((std::is_same <
        typename rime::types::make_unsigned <int>::type, unsigned>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename rime::types::make_unsigned <unsigned>::type, unsigned>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename rime::types::make_unsigned <short>::type, short unsigned>));
    BOOST_MPL_ASSERT ((std::is_same <
        typename rime::types::make_unsigned <unsigned short>::type,
        short unsigned>));

    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::types::make_unsigned <rime::int_ <1>>::type,
        rime::constant <unsigned, 1u>>));
    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::types::make_unsigned <rime::constant <int, 1>>::type,
        rime::constant <unsigned, 1u>>));
    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::types::make_unsigned <std::integral_constant <int, 2>>::type,
        rime::constant <unsigned, 2u>>));

    // make_zero.
    RIME_CHECK_EQUAL (rime::types::make_zero <rime::int_ <1>>::type(),
        rime::int_ <0>());
    RIME_CHECK_EQUAL (rime::types::make_zero <rime::int_ <-5>>::type(),
        rime::int_ <0>());
    RIME_CHECK_EQUAL (
        (rime::types::make_zero <rime::constant <unsigned, 1>>::type()),
        (rime::constant <unsigned, 0>()));
}

BOOST_AUTO_TEST_CASE (test_rime_sign_convert) {
    // make_signed.
    RIME_CHECK_EQUAL (rime::make_signed (1), 1);
    RIME_CHECK_EQUAL (rime::make_signed (1u), 1);

    RIME_CHECK_EQUAL (rime::make_signed (rime::int_ <1>()), rime::int_ <1>());
    RIME_CHECK_EQUAL (
        rime::make_signed (rime::constant <unsigned, 1u>()), rime::int_ <1>());

    // make_unsigned.
    RIME_CHECK_EQUAL (rime::make_unsigned (1), 1u);
    RIME_CHECK_EQUAL (rime::make_unsigned (1u), 1u);

    RIME_CHECK_EQUAL (rime::make_unsigned (rime::int_ <3>()),
        (rime::constant <unsigned, 3u>()));
    RIME_CHECK_EQUAL (rime::make_unsigned (rime::constant <unsigned, 2u>()),
        (rime::constant <unsigned, 2u>()));

    // make_zero.
    RIME_CHECK_EQUAL (rime::make_zero (1), 0);
    RIME_CHECK_EQUAL (rime::make_zero (1u), 0u);

    RIME_CHECK_EQUAL (rime::make_zero (rime::int_ <1>()), rime::int_ <0>());
    RIME_CHECK_EQUAL (
        rime::make_zero (rime::constant <unsigned, 1u>()),
        (rime::constant <unsigned, 0u>()));

}

// Signed constants.
typedef rime::int_ <unsigned (-1)/2 + 1> very_negative_int;
very_negative_int i1;
boost::mpl::int_ <-4> i2;
std::integral_constant <int, 0> i3;
rime::int_ <30> i4;

rime::constant <unsigned, 0> u3;
rime::constant <unsigned, 30> u4;
typedef rime::constant <unsigned, unsigned (-1)> very_great_unsigned;
very_great_unsigned u5;

// Sanity check for constants.
static_assert (very_negative_int::value < 0, "Should be negative");
static_assert (int (very_great_unsigned::value) < 0,
    "Unsigned should wrongly convert to negative");

template <bool reference, class Left, class Right>
    void check_less_sign_safe (Left const & left, Right const & right)
{
    // Known at compile time.
    RIME_CHECK_EQUAL (rime::less_sign_safe (left, right),
        rime::bool_ <reference>());

    // Known only at run time.
    RIME_CHECK_EQUAL (rime::less_sign_safe (Left::value, right), reference);
    RIME_CHECK_EQUAL (rime::less_sign_safe (left, Right::value), reference);
    RIME_CHECK_EQUAL (rime::less_sign_safe (Left::value, Right::value),
        reference);
}

// The case where if the left operand is constant, the result is known.
// I.e. -1 < u for any unsigned u.
template <bool reference, class Left, class Right>
    void check_less_sign_safe_left_constant (
        Left const & left, Right const & right)
{
    // Known at compile time.
    RIME_CHECK_EQUAL (rime::less_sign_safe (left, right),
        rime::bool_ <reference>());
    RIME_CHECK_EQUAL (rime::less_sign_safe (left, Right::value),
        rime::bool_ <reference>());

    // Known only at run time.
    RIME_CHECK_EQUAL (rime::less_sign_safe (Left::value, right), reference);
    RIME_CHECK_EQUAL (rime::less_sign_safe (Left::value, Right::value),
        reference);
}

// The case where if the right operand is constant, the result is known.
// I.e. u < -1 for any unsigned u.
template <bool reference, class Left, class Right>
    void check_less_sign_safe_right_constant (
        Left const & left, Right const & right)
{
    // Known at compile time.
    RIME_CHECK_EQUAL (rime::less_sign_safe (left, right),
        rime::bool_ <reference>());
    RIME_CHECK_EQUAL (rime::less_sign_safe (Left::value, right),
        rime::bool_ <reference>());

    // Known only at run time.
    RIME_CHECK_EQUAL (rime::less_sign_safe (left, Right::value), reference);
    RIME_CHECK_EQUAL (rime::less_sign_safe (Left::value, Right::value),
        reference);
}

struct not_an_int {};

bool operator < (not_an_int, not_an_int) { return false; }

BOOST_AUTO_TEST_CASE (test_rime_less_sign_safe) {
    check_less_sign_safe <false> (i1, i1);
    check_less_sign_safe <true> (i1, i2);
    check_less_sign_safe <true> (i1, i3);
    check_less_sign_safe_left_constant <true> (i1, u3);
    check_less_sign_safe <true> (i1, i4);
    check_less_sign_safe_left_constant <true> (i1, u4);
    check_less_sign_safe_left_constant <true> (i1, u5);

    check_less_sign_safe <false> (i2, i1);
    check_less_sign_safe <false> (i2, i2);
    check_less_sign_safe <true> (i2, i3);
    check_less_sign_safe_left_constant <true> (i2, u3);
    check_less_sign_safe <true> (i2, i4);
    check_less_sign_safe_left_constant <true> (i2, u4);
    check_less_sign_safe_left_constant <true> (i2, u5);

    check_less_sign_safe <false> (i3, i1);
    check_less_sign_safe <false> (i3, i2);
    check_less_sign_safe <false> (i3, i3);
    check_less_sign_safe <false> (i3, u3);
    check_less_sign_safe <true> (i3, i4);
    check_less_sign_safe <true> (i3, u4);
    check_less_sign_safe <true> (i3, u5);

    check_less_sign_safe <false> (i4, i1);
    check_less_sign_safe <false> (i4, i2);
    check_less_sign_safe <false> (i4, i3);
    check_less_sign_safe <false> (i4, u3);
    check_less_sign_safe <false> (i4, i4);
    check_less_sign_safe <false> (i4, u4);
    check_less_sign_safe <true> (i4, u5);

    check_less_sign_safe_right_constant  <false> (u3, i1);
    check_less_sign_safe_right_constant  <false> (u3, i2);
    check_less_sign_safe <false> (u3, i3);
    check_less_sign_safe <false> (u3, u3);
    check_less_sign_safe <true> (u3, i4);
    check_less_sign_safe <true> (u3, u4);
    check_less_sign_safe <true> (u3, u5);

    check_less_sign_safe_right_constant <false> (u4, i1);
    check_less_sign_safe_right_constant <false> (u4, i2);
    check_less_sign_safe <false> (u4, i3);
    check_less_sign_safe <false> (u4, u3);
    check_less_sign_safe <false> (u4, i4);
    check_less_sign_safe <false> (u4, u4);
    check_less_sign_safe <true> (u4, u5);

    check_less_sign_safe_right_constant <false> (u5, i1);
    check_less_sign_safe_right_constant <false> (u5, i2);
    check_less_sign_safe <false> (u5, i3);
    check_less_sign_safe <false> (u5, u3);
    check_less_sign_safe <false> (u5, i4);
    check_less_sign_safe <false> (u5, u4);
    check_less_sign_safe <false> (u5, u5);

    RIME_CHECK_EQUAL (rime::less_sign_safe (not_an_int(), not_an_int()), false);
}

BOOST_AUTO_TEST_SUITE_END()

