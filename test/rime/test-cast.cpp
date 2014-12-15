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

