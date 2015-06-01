/*
Copyright 2014 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/** \file
This tests RIME_CHECK_EQUAL with things that are equal.
Different types of failure are in fail-check_equal.
*/

#define BOOST_TEST_MODULE test_rime_check_equal
#include "utility/test/boost_unit_test.hpp"

#include "rime/check/check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_check_equal)

/// Object that can be compared with BOOST_CHECK_EQUAL.
struct object { int i; };

bool operator== (object const & a, object const & b) { return a.i == b.i; }

std::ostream & operator<< (std::ostream & os, object const & o)
{ return os << o.i; }

BOOST_AUTO_TEST_CASE (test_rime_check_equal) {
    RIME_CHECK_EQUAL (4, 4);
    short s = 4;
    RIME_CHECK_EQUAL (s, short (4));

    object a;
    a.i = 5;
    object b;
    b.i = 5;
    RIME_CHECK_EQUAL (a, b);

    RIME_CHECK_EQUAL (rime::false_, (std::integral_constant <bool, false>()));
    RIME_CHECK_EQUAL (rime::size_t <1>(),
        (std::integral_constant <std::size_t, 1>()));
}

BOOST_AUTO_TEST_SUITE_END()
