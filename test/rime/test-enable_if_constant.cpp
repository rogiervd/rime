/*
Copyright 2011, 2012 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_enable_if_constant
#include "utility/test/boost_unit_test.hpp"

#include "rime/enable_if_constant.hpp"

#include <type_traits>

#include <rime/core.hpp>

struct runtime_type {};

BOOST_AUTO_TEST_SUITE(test_rime_enable_if_constant)

// This takes Argument const & and the next Argument &&.
// This is so that the compiler understands that the signatures are different.
template <class Argument, class Enable = typename
    rime::enable_if_constant <Argument>::type>
bool is_constant_1 (Argument const &) { return true; }

template <class Argument, class Enable = typename
    rime::disable_if_constant <Argument>::type>
bool is_constant_1 (Argument &&) { return false; }

template <class Argument>
bool is_constant_2 (Argument const &,
    typename rime::enable_if_constant <Argument, int>::type) { return true; }

template <class Argument>
bool is_constant_2 (Argument &&,
    typename rime::disable_if_constant <Argument, int>::type) { return false; }

BOOST_AUTO_TEST_CASE (test_rime_enable_if_constant) {
    rime::false_type constant_false;
    rime::true_type constant_true;
    rime::int_ <0> constant_zero;
    rime::int_ <7> constant_seven;
    bool runtime_false = false;
    runtime_type runtime;

    BOOST_CHECK_EQUAL (is_constant_1 (constant_false), true);
    BOOST_CHECK_EQUAL (is_constant_1 (constant_true), true);
    BOOST_CHECK_EQUAL (is_constant_1 (constant_zero), true);
    BOOST_CHECK_EQUAL (is_constant_1 (constant_seven), true);
    BOOST_CHECK_EQUAL (is_constant_1 (runtime_false), false);
    BOOST_CHECK_EQUAL (is_constant_1 (runtime), false);

    BOOST_CHECK_EQUAL (is_constant_2 (constant_false, 5), true);
    BOOST_CHECK_EQUAL (is_constant_2 (constant_true, 5), true);
    BOOST_CHECK_EQUAL (is_constant_2 (constant_zero, 5), true);
    BOOST_CHECK_EQUAL (is_constant_2 (constant_seven, 5), true);
    BOOST_CHECK_EQUAL (is_constant_2 (runtime_false, 5), false);
    BOOST_CHECK_EQUAL (is_constant_2 (runtime, 5), false);
}

// This takes Argument const & and the next Argument &&.
// This is so that the compiler understands that the signatures are different.
template <class Argument, class Enable = typename
    rime::enable_if_constant_true <Argument>::type>
bool is_constant_true_1 (Argument const &) { return true; }

template <class Argument, class Enable = typename
    rime::disable_if_constant_true <Argument>::type>
bool is_constant_true_1 (Argument &&) { return false; }

template <class Argument>
bool is_constant_true_2 (Argument const &,
    typename rime::enable_if_constant_true <Argument, int>::type)
{ return true; }

template <class Argument>
bool is_constant_true_2 (Argument &&,
    typename rime::disable_if_constant_true <Argument, int>::type)
{ return false; }

BOOST_AUTO_TEST_CASE (test_rime_enable_if_constant_true) {
    rime::false_type constant_false;
    rime::true_type constant_true;
    rime::int_ <0> constant_zero;
    rime::int_ <7> constant_seven;
    bool runtime_false = false;
    runtime_type runtime;

    BOOST_CHECK_EQUAL (is_constant_true_1 (constant_false), false);
    BOOST_CHECK_EQUAL (is_constant_true_1 (constant_true), true);
    BOOST_CHECK_EQUAL (is_constant_true_1 (constant_zero), false);
    BOOST_CHECK_EQUAL (is_constant_true_1 (constant_seven), true);
    BOOST_CHECK_EQUAL (is_constant_true_1 (runtime_false), false);
    BOOST_CHECK_EQUAL (is_constant_true_1 (runtime), false);

    BOOST_CHECK_EQUAL (is_constant_true_2 (constant_false, 5), false);
    BOOST_CHECK_EQUAL (is_constant_true_2 (constant_true, 5), true);
    BOOST_CHECK_EQUAL (is_constant_true_2 (constant_zero, 5), false);
    BOOST_CHECK_EQUAL (is_constant_true_2 (constant_seven, 5), true);
    BOOST_CHECK_EQUAL (is_constant_true_2 (runtime_false, 5), false);
    BOOST_CHECK_EQUAL (is_constant_true_2 (runtime, 5), false);
}

// This takes Argument const & and the next Argument &&.
// This is so that the compiler understands that the signatures are different.
template <class Argument, class Enable = typename
    rime::enable_if_constant_false <Argument>::type>
bool is_constant_false_1 (Argument const &) { return true; }

template <class Argument, class Enable = typename
    rime::disable_if_constant_false <Argument>::type>
bool is_constant_false_1 (Argument &&) { return false; }

template <class Argument>
bool is_constant_false_2 (Argument const &,
    typename rime::enable_if_constant_false <Argument, int>::type)
{ return true; }

template <class Argument>
bool is_constant_false_2 (Argument &&,
    typename rime::disable_if_constant_false <Argument, int>::type)
{ return false; }

BOOST_AUTO_TEST_CASE (test_rime_enable_if_constant_false) {
    rime::false_type constant_false;
    rime::true_type constant_true;
    rime::int_ <0> constant_zero;
    rime::int_ <7> constant_seven;
    bool runtime_false = false;
    runtime_type runtime;

    BOOST_CHECK_EQUAL (is_constant_false_1 (constant_false), true);
    BOOST_CHECK_EQUAL (is_constant_false_1 (constant_true), false);
    BOOST_CHECK_EQUAL (is_constant_false_1 (constant_zero), true);
    BOOST_CHECK_EQUAL (is_constant_false_1 (constant_seven), false);
    BOOST_CHECK_EQUAL (is_constant_false_1 (runtime_false), false);
    BOOST_CHECK_EQUAL (is_constant_false_1 (runtime), false);

    BOOST_CHECK_EQUAL (is_constant_false_2 (constant_false, 5), true);
    BOOST_CHECK_EQUAL (is_constant_false_2 (constant_true, 5), false);
    BOOST_CHECK_EQUAL (is_constant_false_2 (constant_zero, 5), true);
    BOOST_CHECK_EQUAL (is_constant_false_2 (constant_seven, 5), false);
    BOOST_CHECK_EQUAL (is_constant_false_2 (runtime_false, 5), false);
    BOOST_CHECK_EQUAL (is_constant_false_2 (runtime, 5), false);
}

BOOST_AUTO_TEST_SUITE_END()
