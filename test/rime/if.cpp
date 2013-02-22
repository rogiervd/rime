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

#define BOOST_TEST_MODULE test_rime_if
#include "../boost_unit_test.hpp"

#include "rime/if.hpp"

#include <type_traits>
#include <boost/mpl/assert.hpp>

#include "check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_if)

struct return_as_double {
    double operator() (double d) const { return d; }
};
struct return_as_long {
    long operator() (long l) const { return l; }
};

BOOST_AUTO_TEST_CASE (test_rime_if) {
    // Run-time conditions
    RIME_CHECK_EQUAL (rime::if_ (false, 3, 10), 10);
    RIME_CHECK_EQUAL (rime::if_ (true, 3, 10), 3);
    RIME_CHECK_EQUAL (rime::if_ (true, 3., 10.), 3.);

    // Results in boost::variant
    BOOST_CHECK_EQUAL (rime::get <double> (rime::if_ (false, 3, 10.3)), 10.3);
    BOOST_CHECK_EQUAL (rime::get <int> (rime::if_ (true, 3, 10.3)), 3);

    // Compile-time conditions
    RIME_CHECK_EQUAL (rime::if_ (rime::false_, 3, 10.3), 10.3);
    RIME_CHECK_EQUAL (rime::if_ (rime::true_, 3, 10.3), 3);

    // With C++11 compile-time constants.
    RIME_CHECK_EQUAL (rime::if_ (std::false_type(), 3, 10.3), 10.3);
    RIME_CHECK_EQUAL (rime::if_ (std::true_type(), 3, 10.3), 3);

    int i = 3;
    int const & ri = i;
    double d = 3.;
    double & rd = d;

    BOOST_MPL_ASSERT ((std::is_same <
        decltype (rime::if_ (rime::true_, 1u, rd)), unsigned>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (rime::if_ (rime::false_, ri, 2.3)), double>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (rime::if_ (rime::true_, i, rd)), int &>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (rime::if_ (rime::false_, ri, rd)), double &>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (rime::if_ (rime::true_, ri, rd)), int const &>));

    RIME_CHECK_EQUAL (
        rime::if_ (rime::true_, return_as_double(), return_as_long()) (6), 6.);
    RIME_CHECK_EQUAL (
        rime::if_ (rime::false_, return_as_double(), return_as_long()) (5), 5l);

    auto vd = rime::if_ (true, return_as_double(), return_as_long()) (2);
    BOOST_CHECK (vd.contains <double>());
    BOOST_CHECK_EQUAL (rime::get <double> (vd), 2.);
    auto vl = rime::if_ (false, return_as_double(), return_as_long()) (3);
    BOOST_CHECK (vl.contains <long>());
    BOOST_CHECK_EQUAL (rime::get <long> (vl), 3);
}

BOOST_AUTO_TEST_SUITE_END()

