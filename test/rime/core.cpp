/*
Copyright 2011, 2012 Rogier van Dalen.

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
#include "../boost_unit_test.hpp"

#include "rime/core.hpp"

#include <type_traits>

#include <boost/mpl/int.hpp>

#include <boost/mpl/assert.hpp>

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
    BOOST_MPL_ASSERT ((rime::is_rime_constant <rime::int_ <3> >));
    BOOST_MPL_ASSERT ((rime::is_rime_constant <rime::constant <long, 3> >));

    // is_constant
    BOOST_MPL_ASSERT ((rime::is_constant <std::true_type>));
    BOOST_MPL_ASSERT ((rime::is_constant <std::integral_constant <int, 3> >));
    BOOST_MPL_ASSERT ((rime::is_constant <boost::mpl::true_>));
    BOOST_MPL_ASSERT ((rime::is_constant <boost::mpl::int_ <3> >));
    BOOST_MPL_ASSERT ((rime::is_constant <rime::bool_<true> >));
    BOOST_MPL_ASSERT ((rime::is_constant <rime::int_ <3> >));
    BOOST_MPL_ASSERT ((rime::is_constant <rime::constant <long, 3> >));

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

BOOST_AUTO_TEST_SUITE_END()

