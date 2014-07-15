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

#define BOOST_TEST_MODULE test_rime_variant
#include "utility/test/boost_unit_test.hpp"

#include "rime/core.hpp"

#include <type_traits>

#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/assert.hpp>
#include "check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_operators)

// \TODO make sure this works correctly without Koenig lookup!

template <class Type, Type Content, class ReferenceType>
    void check_constant_equals (rime::constant <Type, Content> const &,
        ReferenceType const & reference)
{
    BOOST_MPL_ASSERT ((std::is_same <Type, ReferenceType>));
    BOOST_CHECK_EQUAL (Content, reference);
}

#define TEST_BINARY_OPERATOR_ON_TYPE(TYPE, NAME, OPERATOR, LEFT, RIGHT) { \
    rime::constant <TYPE, LEFT> left; \
    rime::constant <TYPE, RIGHT> right; \
    boost::mpl::integral_c <TYPE, LEFT> mpl_left; \
    boost::mpl::integral_c <TYPE, RIGHT> mpl_right; \
\
    /* Explicit function */ \
    check_constant_equals (rime::NAME (left, right), (LEFT OPERATOR RIGHT)); \
    /* Infect with MPL constant: return compile-time constant */ \
    check_constant_equals (rime::NAME (mpl_left, right), \
        (LEFT OPERATOR RIGHT)); \
    check_constant_equals (rime::NAME (left, mpl_right), \
        (LEFT OPERATOR RIGHT)); \
    check_constant_equals (rime::NAME (mpl_left, mpl_right), \
        (LEFT OPERATOR RIGHT)); \
    /* Infect with runtime value: return runtime value */ \
    RIME_CHECK_EQUAL (rime::NAME (left, RIGHT), (LEFT OPERATOR RIGHT)); \
    RIME_CHECK_EQUAL (rime::NAME (LEFT, right), (LEFT OPERATOR RIGHT)); \
    RIME_CHECK_EQUAL (rime::NAME (LEFT, RIGHT), (LEFT OPERATOR RIGHT)); \
\
    /* Operator with constant */ \
    check_constant_equals ((left OPERATOR right), (LEFT OPERATOR RIGHT)); \
    /* Infect with MPL constant: return compile-time constant */ \
    check_constant_equals ((mpl_left OPERATOR right), (LEFT OPERATOR RIGHT)); \
    check_constant_equals ((left OPERATOR mpl_right), (LEFT OPERATOR RIGHT)); \
    /* Two MPL constants: no change to behaviour, return runtime value */ \
    RIME_CHECK_EQUAL ((mpl_left OPERATOR mpl_right), (LEFT OPERATOR RIGHT)); \
    /* Infect with runtime value: return runtime value */ \
    RIME_CHECK_EQUAL ((left OPERATOR RIGHT), (LEFT OPERATOR RIGHT)); \
    RIME_CHECK_EQUAL ((LEFT OPERATOR right), (LEFT OPERATOR RIGHT)); \
}

#define TEST_UNARY_OPERATOR_ON_TYPE(TYPE, NAME, OPERATOR, VALUE) { \
    rime::constant <TYPE, VALUE> value; \
    boost::mpl::integral_c <TYPE, VALUE> mpl_value; \
\
    /* Explicit function */ \
    check_constant_equals (rime::NAME (value), (OPERATOR VALUE)); \
    check_constant_equals (rime::NAME (mpl_value), (OPERATOR VALUE)); \
    /* Operator */\
    check_constant_equals (OPERATOR value, (OPERATOR VALUE)); \
    RIME_CHECK_EQUAL (OPERATOR mpl_value, (OPERATOR VALUE)); \
    RIME_CHECK_EQUAL (rime::NAME (VALUE), (OPERATOR VALUE)); \
}

#define TEST_BINARY_OPERATOR_ON(NAME, OPERATOR, LEFT, RIGHT) \
    TEST_BINARY_OPERATOR_ON_TYPE(int, NAME, OPERATOR, LEFT, RIGHT)
#define TEST_UNARY_OPERATOR_ON(NAME, OPERATOR, VALUE) \
    TEST_UNARY_OPERATOR_ON_TYPE(int, NAME, OPERATOR, VALUE)

