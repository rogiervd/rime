/*
Copyright 2011, 2012 Rogier van Dalen.

This file is part of Rogier van Dalen's Range library for C++.

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

#define BOOST_TEST_MODULE test_rime_detail_switch
#include "../boost_unit_test.hpp"

#include <boost/mpl/assert.hpp>

#include "rime/detail/switch.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_detail_switch)

struct struct_default {
    float operator() (float f) const { return -1.0; }
};

struct struct_times_5 {
    float operator() (float f) const { return 5*f; }
};

struct struct_times_7 {
    float operator() (float f) const { return 7*f; }
};

using rime::detail::switch_;

BOOST_AUTO_TEST_CASE (test_rime_detail_switch) {
    switch_ <double,
        meta::vector <struct_default, struct_times_5, struct_times_7> > s;
    BOOST_CHECK_EQUAL (s (0, 7), -1.);
    BOOST_CHECK_EQUAL (s (1, 10), 50.);
    BOOST_CHECK_EQUAL (s (2, 11), 77.);
}

BOOST_AUTO_TEST_SUITE_END()

