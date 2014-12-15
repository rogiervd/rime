/*
Copyright 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_check_equal_fail
#include "utility/test/boost_unit_test.hpp"

#include "rime/check/check_equal.hpp"

BOOST_AUTO_TEST_SUITE(fail_rime_check_equal)

BOOST_AUTO_TEST_CASE (fail_rime_check_equal) {
    RIME_CHECK_EQUAL (rime::int_ <7>(),
        (std::integral_constant <short, 7>()));
}

BOOST_AUTO_TEST_SUITE_END()
