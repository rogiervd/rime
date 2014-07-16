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

#define BOOST_TEST_MODULE test_rime_nothing
#include "utility/test/boost_unit_test.hpp"

#include "rime/nothing.hpp"

#include <type_traits>
#include <string>

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_rime_nothing)

BOOST_AUTO_TEST_CASE (test_rime_nothing) {
    // Call "rime::nothing" with various arguments.
    BOOST_MPL_ASSERT ((std::is_same <void, decltype (rime::nothing())>));
    rime::nothing();

    BOOST_MPL_ASSERT ((std::is_same <void, decltype (rime::nothing (5))>));
    rime::nothing (5);

    std::string hello ("hello");

    BOOST_MPL_ASSERT ((std::is_same <void,
        decltype (rime::nothing (5, hello, 6.5))>));
    rime::nothing (5, hello, 6.5);

    // Make our own "nothing".
    rime::callable::nothing nothing;
    nothing (5, hello, 6.5);
}

BOOST_AUTO_TEST_SUITE_END()
