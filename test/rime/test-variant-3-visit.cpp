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

#define BOOST_TEST_MODULE test_rime_variant_visit
#include "utility/test/boost_unit_test.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include <string>

// GCC rightfully complains about comparisons between signed and unsigned
// integers when testing binary operators.
// However, all we require here is for the behaviour to be the same between
// the variant and non-variant versions.
// (This includes warnings, but that is hard to test for.)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

#include "rime/variant.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_variant_visit)

struct plus {
public:
    template <typename Argument>
        Argument operator() (Argument argument) const
    { return argument; }

    template <typename First, typename ... Rest>
        auto operator() (First first, Rest ... rest) const
    -> decltype (first + (std::declval <plus const &>()) (rest ...))
    { return first + (*this) (rest ...); }
};

struct times {
public:
    template <typename Argument>
        Argument operator() (Argument argument) const
    { return argument; }

    template <typename First, typename ... Rest>
        auto operator() (First first, Rest ... rest) const
    -> decltype (first * (std::declval <plus const &>()) (rest ...))
    { return first * (*this) (rest ...); }
};

struct add_one_visitor {
    std::type_info const * & argument_type;
public:
    add_one_visitor (std::type_info const * & argument_type)
    : argument_type (argument_type) {}

    template <class Argument>
        void operator() (Argument && argument) const
    {
        argument_type = &typeid (Argument);
        argument += 1;
    }
};

struct plus_assign {
public:
    template <typename Left, typename Right>
        Left && operator() (Left && left, Right && right) const
    {
        left += right;
        return std::forward <Left> (left);
    }

    template <typename Argument>
        Argument && operator() (Argument && left) const
    {
        return left;
    }

    void operator() () const {}
};

struct plus_assign_void {
    plus operation;

    void operator() () const {}

    template <typename Argument>
        void operator() (Argument) const {}

    template <typename Target, typename ... Arguments>
        void operator() (Target & target, Arguments ... arguments) const
    {
        target = operation (target, arguments ...);
    }
};

struct count_arguments {
    template <typename First, typename ... Arguments>
    int operator() (First, Arguments... arguments) const
    { return 1 + (*this) (arguments...); }

    template <typename Argument> int operator() (Argument) const { return 1; }

    void operator() () const {}
};

struct is_const {
    is_const() {}

    bool operator() () const { return true; }
    bool operator() () { return false; }
};

BOOST_AUTO_TEST_CASE (test_rime_variant_visit) {
    typedef rime::variant <int, float, double &> variant;
    typedef rime::variant <int, float, double &, void> variant_void;
    std::type_info const * type;
    add_one_visitor add_one (type);

    {
        variant v (5);
        rime::visit (add_one) (v);
        BOOST_CHECK_EQUAL (type, &typeid (int &));
        BOOST_CHECK_EQUAL (rime::get <int> (v), 6);
    }

    {
        variant v (5.5f);
        rime::visit (add_one) (v);
        BOOST_CHECK_EQUAL (type, &typeid (float &));
        BOOST_CHECK_EQUAL (rime::get <float> (v), 6.5f);
    }


    {
        double d = 1.1;
        variant v (d);
        rime::visit (add_one) (v);
        BOOST_CHECK_EQUAL (type, &typeid (double &));
        BOOST_CHECK_EQUAL (rime::get <double &> (v), 2.1);
        BOOST_CHECK_EQUAL (&rime::get <double &> (v), &d);
    }

    {
        variant v1 (4);
        variant v2 (5);
        rime::visit (plus_assign()) (v1, v2);
        BOOST_CHECK_EQUAL (rime::get <int> (v1), 9);
    }
    {
        variant_void v1 (4);
        variant_void v2 (5);
        rime::variant <int &, float &, double &, void> result =
            rime::visit (plus_assign()) (v1, v2);
        BOOST_CHECK_EQUAL (rime::get <int &> (result), 9);
        BOOST_CHECK_EQUAL (rime::get <int> (v1), 9);
        BOOST_CHECK_EQUAL (
            &rime::get <int &> (result), &rime::get <int> (v1));

        rime::variant <int &, float &, double &, void> void_;
        auto result2 = rime::visit (plus_assign()) (v1, void_);
        BOOST_CHECK_EQUAL (rime::get <int &> (result2), 9);
        BOOST_CHECK_EQUAL (rime::get <int> (v1), 9);
    }
    {
        double d = 6.7;
        variant v1 (4);
        variant v2 (d);
        rime::visit (plus_assign()) (v1, v2);
        BOOST_CHECK_EQUAL (rime::get <int> (v1), 10);
    }

    {
        double d = 6.7;
        variant v1 (4);
        variant v2 (d);
        auto result = rime::visit (plus()) (v1, v2);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (result), rime::variant <int, float, double> >));
        BOOST_CHECK_EQUAL (rime::get <double> (result), 10.7);
    }
    {
        variant v1 (4.f);
        variant v2 (3.f);
        auto result = rime::visit (plus()) (v1, v2);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (result), rime::variant <int, float, double> >));
        BOOST_CHECK_EQUAL (rime::get <float> (result), 7.f);
    }
    {
        variant v1 (4);
        variant v2 (3.f);
        auto result = rime::visit (plus()) (v1, v2);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (result), rime::variant <int, float, double> >));
        BOOST_CHECK_EQUAL (rime::get <float> (result), 7.f);
    }
    {
        variant v1 (-3);
        variant v2 (-6);
        auto result = rime::visit (plus()) (v1, v2);
        BOOST_MPL_ASSERT ((std::is_same <
            decltype (result), rime::variant <int, float, double> >));
        BOOST_CHECK_EQUAL (rime::get <int> (result), -9);
    }
    {
        rime::variant <int, void> void_;
        rime::variant <int, void> int_ (5);
        BOOST_CHECK_EQUAL (rime::get <int> (
            rime::visit (count_arguments()) (int_, int_, int_)), 3);
        BOOST_CHECK_EQUAL (rime::get <int> (
            rime::visit (count_arguments()) (void_, int_, int_)), 2);
        BOOST_CHECK_EQUAL (rime::get <int> (
            rime::visit (count_arguments()) (int_, void_, int_)), 2);
        BOOST_CHECK_EQUAL (rime::get <int> (
            rime::visit (count_arguments()) (int_, int_, void_)), 2);
        BOOST_CHECK_EQUAL (rime::get <int> (
            rime::visit (count_arguments()) (int_, void_, void_)), 1);
        BOOST_CHECK_EQUAL (rime::get <int> (
            rime::visit (count_arguments()) (void_, int_, void_)), 1);
        BOOST_CHECK_EQUAL (rime::get <int> (
            rime::visit (count_arguments()) (void_, void_, int_)), 1);
        // No argument: returns void
        BOOST_CHECK ((rime::visit (count_arguments()) (void_, void_, void_))
            .contains <void>());
    }
}

