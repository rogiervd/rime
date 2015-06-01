/*
Copyright 2011, 2012, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_variant_operator
#include "utility/test/boost_unit_test.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "utility/test/tracked.hpp"
#include <string>

// GCC rightfully complains about comparisons between signed and unsigned
// integers when testing binary operators.
// However, all we require here is for the behaviour to be the same between
// the variant and non-variant versions.
// (This includes warnings, but that is hard to test for.)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

#include "rime/variant.hpp"

#include <iostream>
#include <array>

using std::is_same;

BOOST_AUTO_TEST_SUITE(test_rime_variant_operator)

struct tracked_assignable {
    tracked_assignable & operator = (utility::tracked<> const &)
    { return *this; }
};

BOOST_AUTO_TEST_CASE (test_rime_variant_assign) {
    typedef rime::variant <int, float &, double> variant;
    {
        variant vi (4);
        float f = 5.f;
        variant vfr (f);
        variant vd (6.);

        // Sanity checks
        BOOST_CHECK_EQUAL (vi.which(), 0u);
        BOOST_CHECK_EQUAL (rime::get <int> (vi), 4);
        BOOST_CHECK_EQUAL (vfr.which(), 1u);
        BOOST_CHECK_EQUAL (&rime::get <float &> (vfr), &f);
        BOOST_CHECK_EQUAL (vd.which(), 2u);
        BOOST_CHECK_EQUAL (rime::get <double> (vd), 6.);

        /* Assignment. */
        // The type (which()) does not change; the contents do change.
        vi = 7;
        BOOST_CHECK_EQUAL (vi.which(), 0u);
        BOOST_CHECK_EQUAL (rime::get <int> (vi), 7);

        vfr = 8;
        BOOST_CHECK_EQUAL (vfr.which(), 1u);
        BOOST_CHECK_EQUAL (f, 8);
        BOOST_CHECK_EQUAL (&rime::get <float &> (vfr), &f);

        // This should cause the variant to contain a double with value 9.0,
        // not an int!
        vd = 9;
        BOOST_CHECK_EQUAL (vd.which(), 2u);
        BOOST_CHECK_EQUAL (rime::get <double> (vd), 9.);

        /* Assignment from itself */
        vi = vi;
        BOOST_CHECK_EQUAL (vi.which(), 0u);
        BOOST_CHECK_EQUAL (rime::get <int> (vi), 7);

        vfr = vfr;
        BOOST_CHECK_EQUAL (vfr.which(), 1u);
        BOOST_CHECK_EQUAL (f, 8);
        BOOST_CHECK_EQUAL (&rime::get <float &> (vfr), &f);

        vd = vd;
        BOOST_CHECK_EQUAL (vd.which(), 2u);
        BOOST_CHECK_EQUAL (rime::get <double> (vd), 9.);

        /* Assignment from another variant */
        typedef rime::variant <int, unsigned int, char> other_variant;
        other_variant v2 ('r');
        other_variant v3 (10u);

        // From char 'r'
        vi = v2;
        BOOST_CHECK_EQUAL (vi.which(), 0u);
        BOOST_CHECK_EQUAL (rime::get <int> (vi), int ('r'));

        vfr = v2;
        BOOST_CHECK_EQUAL (vfr.which(), 1u);
        BOOST_CHECK_EQUAL (f, int ('r'));
        BOOST_CHECK_EQUAL (&rime::get <float &> (vfr), &f);

        vd = v2;
        BOOST_CHECK_EQUAL (vd.which(), 2u);
        BOOST_CHECK_EQUAL (rime::get <double> (vd), int ('r'));

        // From unsigned 10u
        vi = v3;
        BOOST_CHECK_EQUAL (vi.which(), 0u);
        BOOST_CHECK_EQUAL (rime::get <int> (vi), 10);

        vfr = v3;
        BOOST_CHECK_EQUAL (vfr.which(), 1u);
        BOOST_CHECK_EQUAL (f, 10);
        BOOST_CHECK_EQUAL (&rime::get <float &> (vfr), &f);

        vd = v3;
        BOOST_CHECK_EQUAL (vd.which(), 2u);
        BOOST_CHECK_EQUAL (rime::get <double> (vd), 10);
    }

    // Assign with objects and moving
    {
        utility::tracked_registry registry;
        using utility::tracked;
        {
            tracked<> c (registry);
            rime::variant <tracked<>, tracked_assignable> v =
                tracked<> (registry);

            // Sanity
            BOOST_CHECK_EQUAL (registry.value_construct_count(), 2);
            BOOST_CHECK_EQUAL (registry.move_count(), 1);
            BOOST_CHECK_EQUAL (registry.copy_assign_count(), 0);
            BOOST_CHECK_EQUAL (registry.move_assign_count(), 0);

            // Copy assignment
            tracked<> c2 (registry);
            BOOST_CHECK_EQUAL (registry.value_construct_count(), 3);
            v = c2;
            BOOST_CHECK_EQUAL (registry.copy_assign_count(), 1);
            BOOST_CHECK_EQUAL (registry.move_assign_count(), 0);

            // Move assignment
            v = tracked<> (registry);
            BOOST_CHECK_EQUAL (registry.value_construct_count(), 4);
            BOOST_CHECK_EQUAL (registry.move_count(), 1);
            BOOST_CHECK_EQUAL (registry.copy_assign_count(), 1);
            BOOST_CHECK_EQUAL (registry.move_assign_count(), 1);

            // From other variant type.
            // All possible content types for v2 must be convertible to
            // tracked.
            rime::variant <tracked<>, tracked<> &> v2 = tracked<> (registry);
            BOOST_CHECK_EQUAL (registry.value_construct_count(), 5);
            BOOST_CHECK_EQUAL (registry.move_count(), 2);
            v = v2;
            BOOST_CHECK_EQUAL (registry.copy_assign_count(), 2);
        }
        BOOST_CHECK (registry.consistent());
    }
}

