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

#define BOOST_TEST_MODULE test_utility_is_assignable
#include "../boost_unit_test.hpp"

#include "utility/is_assignable.hpp"

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_utility_is_assignable)

struct unassignable {
    unassignable & operator = (unassignable const &) = delete;
};

struct assignable {
    template <class Type>
    assignable & operator = (Type const &) { return *this; }
};

BOOST_AUTO_TEST_CASE (test_utility_is_assignable) {
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <void, int>));
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <void, void>));
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <void, unassignable>));
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <void, assignable>));

    // 6 = 7 does not compile.
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <int, int>));
    BOOST_MPL_ASSERT ((utility::is_assignable <int &, int>));

    BOOST_MPL_ASSERT ((utility::is_assignable <int &, int const &>));
    BOOST_MPL_ASSERT_NOT ((utility::is_assignable <int const &, int const &>));

    // This does compile.
    assignable() = 5;

    BOOST_MPL_ASSERT_NOT ((
        utility::is_assignable <unassignable &, unassignable>));
    BOOST_MPL_ASSERT ((
        utility::is_assignable <assignable, int>));
    BOOST_MPL_ASSERT ((
        utility::is_assignable <assignable &, int>));
    BOOST_MPL_ASSERT ((
        utility::is_assignable <assignable &, unassignable>));
}

BOOST_AUTO_TEST_SUITE_END()

