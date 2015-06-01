/*
Copyright 2013, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_min_max
#include "utility/test/boost_unit_test.hpp"

#include "rime/min.hpp"
#include "rime/max.hpp"

#include <type_traits>
#include <boost/mpl/assert.hpp>

#include "rime/check/check_equal.hpp"

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