BOOST_AUTO_TEST_CASE (test_variant_operation_assign) {
    int i = 5;

    rime::variant <short, int &> four (short (4));
    rime::variant <short, int &> five (i);
    rime::variant <short, int &, double> two_half (2.5);

    // This uses the knowledge that the assignment operators are defined
    // very similarly.
    {
        auto result = four;
        result += 4;
        BOOST_CHECK (result.contains <short>());
        BOOST_CHECK_EQUAL (rime::get <short> (result), 8);

        result -= five;
        BOOST_CHECK (result.contains <short>());
        BOOST_CHECK_EQUAL (rime::get <short> (result), 3);
    }
    {
        auto result = five;
        // This will change i, because "result" contains a reference to it
        result *= four;
        BOOST_CHECK_EQUAL (i, 20);
        // Reset.
        i = 5;
    }
    {
        auto result = two_half;
        BOOST_CHECK ((result /= 5).contains <double>());
        BOOST_CHECK_EQUAL (rime::get <double> (result), .5);
        result *= two_half;
        BOOST_CHECK (result.contains <double>());
        BOOST_CHECK_EQUAL (rime::get <double> (result), 1.25);
    }
    {
        auto result = five;
        BOOST_CHECK_EQUAL (rime::get <int &> (result %= four), 1);
        BOOST_CHECK_EQUAL (rime::get <int &> (result), 1);
    }
    {
        // 0b0011 & 0b1010
        rime::variant <short, int &> result = 3;
        result &= 10;
        BOOST_CHECK_EQUAL (rime::get <short> (result), 2);
    }
    {
        // 0b0011 | 0b1010
        rime::variant <short, int &> result = 3;
        result |= 10;
        BOOST_CHECK_EQUAL (rime::get <short> (result), 11);
    }
    {
        // 0b0011 ^ 0b1010
        rime::variant <short, int &> result = 3;
        result ^= 10;
        BOOST_CHECK_EQUAL (rime::get <short> (result), 9);
    }
    {
        five = 5;
        four = 4;
        auto result = four;
        result <<= five;
        BOOST_CHECK_EQUAL (rime::get <short> (result), 4 * 32);
    }
    {
        auto result = four;
        result >>= 2;
        BOOST_CHECK_EQUAL (rime::get <short> (result), 1);
    }
}

