/*
Copyright 2013, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_min_max
#include "utility/test/boost_unit_test.hpp"

#include "rime/min.hpp"
#include "rime/max.hpp"

#include <type_traits>
#include <boost/mpl/assert.hpp>

#include "check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_min_max)

BOOST_AUTO_TEST_CASE (test_rime_min_max__common_constant) {
    typedef rime::merge_policy::constant <rime::merge_policy::common_type>
        common_constant;

    BOOST_MPL_ASSERT ((std::is_same <
        common_constant::apply <unsigned char, float>::type, float>));
    BOOST_MPL_ASSERT ((std::is_same <
        common_constant::apply <rime::int_ <5>, float>::type, float>));
    BOOST_MPL_ASSERT ((std::is_same <
        common_constant::apply <rime::int_ <5>, rime::constant <unsigned, 5>
        >::type, rime::constant <unsigned, 5>>));
}

BOOST_AUTO_TEST_CASE (test_rime_min_max) {
    using rime::min;
    using rime::max;

    rime::constant <short, 5> five;
    std::integral_constant <unsigned short, 10> ten;
    std::integral_constant <short, 15> fifteen;

    // Run-time.
    RIME_CHECK_EQUAL (min (1, 2), 1);
    RIME_CHECK_EQUAL (min (7, 2), 2);

    RIME_CHECK_EQUAL (max (1, 2), 2);
    RIME_CHECK_EQUAL (max (7, 2), 7);

    // Run-time and different types (signed and unsigned).
    RIME_CHECK_EQUAL (rime::get <int> (min (1, 5u)), 1);
    RIME_CHECK_EQUAL (rime::get <int> (min (5u, 1)), 1);
    RIME_CHECK_EQUAL (rime::get <unsigned> (min (7, 5u)), 5u);
    RIME_CHECK_EQUAL (rime::get <unsigned> (min (5u, 7)), 5u);

    RIME_CHECK_EQUAL (rime::get <unsigned> (max (1, 5u)), 5u);
    RIME_CHECK_EQUAL (rime::get <unsigned> (max (5u, 1)), 5u);
    RIME_CHECK_EQUAL (rime::get <int> (max (7, 5u)), 7);
    RIME_CHECK_EQUAL (rime::get <int> (max (5u, 7)), 7);

    // Run-time and compile-time, same type.
    RIME_CHECK_EQUAL (min (short (1), five), short (1));
    RIME_CHECK_EQUAL (min (short (7), five), short (5));

    RIME_CHECK_EQUAL (max (short (1), five), short (5));
    RIME_CHECK_EQUAL (max (short (7), five), short (7));

    // Run-time and compile time, different types: produces variant.
    RIME_CHECK_EQUAL (rime::get <int> (min (1, five)), 1);
    RIME_CHECK_EQUAL (rime::get <int> (min (five, 1)), 1);
    RIME_CHECK_EQUAL (rime::get <decltype (five) &> (min (7, five)), five);
    RIME_CHECK_EQUAL (rime::get <decltype (five) &> (min (five, 7)), five);

    RIME_CHECK_EQUAL (rime::get <decltype (five) &> (max (1, five)), five);
    RIME_CHECK_EQUAL (rime::get <decltype (five) &> (max (five, 1)), five);
    RIME_CHECK_EQUAL (rime::get <int> (max (7, five)), 7);
    RIME_CHECK_EQUAL (rime::get <int> (max (five, 7)), 7);

    // Known at compile-time.
    RIME_CHECK_EQUAL (min (ten, five), five);
    RIME_CHECK_EQUAL (min (five, ten), five);

    RIME_CHECK_EQUAL (max (ten, five), ten);
    RIME_CHECK_EQUAL (max (five, ten), ten);

    RIME_CHECK_EQUAL (min (ten, fifteen), ten);
    RIME_CHECK_EQUAL (max (ten, fifteen), fifteen);
}

BOOST_AUTO_TEST_CASE (test_rime_min_max__policy) {
    typedef rime::merge_policy::constant <rime::merge_policy::common_type>
        merge_policy;

    BOOST_MPL_ASSERT ((std::is_same <
        merge_policy::apply <int, char>::type, int>));

    auto v1 = rime::min_ <merge_policy> (char (7), double (3));
    BOOST_MPL_ASSERT ((std::is_same <decltype (v1), double>));
    BOOST_CHECK_EQUAL (v1, 3.);

    auto v2 = rime::max_ <merge_policy> (char (7), double (3));
    BOOST_MPL_ASSERT ((std::is_same <decltype (v1), double>));
    BOOST_CHECK_EQUAL (v2, 7.);

    v1 = rime::min_ <merge_policy> (char (4), double (7));
    BOOST_CHECK_EQUAL (v1, 4.);

    v2 = rime::max_ <merge_policy> (char (4), double (7));
    BOOST_CHECK_EQUAL (v2, 7.);

    char c = 1;
    int i = 5;

    auto v3 = rime::min_ <merge_policy> (c, i);
    BOOST_MPL_ASSERT ((std::is_same <decltype (v3), int>));
    BOOST_CHECK_EQUAL (v3, 1);

    auto v4 = rime::max_ <merge_policy> (c, i);
    BOOST_MPL_ASSERT ((std::is_same <decltype (v3), int>));
    BOOST_CHECK_EQUAL (v4, 5);

    v3 = rime::min_ <merge_policy> (char (7), i);
    BOOST_CHECK_EQUAL (v3, 5);

    v4 = rime::max_ <merge_policy> (char (7), i);
    BOOST_CHECK_EQUAL (v4, 7);
}

BOOST_AUTO_TEST_SUITE_END()