#define TEST_BINARY_OPERATOR_ON_FLOAT(NAME, OPERATOR, LEFT, RIGHT) { \
    rime::int_ <LEFT> left; \
    rime::int_ <RIGHT> right; \
\
    /* Explicit function */ \
    RIME_CHECK_EQUAL (rime::NAME (left, double (RIGHT)), \
        (LEFT OPERATOR double (RIGHT))); \
    RIME_CHECK_EQUAL (rime::NAME (double (LEFT), right), \
        (double (LEFT) OPERATOR RIGHT)); \
    RIME_CHECK_EQUAL (rime::NAME (double (LEFT), double (RIGHT)), \
        (double (LEFT) OPERATOR double (RIGHT))); \
\
    /* Operator with constant */ \
    RIME_CHECK_EQUAL ((left OPERATOR double (RIGHT)), \
        (LEFT OPERATOR double (RIGHT))); \
    RIME_CHECK_EQUAL ((double (LEFT) OPERATOR right), \
        (double (LEFT) OPERATOR RIGHT)); \
}

#define DEFINE_TEST_BINARY_OPERATOR_IMPLEMENTATION(APPLY, NAME, OPERATOR) \
    APPLY (NAME, OPERATOR, 4, 5) \
    APPLY (NAME, OPERATOR, 7, 5) \
    APPLY (NAME, OPERATOR, 3, 3)

#define DEFINE_TEST_UNARY_OPERATOR_IMPLEMENTATION(APPLY, NAME, OPERATOR) \
    APPLY (NAME, OPERATOR, 5) \
    APPLY (NAME, OPERATOR, -3) \
    APPLY (NAME, OPERATOR, 1) \
    APPLY (NAME, OPERATOR, 0)

// Test all
#define DEFINE_TEST_BINARY_OPERATOR(NAME, OPERATOR) \
BOOST_AUTO_TEST_CASE (test_operator_##NAME) { \
    DEFINE_TEST_BINARY_OPERATOR_IMPLEMENTATION ( \
        TEST_BINARY_OPERATOR_ON, NAME, OPERATOR) \
    DEFINE_TEST_BINARY_OPERATOR_IMPLEMENTATION ( \
        TEST_BINARY_OPERATOR_ON_FLOAT, NAME, OPERATOR) \
    TEST_BINARY_OPERATOR_ON (NAME, OPERATOR, 0, -1) \
    TEST_BINARY_OPERATOR_ON (NAME, OPERATOR, 0, 0) \
}

#define DEFINE_TEST_UNARY_OPERATOR(NAME, OPERATOR) \
BOOST_AUTO_TEST_CASE (test_operator_##NAME) { \
    DEFINE_TEST_UNARY_OPERATOR_IMPLEMENTATION ( \
        TEST_UNARY_OPERATOR_ON, NAME, OPERATOR) \
}

// Test all except 0/0
#define DEFINE_TEST_DIVIDE_OPERATOR(NAME, OPERATOR) \
BOOST_AUTO_TEST_CASE (test_operator_##NAME) { \
    DEFINE_TEST_BINARY_OPERATOR_IMPLEMENTATION ( \
        TEST_BINARY_OPERATOR_ON, NAME, OPERATOR) \
    TEST_BINARY_OPERATOR_ON (NAME, OPERATOR, 0, -1) \
}

// Test all except shift by negative amount
#define DEFINE_TEST_SHIFT_OPERATOR(NAME, OPERATOR) \
BOOST_AUTO_TEST_CASE (test_operator_##NAME) { \
    DEFINE_TEST_BINARY_OPERATOR_IMPLEMENTATION ( \
        TEST_BINARY_OPERATOR_ON, NAME, OPERATOR) \
    TEST_BINARY_OPERATOR_ON (NAME, OPERATOR, 0, 0) \
}

DEFINE_TEST_UNARY_OPERATOR(negate, -)
DEFINE_TEST_UNARY_OPERATOR(not_, !)
DEFINE_TEST_UNARY_OPERATOR(bitwise_not, ~)