BOOST_AUTO_TEST_CASE (test_rime_make_variant_over) {
    using rime::variant;
    using rime::make_variant_over;

    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <meta::vector <int>>::type, int>));

    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <meta::vector <int, int>>::type, int>));

    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <meta::vector <const int, int>>::type,
        variant <const int, int>>));

    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <meta::vector <double, int, const int, int>>::type,
        variant <double, int, const int>>));

    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <meta::vector <int, rime::int_ <10>>>::type,
        int>));

    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <meta::vector <bool, rime::bool_ <false>>>::type,
        bool>));

    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <meta::vector <bool, rime::true_type>>::type,
        bool>));

    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <
            meta::vector <rime::int_ <1>, rime::int_ <10>>>::type,
        int>));

    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <
            meta::vector <rime::true_type, rime::bool_ <false>>>::type,
        bool>));

    // Constants with the same value type and value,
    // but different apparent type.
    BOOST_MPL_ASSERT ((rime::same_constant <
        rime::constant <int, 10>, rime::int_ <10>>));

    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <
            meta::vector <rime::constant <int, 10>, rime::int_ <10>>>::type,
        rime::constant <int, 10>>));

    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <
            meta::vector <rime::false_type,
                rime::constant <bool, false>>>::type,
        rime::constant <bool, false>>));

    // Don't turn two constants with the same type into their value type.
    BOOST_MPL_ASSERT ((std::is_same <
        make_variant_over <
            meta::vector <rime::true_type, rime::true_type>>::type,
        rime::true_type>));
}

template <typename Argument>
    void check_same_type_equal (Argument const & left, Argument const & right)
{
    BOOST_CHECK_EQUAL (left, right);
}

template <typename ... Types, typename Argument>
    void check_same_type_equal (
        rime::variant <Types...> const & left, Argument && right)
{
    BOOST_CHECK (left.template contains <Argument>());
    if (left.template contains <Argument>()) {
        BOOST_CHECK_EQUAL (rime::get <Argument> (left), right);
    }
}

template <typename ... Types, typename Argument>
    void check_contains (
        rime::variant <Types...> const & left, Argument const & right)
{
    BOOST_CHECK (left.template contains <Argument>());
    if (left.template contains <Argument>()) {
        BOOST_CHECK_EQUAL (rime::get <Argument> (left), right);
    }
}

BOOST_AUTO_TEST_CASE (test_variant_operator_unary) {
    unsigned u = 4u;
    int i = 5;
    long l = -6l;

    // Operators that can be tested on numeric values.
    {
        typedef rime::variant <unsigned, int, long> variant;

        variant vu (u);
        variant vi (i);
        variant vl (l);

        check_contains (vu, u);
        check_contains (vi, i);
        check_contains (vl, l);

        check_same_type_equal (-vu, -u);
        check_same_type_equal (-vi, -i);
        check_same_type_equal (-vl, -l);

        check_same_type_equal (+vu, +u);
        check_same_type_equal (+vi, +i);
        check_same_type_equal (+vl, +l);

        check_same_type_equal (!vu, !u);
        check_same_type_equal (!vi, !i);
        check_same_type_equal (!vl, !l);

        check_same_type_equal (~vu, ~u);
        check_same_type_equal (~vi, ~i);
        check_same_type_equal (~vl, ~l);

        check_same_type_equal (++vu, ++u);
        check_contains (vu, u);
        check_same_type_equal (++vi, ++i);
        check_contains (vi, i);
        check_same_type_equal (++vl, ++l);
        check_contains (vl, l);

        check_same_type_equal (vu++, u++);
        check_contains (vu, u);
        check_same_type_equal (vi++, i++);
        check_contains (vi, i);
        check_same_type_equal (vl++, l++);
        check_contains (vl, l);

        check_same_type_equal (--vu, --u);
        check_contains (vu, u);
        check_same_type_equal (--vi, --i);
        check_contains (vi, i);
        check_same_type_equal (--vl, --l);
        check_contains (vl, l);

        check_same_type_equal (vu--, u--);
        check_contains (vu, u);
        check_same_type_equal (vi--, i--);
        check_contains (vi, i);
        check_same_type_equal (vl--, l--);
        check_contains (vl, l);
    }

    // Operators that can be tested on pointer values.
    {
        unsigned * pu = &u;
        std::shared_ptr <int> pi (new int (5));

        typedef rime::variant <unsigned *,
            std::shared_ptr <int>, std::unique_ptr <long>> variant;

        variant vpu (pu);
        variant vpi (pi);

        check_same_type_equal (*vpu, *pu);
        check_same_type_equal (*vpi, *pi);
    }
}

