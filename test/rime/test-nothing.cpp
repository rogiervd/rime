// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_rime_nothing
#include "boost/test/unit_test.hpp"

#include "rime/nothing.hpp"

#include <string>
#include <type_traits>


BOOST_AUTO_TEST_SUITE(test_rime_nothing)

BOOST_AUTO_TEST_CASE(test_rime_nothing)
{
    // Call "rime::nothing" with various arguments.
    static_assert(std::is_same_v<void, decltype(rime::nothing())>);
    rime::nothing();

    static_assert(std::is_same_v<void, decltype(rime::nothing(5))>);
    rime::nothing(5);

    std::string hello("hello");

    static_assert(std::is_same_v<void, decltype(rime::nothing(5, hello, 6.5))>);
    rime::nothing(5, hello, 6.5);

    // Make our own "nothing".
    rime::callable::nothing nothing;
    nothing(5, hello, 6.5);
}

BOOST_AUTO_TEST_SUITE_END()
