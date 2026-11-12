// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_rime_always
#include "boost/test/unit_test.hpp"

#include "rime/always.hpp"

#include <string>
#include <type_traits>

#include "boost/mpl/assert.hpp"

#include "rime/core.hpp"

struct example_type
{
    int i;
    example_type() : i(27) {}
};

BOOST_AUTO_TEST_SUITE(test_rime_always)

BOOST_AUTO_TEST_CASE(test_rime_always)
{
    std::string hello("hello");

    // Run-time value.
    {
        auto f = rime::always(5);
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f())>) );
        BOOST_CHECK_EQUAL(f(), 5);
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f(7))>) );
        BOOST_CHECK_EQUAL(f(7), 5);
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f(8, hello, 87.5))>) );
        BOOST_CHECK_EQUAL(f(8, hello, 87.5), 5);
    }
    // Run-time value which is a reference.
    {
        int i = 7;
        auto f = rime::always<int &>(i);
        BOOST_MPL_ASSERT((std::is_same<int &, decltype(f())>) );
        BOOST_CHECK_EQUAL(f(), 7);

        i = 8;
        BOOST_MPL_ASSERT((std::is_same<int &, decltype(f(7))>) );
        BOOST_CHECK_EQUAL(f(7), 8);

        i = 9;
        BOOST_MPL_ASSERT((std::is_same<int &, decltype(f(8, hello, 87.5))>) );
        BOOST_CHECK_EQUAL(f(8, hello, 87.5), 9);
    }
    // Compile-time value.
    {
        auto f = rime::always<int, 5>();
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f())>) );
        BOOST_CHECK_EQUAL(f(), 5);
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f(7))>) );
        BOOST_CHECK_EQUAL(f(7), 5);
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f(8, hello, 87.5))>) );
        BOOST_CHECK_EQUAL(f(8, hello, 87.5), 5);
    }

    // Default-constructed type.
    {
        auto f = rime::always<int>();
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f())>) );
        BOOST_CHECK_EQUAL(f(), 0);
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f(7))>) );
        BOOST_CHECK_EQUAL(f(7), 0);
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f(8, hello, 87.5))>) );
        BOOST_CHECK_EQUAL(f(8, hello, 87.5), 0);
    }
    {
        auto f = rime::always<example_type>();
        BOOST_MPL_ASSERT((std::is_same<example_type, decltype(f())>) );
        BOOST_CHECK_EQUAL(f().i, 27);
    }
    // A major use case
    {
        auto f = rime::always(rime::false_);
        BOOST_MPL_ASSERT((std::is_same<rime::false_type, decltype(f())>) );
    }
    {
        auto f = rime::always<rime::false_type>();
        BOOST_MPL_ASSERT((std::is_same<rime::false_type, decltype(f())>) );
    }

    // Explicitly use the types in rime::callable.
    {
        rime::callable::always<int, 5> f;
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f())>) );
        BOOST_CHECK_EQUAL(f(), 5);
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f(7))>) );
        BOOST_CHECK_EQUAL(f(7), 5);
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f(8, hello, 87.5))>) );
        BOOST_CHECK_EQUAL(f(8, hello, 87.5), 5);
    }
    {
        rime::callable::always_default<int> f;
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f())>) );
        BOOST_CHECK_EQUAL(f(), 0);
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f(7))>) );
        BOOST_CHECK_EQUAL(f(7), 0);
        BOOST_MPL_ASSERT((std::is_same<int, decltype(f(8, hello, 87.5))>) );
        BOOST_CHECK_EQUAL(f(8, hello, 87.5), 0);
    }
}

BOOST_AUTO_TEST_SUITE_END()
