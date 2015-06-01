/*
Copyright 2013 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_cast
#include "utility/test/boost_unit_test.hpp"

#include "rime/cast.hpp"

#include "rime/check/check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_cast)

BOOST_AUTO_TEST_CASE (test_rime_cast) {
    int i = 4;
    rime::int_<6> six;

    // Casting run-time values.
    RIME_CHECK_EQUAL (rime::cast <int> (4), 4);
    RIME_CHECK_EQUAL (rime::cast <int> (i), 4);

    RIME_CHECK_EQUAL (rime::cast <short> (4), short (4));
    RIME_CHECK_EQUAL (rime::cast <short> (i), short (4));

    // Casting constants.
    RIME_CHECK_EQUAL (rime::cast <int> (six), six);
    RIME_CHECK_EQUAL (rime::cast <short> (six), (rime::constant <short, 6>()));
    RIME_CHECK_EQUAL (rime::cast <std::size_t> (six), rime::size_t <6>());
}

BOOST_AUTO_TEST_CASE (test_rime_cast_value) {
    int i = 4;
    rime::int_<6> six;

    // Casting run-time values.
    RIME_CHECK_EQUAL (rime::cast_value <int> (4), 4);
    RIME_CHECK_EQUAL (rime::cast_value <int> (i), 4);

    RIME_CHECK_EQUAL (rime::cast_value <rime::int_<1>> (4), 4);
    RIME_CHECK_EQUAL (rime::cast_value <rime::int_<1>> (i), 4);

    RIME_CHECK_EQUAL (
        (rime::cast_value <rime::constant <short, 2>> (4)), short (4));
    RIME_CHECK_EQUAL (
        (rime::cast_value <rime::constant <short, 2>> (i)), short (4));

    // Casting constants.
    RIME_CHECK_EQUAL (rime::cast_value <int> (six), six);
    RIME_CHECK_EQUAL (
        rime::cast_value <short> (six), (rime::constant <short, 6>()));
    RIME_CHECK_EQUAL (rime::cast_value <std::size_t> (six), rime::size_t <6>());

    RIME_CHECK_EQUAL ((rime::cast_value <rime::constant <int, 8>> (six)), six);
    RIME_CHECK_EQUAL ((rime::cast_value <rime::constant <short, 8>> (six)),
        (rime::constant <short, 6>()));
    RIME_CHECK_EQUAL (
        (rime::cast_value <rime::size_t <8>> (six)), rime::size_t <6>());
}

BOOST_AUTO_TEST_SUITE_END()

