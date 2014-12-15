/*
Copyright 2011, 2012, 2014 Rogier van Dalen.

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

/** \file
Check that two objects have exactly the same value.
Include <boost/test/unit_test.hpp> before this.
*/

#ifndef RIME_CHECK_CHECK_EQUAL_HPP_INCLUDED
#define RIME_CHECK_CHECK_EQUAL_HPP_INCLUDED

#include <type_traits>

#include "rime/core.hpp"

namespace rime {

template <class Left, class Right>
    struct exactly_equal_implementation
: std::integral_constant <bool,
    std::is_same <Left, Right>::value
        || (rime::is_constant <Left>::value
            && rime::is_constant <Right>::value
            && std::is_same <typename rime::value <Left>::type,
                typename rime::value <Right>::type>::value
            && rime::equal_constant <Left, Right>::value)> {};

template <class Left, class Right>
    struct exactly_equal
: exactly_equal_implementation <
    typename std::decay <Left>::type, typename std::decay <Right>::type> {};

template <class Left, class Right> void static_assert_exactly_equal() {
    static_assert (exactly_equal <Left, Right>::value,
        "The offending types are in the instantiation of this message.");
}

} // namespace rime

/**
Check whether a and b are either:
\li of the same run-time type and the same value at run-time;
\li or constants with the same value type and same constant value.

That is, <c>rime::int_<7></c> is not equal to <c>(int) 7</c>.
However, it is equal to <c>std::integral_constant \<int, 7><c/>.
*/
#define RIME_CHECK_EQUAL(a, b)\
    static_assert (rime::exactly_equal <decltype (a), decltype (b)>::value, \
        "Values must either be the same run-time type, or both compile-time " \
        "constants with the same value."); \
    rime::static_assert_exactly_equal <decltype (a), decltype (b)>(); \
    BOOST_CHECK_EQUAL (a, b)

#endif // RIME_CHECK_CHECK_EQUAL_HPP_INCLUDED