struct non_copyable {
    non_copyable() {}
    non_copyable (int) {}

    non_copyable (non_copyable const &) = delete;
    non_copyable (non_copyable &&) = delete;
};

struct return_one_from_last : non_copyable {
    template <typename Type1, typename Type2, typename Type3>
        typename std::add_pointer <Type2>::type
            operator() (Type1 &&, Type2 && result, Type3 &&) const
    { return &result; }
};

BOOST_AUTO_TEST_CASE (test_rime_variant_call) {
    {
        rime::variant <plus> v_plus ((plus()));
        BOOST_CHECK_EQUAL (v_plus (4.5, 4.5, -6), 3);
    }

    {
        rime::variant <plus, plus_assign_void, count_arguments>
            v_plus ((plus()));
        rime::variant <plus, plus_assign_void, count_arguments>
            v_plus_assign_void ((plus_assign_void()));
        rime::variant <plus, plus_assign_void, count_arguments>
            v_count_arguments ((count_arguments()));

        int i = 5;
        int check = rime::get <int> (v_plus (i));
        BOOST_CHECK_EQUAL (check, 5);
        BOOST_CHECK (v_plus_assign_void (i, 3).contains <void>());
        BOOST_CHECK_EQUAL (i, 8);
        check = rime::get <int> (v_count_arguments (i, 3));
        BOOST_CHECK_EQUAL (check, 2);
        BOOST_CHECK_EQUAL (i, 8);
        check = rime::get <int> (v_count_arguments (i, 3, 7, i, 6));
        BOOST_CHECK_EQUAL (check, 5);
    }

    {
        is_const c1;
        is_const const c2;
        rime::variant <is_const, is_const const> v_mutable ((c1));
        rime::variant <is_const, is_const const> v_const ((c2));
        rime::variant <is_const, is_const const> const v_const_2 ((c1));
        BOOST_CHECK (!v_mutable());
        BOOST_CHECK (v_const());
        BOOST_CHECK (v_const_2());
    }

    // Test that there is no gratuitous copying.
    {
        return_one_from_last f;
        non_copyable n;
        non_copyable * n2 = rime::visit (f) (4, std::move (n), 'a');
        BOOST_CHECK (&n == n2);

        n2 = rime::visit (f) (
            rime::variant <int, bool> (3), std::move (n), 'a');
        BOOST_CHECK (&n == n2);

        n2 = rime::visit (f) (
            rime::variant <int, bool> (3), n, 'a');
        BOOST_CHECK (&n == n2);
    }
}

double take_two_arguments (int a, float b)
{ return double (a) + b; }

/**
Try to make compiler output on errors understandable.
*/
BOOST_AUTO_TEST_CASE (test_rime_variant_error_messages) {
/*
    // A variant containing a variant.
    rime::variant <int, rime::variant <int, double>> nested (4);
*/
/*
    // A type occurring more than once.
    rime::variant <int, double, int> duplicate_type (2.3);
*/
/*
    // Ambiguous conversion
    rime::variant <int, float, double &> v1 (6.7);
*/
/*
    // Invalid conversion
    std::string s;
    rime::variant <int, float, double &> v2 (s);
*/
/*
    // Invalid conversion from other variant.
    rime::variant <int, float, std::string> v3 (4);
    rime::variant <int, float, double &> v4 (v3);
*/
    rime::variant <int, float> v5 (5.0f);
/*
    // Impossible get
    rime::get <double> (v5);
*/
/*
    // Impossible get with void.
    rime::get <void> (v5);
*/
/*
    // Impossible get from pointer.
    rime::get <double> (&v5);
*/
/*
    // Attempt to default-construct a variant that cannot contain void.
    auto v6 = rime::variant <int, float>();
*/
/*
    // Attempt to assign to a variant that cannot contain void.
    auto void_variant = rime::variant <int, float, void>();
    rime::variant <double> non_void_variant (void_variant);
*/
/*
    // Call a function with the wrong number of arguments.
    double d1 = rime::visit (take_two_arguments) (
        rime::variant <int, char> ('a'), 4.f, rime::variant <int, float> (4.f));
    double d2 = rime::visit (take_two_arguments) (
        rime::variant <int, char> ('a'), rime::variant <void, float> (4.f));
*/
}

BOOST_AUTO_TEST_SUITE_END()
