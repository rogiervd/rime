// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_rime_nothing
#include <boost/test/unit_test.hpp>

#include "rime/nothing.hpp"

#include <string>
#include <type_traits>

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_rime_nothing)

BOOST_AUTO_TEST_CASE(test_rime_nothing)
{
    // Call "rime::nothing" with various arguments.
    BOOST_MPL_ASSERT((std::is_same<void, decltype(rime::nothing())>) );
    rime::nothing();

    BOOST_MPL_ASSERT((std::is_same<void, decltype(rime::nothing(5))>) );
    rime::nothing(5);

    std::string hello("hello");

    BOOST_MPL_ASSERT(
        (std::is_same<void, decltype(rime::nothing(5, hello, 6.5))>) );
    rime::nothing(5, hello, 6.5);

    // Make our own "nothing".
    rime::callable::nothing nothing;
    nothing(5, hello, 6.5);
}

BOOST_AUTO_TEST_SUITE_END()