template <typename Left, typename Right>
    void check_binary_operators (Left const & left, Right const & right)
{
    typedef rime::variant <unsigned, int, long> variant;
    variant v_left (left);
    variant v_right (right);

    check_same_type_equal (v_left + v_right, left + right);
    check_same_type_equal (v_left - v_right, left - right);
    check_same_type_equal (v_left * v_right, left * right);
    // Don't divide by zero.
    if (right) {
        check_same_type_equal (v_left / v_right, left / right);
        check_same_type_equal (v_left % v_right, left % right);
    }

    check_same_type_equal (v_left == v_right, left == right);
    check_same_type_equal (v_left != v_right, left != right);
    check_same_type_equal (v_left > v_right, left > right);
    check_same_type_equal (v_left < v_right, left < right);
    check_same_type_equal (v_left >= v_right, left >= right);
    check_same_type_equal (v_left <= v_right, left <= right);

    check_same_type_equal (v_left & v_right, left & right);
    check_same_type_equal (v_left | v_right, left | right);
    check_same_type_equal (v_left ^ v_right, left ^ right);
    check_same_type_equal (v_left << v_right, left << right);
    check_same_type_equal (v_left >> v_right, left >> right);
}

BOOST_AUTO_TEST_CASE (test_variant_operator_binary) {
    check_binary_operators (unsigned (4), unsigned (5));
    check_binary_operators (unsigned (4), int (6));
    check_binary_operators (unsigned (4), long (7));
    check_binary_operators (int (87), unsigned (5));
    check_binary_operators (int (87), int (6));
    check_binary_operators (int (87), long (7));
    check_binary_operators (long (-23), unsigned (5));
    check_binary_operators (long (-23), int (6));
    check_binary_operators (long (-23), long (7));

    check_binary_operators (unsigned (4), unsigned (0));
    check_binary_operators (int (0), long (7));
    check_binary_operators (long (-23), int (0));
}

BOOST_AUTO_TEST_CASE (test_variant_operator_subscript) {
    typedef rime::variant <std::array <int, 3>, short *> variant;
    typedef rime::variant <std::array <int, 3>, short *, long const *> variant2;

    std::array <int, 3> i;
    i [0] = 3; i [1] = 4; i [2] = 5;
    short s[] = {7, 8, 9, 10, 11};
    long l[] = {10, 11};

    variant vi (i);
    variant vs (s);
    variant2 vlc (l);

    variant2 const vic (i);
    variant2 const vsc (s);
    variant2 const vlcc (l);

    BOOST_CHECK_EQUAL (rime::get <int &> (vi [0]), 3);
    BOOST_CHECK_EQUAL (rime::get <int &> (vi [2]), 5);
    // The array is contained so that constness propagates.
    BOOST_CHECK_EQUAL (rime::get <int const &> (vic [0]), 3);
    BOOST_CHECK_EQUAL (rime::get <int const &> (vic [2]), 5);

    BOOST_CHECK_EQUAL (rime::get <short &> (vs [0]), 7);
    BOOST_CHECK_EQUAL (rime::get <short &> (vs [3]), 10);
    // The pointer in vsc is const, but the pointees are not.
    BOOST_CHECK_EQUAL (rime::get <short &> (vsc [0]), 7);
    BOOST_CHECK_EQUAL (rime::get <short &> (vsc [3]), 10);

    BOOST_CHECK_EQUAL (rime::get <long const &> (vlc [0]), 10);
    BOOST_CHECK_EQUAL (rime::get <long const &> (vlc [1]), 11);
    // The pointee is always const.
    BOOST_CHECK_EQUAL (rime::get <long const &> (vlcc [0]), 10);
    BOOST_CHECK_EQUAL (rime::get <long const &> (vlcc [1]), 11);

    vs [1] = 56;
    BOOST_CHECK_EQUAL (s [1], 56);
}

BOOST_AUTO_TEST_SUITE_END()