DEFINE_TEST_BINARY_OPERATOR(equal_to, ==)
DEFINE_TEST_BINARY_OPERATOR(not_equal_to, !=)
DEFINE_TEST_BINARY_OPERATOR(less, <)
DEFINE_TEST_BINARY_OPERATOR(less_equal, <=)
DEFINE_TEST_BINARY_OPERATOR(greater, >)
DEFINE_TEST_BINARY_OPERATOR(greater_equal, >=)

DEFINE_TEST_BINARY_OPERATOR(plus, +)
DEFINE_TEST_BINARY_OPERATOR(minus, -)
DEFINE_TEST_BINARY_OPERATOR(times, *)
DEFINE_TEST_DIVIDE_OPERATOR(divides, /)
DEFINE_TEST_DIVIDE_OPERATOR(modulo, %)

DEFINE_TEST_SHIFT_OPERATOR(bitwise_and, &)
DEFINE_TEST_SHIFT_OPERATOR(bitwise_or, |)
DEFINE_TEST_SHIFT_OPERATOR(bitwise_xor, ^)
DEFINE_TEST_SHIFT_OPERATOR(shift_right, >>)
DEFINE_TEST_SHIFT_OPERATOR(shift_left, <<)

/*
and_ and or_ are written out, because
    or_ (rime::true_(), false)
is rime::true_() et cetera.
*/
BOOST_AUTO_TEST_CASE (test_operator_and) {
    rime::constant <bool, false> false_;
    rime::constant <bool, true> true_;

    /* Explicit function only */
    // 0 arguments
    RIME_CHECK_EQUAL (rime::and_ (), true_);

    // 1 argument
    RIME_CHECK_EQUAL (rime::and_ (false_), false_);
    RIME_CHECK_EQUAL (rime::and_ (true_), true_);
    RIME_CHECK_EQUAL (rime::and_ (false), false);
    RIME_CHECK_EQUAL (rime::and_ (true), true);

    // 2 arguments
    RIME_CHECK_EQUAL (rime::and_ (false_, false_), false_);
    RIME_CHECK_EQUAL (rime::and_ (false_, false), false_);
    RIME_CHECK_EQUAL (rime::and_ (false, false_), false_);
    RIME_CHECK_EQUAL (rime::and_ (false, false), false);

    RIME_CHECK_EQUAL (rime::and_ (false_, true_), false_);
    RIME_CHECK_EQUAL (rime::and_ (false_, true), false_);
    RIME_CHECK_EQUAL (rime::and_ (false, true_), false);
    RIME_CHECK_EQUAL (rime::and_ (false, true), false);

    RIME_CHECK_EQUAL (rime::and_ (true_, false_), false_);
    RIME_CHECK_EQUAL (rime::and_ (true_, false), false);
    RIME_CHECK_EQUAL (rime::and_ (true, false_), false_);
    RIME_CHECK_EQUAL (rime::and_ (true, false), false);

    RIME_CHECK_EQUAL (rime::and_ (true_, true_), true_);
    RIME_CHECK_EQUAL (rime::and_ (true_, true), true);
    RIME_CHECK_EQUAL (rime::and_ (true, true_), true);
    RIME_CHECK_EQUAL (rime::and_ (true, true), true);

    // 3 arguments: spot check.
    RIME_CHECK_EQUAL (rime::and_ (false_, false_, false_), false_);
    RIME_CHECK_EQUAL (rime::and_ (false_, true_, true_), false_);
    RIME_CHECK_EQUAL (rime::and_ (false_, true_, true), false_);
    RIME_CHECK_EQUAL (rime::and_ (false_, true, true_), false_);
    RIME_CHECK_EQUAL (rime::and_ (true, false_, true_), false_);
    RIME_CHECK_EQUAL (rime::and_ (true, true_, false_), false_);

    RIME_CHECK_EQUAL (rime::and_ (true_, true_, true_), true_);
    RIME_CHECK_EQUAL (rime::and_ (true_, true, true_), true);
    RIME_CHECK_EQUAL (rime::and_ (true_, true, false), false);
    RIME_CHECK_EQUAL (rime::and_ (false, true, true_), false);
    RIME_CHECK_EQUAL (rime::and_ (false, true_, true), false);

    /** operator && **/
    // Rime constants
    RIME_CHECK_EQUAL ((false_ && false_), false_);
    RIME_CHECK_EQUAL ((true_ && false_), false_);
    RIME_CHECK_EQUAL ((false_ && true_), false_);
    RIME_CHECK_EQUAL ((true_ && true_), true_);

    // Run-time and Rime constants: always run-time.
    // (The standard behaviour is not overridden so short-circuiting remains
    // possible.)
    RIME_CHECK_EQUAL ((false && false_), false);
    RIME_CHECK_EQUAL ((true && false_), false);
    RIME_CHECK_EQUAL ((false && true_), false);
    RIME_CHECK_EQUAL ((true && true_), true);
    RIME_CHECK_EQUAL ((false_ && false), false);
    RIME_CHECK_EQUAL ((true_ && false), false);
    RIME_CHECK_EQUAL ((false_ && true), false);
    RIME_CHECK_EQUAL ((true_ && true), true);

    // With MPL constants: known at compile time.
    boost::mpl::bool_ <false> mpl_false;
    boost::mpl::bool_ <true> mpl_true;
    RIME_CHECK_EQUAL ((mpl_false && false_), false_);
    RIME_CHECK_EQUAL ((mpl_true && false_), false_);
    RIME_CHECK_EQUAL ((mpl_false && true_), false_);
    RIME_CHECK_EQUAL ((mpl_true && true_), true_);
    RIME_CHECK_EQUAL ((false_ && mpl_false), false_);
    RIME_CHECK_EQUAL ((true_ && mpl_false), false_);
    RIME_CHECK_EQUAL ((false_ && mpl_true), false_);
    RIME_CHECK_EQUAL ((true_ && mpl_true), true_);

    // MPL constants and run-time values: no change to default behaviour.
    RIME_CHECK_EQUAL ((mpl_false && mpl_false), false);
    RIME_CHECK_EQUAL ((mpl_true && mpl_false), false);
    RIME_CHECK_EQUAL ((mpl_false && mpl_true), false);
    RIME_CHECK_EQUAL ((mpl_true && mpl_true), true);

    RIME_CHECK_EQUAL ((mpl_false && false), false);
    RIME_CHECK_EQUAL ((mpl_true && false), false);
    RIME_CHECK_EQUAL ((mpl_false && true), false);
    RIME_CHECK_EQUAL ((mpl_true && true), true);
    RIME_CHECK_EQUAL ((false && mpl_false), false);
    RIME_CHECK_EQUAL ((true && mpl_false), false);
    RIME_CHECK_EQUAL ((false && mpl_true), false);
    RIME_CHECK_EQUAL ((true && mpl_true), true);
}

BOOST_AUTO_TEST_CASE (test_operator_or) {
    rime::constant <bool, false> false_;
    rime::constant <bool, true> true_;

    /* Explicit function only */
    // 0 arguments
    RIME_CHECK_EQUAL (rime::or_ (), false_);

    // 1 argument
    RIME_CHECK_EQUAL (rime::or_ (false_), false_);
    RIME_CHECK_EQUAL (rime::or_ (true_), true_);
    RIME_CHECK_EQUAL (rime::or_ (false), false);
    RIME_CHECK_EQUAL (rime::or_ (true), true);

    // 2 arguments
    RIME_CHECK_EQUAL (rime::or_ (false_, false_), false_);
    RIME_CHECK_EQUAL (rime::or_ (false_, false), false);
    RIME_CHECK_EQUAL (rime::or_ (false, false_), false);
    RIME_CHECK_EQUAL (rime::or_ (false, false), false);

    RIME_CHECK_EQUAL (rime::or_ (false_, true_), true_);
    RIME_CHECK_EQUAL (rime::or_ (false_, true), true);
    RIME_CHECK_EQUAL (rime::or_ (false, true_), true_);
    RIME_CHECK_EQUAL (rime::or_ (false, true), true);

    RIME_CHECK_EQUAL (rime::or_ (true_, false_), true_);
    RIME_CHECK_EQUAL (rime::or_ (true_, false), true_);
    RIME_CHECK_EQUAL (rime::or_ (true, false_), true);
    RIME_CHECK_EQUAL (rime::or_ (true, false), true);

    RIME_CHECK_EQUAL (rime::or_ (true_, true_), true_);
    RIME_CHECK_EQUAL (rime::or_ (true_, true), true_);
    RIME_CHECK_EQUAL (rime::or_ (true, true_), true_);
    RIME_CHECK_EQUAL (rime::or_ (true, true), true);

    // 3 arguments: spot check.
    RIME_CHECK_EQUAL (rime::or_ (true_, true_, true_), true_);
    RIME_CHECK_EQUAL (rime::or_ (true_, false_, false_), true_);
    RIME_CHECK_EQUAL (rime::or_ (true_, false_, false), true_);
    RIME_CHECK_EQUAL (rime::or_ (true_, false, false_), true_);
    RIME_CHECK_EQUAL (rime::or_ (false, true_, false_), true_);
    RIME_CHECK_EQUAL (rime::or_ (false, false_, true_), true_);

    RIME_CHECK_EQUAL (rime::or_ (false_, false_, false_), false_);
    RIME_CHECK_EQUAL (rime::or_ (false_, false, false_), false);
    RIME_CHECK_EQUAL (rime::or_ (false_, false, true), true);
    RIME_CHECK_EQUAL (rime::or_ (true, false, false_), true);
    RIME_CHECK_EQUAL (rime::or_ (true, false_, false), true);

    /** operator || **/
    // Rime constants
    RIME_CHECK_EQUAL ((false_ || false_), false_);
    RIME_CHECK_EQUAL ((true_ || false_), true_);
    RIME_CHECK_EQUAL ((false_ || true_), true_);
    RIME_CHECK_EQUAL ((true_ || true_), true_);

    // Run-time and Rime constants: always run-time.
    // (The standard behaviour is not overridden so short-circuiting remains
    // possible.)
    RIME_CHECK_EQUAL ((false || false_), false);
    RIME_CHECK_EQUAL ((true || false_), true);
    RIME_CHECK_EQUAL ((false || true_), true);
    RIME_CHECK_EQUAL ((true || true_), true);
    RIME_CHECK_EQUAL ((false_ || false), false);
    RIME_CHECK_EQUAL ((true_ || false), true);
    RIME_CHECK_EQUAL ((false_ || true), true);
    RIME_CHECK_EQUAL ((true_ || true), true);

    // With MPL constants: known at compile time.
    boost::mpl::bool_ <false> mpl_false;
    boost::mpl::bool_ <true> mpl_true;
    RIME_CHECK_EQUAL ((mpl_false || false_), false_);
    RIME_CHECK_EQUAL ((mpl_true || false_), true_);
    RIME_CHECK_EQUAL ((mpl_false || true_), true_);
    RIME_CHECK_EQUAL ((mpl_true || true_), true_);
    RIME_CHECK_EQUAL ((false_ || mpl_false), false_);
    RIME_CHECK_EQUAL ((true_ || mpl_false), true_);
    RIME_CHECK_EQUAL ((false_ || mpl_true), true_);
    RIME_CHECK_EQUAL ((true_ || mpl_true), true_);

    // MPL constants and run-time values: no change to default behaviour.
    RIME_CHECK_EQUAL ((mpl_false || mpl_false), false);
    RIME_CHECK_EQUAL ((mpl_true || mpl_false), true);
    RIME_CHECK_EQUAL ((mpl_false || mpl_true), true);
    RIME_CHECK_EQUAL ((mpl_true || mpl_true), true);

    RIME_CHECK_EQUAL ((mpl_false || false), false);
    RIME_CHECK_EQUAL ((mpl_true || false), true);
    RIME_CHECK_EQUAL ((mpl_false || true), true);
    RIME_CHECK_EQUAL ((mpl_true || true), true);
    RIME_CHECK_EQUAL ((false || mpl_false), false);
    RIME_CHECK_EQUAL ((true || mpl_false), true);
    RIME_CHECK_EQUAL ((false || mpl_true), true);
    RIME_CHECK_EQUAL ((true || mpl_true), true);
}

// Test cases where operators should not do anything.
BOOST_AUTO_TEST_CASE (test_operator_interaction) {
    {
        char c[2] = "g";
        char * p = &c[0];
        char * q = p + rime::int_ <1>();
        BOOST_CHECK_EQUAL (p + 1, q);
    }
}

BOOST_AUTO_TEST_SUITE_END()

