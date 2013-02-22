/*
Copyright 2011, 2012 Rogier van Dalen.

This file is part of Rogier van Dalen's Range library for C++.

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
#include "../boost_unit_test.hpp"

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

#include <iostream>
#include <array>

struct convertee {};
struct converted {
    converted() {}
    converted (convertee) {}
};

BOOST_MPL_ASSERT ((std::is_convertible <convertee &&, converted>));

int const && get_int_const() {
    static int i = 5;
    return static_cast <int const &&> (i);
}

using std::is_same;

typedef rime::variant <
    /* 0 */ int, /* 1 */ float, /* 2 */ int &,
    /* 3 */ int const, /* 4 */ int const &,
    /* 5 */ std::string, /* 6 */ void *, /* 7 */ char *,
    /* 8 */ converted,
    /* 9 */ void
> variant;

typedef rime::variant <
    /* 0 */ int, /* 1 */ float, /* 2 */ int &,
    /* 3 */ int const, /* 4 */ int const &
> other_variant;

template <typename T> variant make_variant (T && o)
{ return variant (std::forward <T> (o)); }

template <typename T> variant const make_const_variant (T && o)
{ return variant (std::forward <T> (o)); }

template <typename T> other_variant make_other_variant (T && o)
{ return other_variant (std::forward <T> (o)); }

template <typename T> other_variant const make_const_other_variant (T && o)
{ return other_variant (std::forward <T> (o)); }

// Make void versions
variant make_variant() { return variant(); }
variant const make_const_variant() { return variant(); }

BOOST_AUTO_TEST_SUITE(test_rime_variant)

BOOST_AUTO_TEST_CASE (test_rime_variant_get_types) {
    // return types for "get"
    {
        variant v (6);
        BOOST_MPL_ASSERT ((is_same <int &,
            decltype (rime::get <int> (v))>));
        BOOST_MPL_ASSERT ((is_same <float &,
            decltype (rime::get <float> (v))>));
        BOOST_MPL_ASSERT ((is_same <int &,
            decltype (rime::get <int &> (v))>));
        BOOST_MPL_ASSERT ((is_same <int const &,
            decltype (rime::get <int const> (v))>));
        BOOST_MPL_ASSERT ((is_same <int const &,
            decltype (rime::get <int const &> (v))>));
        BOOST_MPL_ASSERT ((is_same <std::string &,
            decltype (rime::get <std::string> (v))>));
        BOOST_MPL_ASSERT ((is_same <void * &,
            decltype (rime::get <void *> (v))>));
        BOOST_MPL_ASSERT ((is_same <converted &,
            decltype (rime::get <converted> (v))>));
        BOOST_MPL_ASSERT ((is_same <void,
            decltype (rime::get <void> (v))>));

        BOOST_MPL_ASSERT ((is_same <int &&,
            decltype (rime::get <int> (make_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <float &&,
            decltype (rime::get <float> (make_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <int &,
            decltype (rime::get <int &> (make_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <int const &&,
            decltype (rime::get <int const> (make_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <int const &,
            decltype (rime::get <int const &> (make_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <std::string &&,
            decltype (rime::get <std::string> (make_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <void * &&,
            decltype (rime::get <void *> (make_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <converted &&,
            decltype (rime::get <converted> (make_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <void,
            decltype (rime::get <void> (make_variant (6)))>));

        BOOST_MPL_ASSERT ((is_same <int *,
            decltype (rime::get <int> (&v))>));
        BOOST_MPL_ASSERT ((is_same <float *,
            decltype (rime::get <float> (&v))>));
/*        BOOST_MPL_ASSERT ((is_same <fails,
            decltype (rime::get <int &> (&v))>));*/
        BOOST_MPL_ASSERT ((is_same <int const *,
            decltype (rime::get <int const> (&v))>));
/*        BOOST_MPL_ASSERT ((is_same <fails,
            decltype (rime::get <int const &> (&v))>));*/
        BOOST_MPL_ASSERT ((is_same <std::string *,
            decltype (rime::get <std::string> (&v))>));
        BOOST_MPL_ASSERT ((is_same <void **,
            decltype (rime::get <void *> (&v))>));
        BOOST_MPL_ASSERT ((is_same <converted *,
            decltype (rime::get <converted> (&v))>));
        BOOST_MPL_ASSERT ((is_same <void *,
            decltype (rime::get <void> (&v))>));

        variant const vc (6);
        BOOST_MPL_ASSERT ((is_same <int const &,
            decltype (rime::get <int> (vc))>));
        BOOST_MPL_ASSERT ((is_same <float const &,
            decltype (rime::get <float> (vc))>));
        BOOST_MPL_ASSERT ((is_same <int &,
            decltype (rime::get <int &> (vc))>));
        BOOST_MPL_ASSERT ((is_same <int const &,
            decltype (rime::get <int const> (vc))>));
        BOOST_MPL_ASSERT ((is_same <int const &,
            decltype (rime::get <int const &> (vc))>));
        BOOST_MPL_ASSERT ((is_same <std::string const &,
            decltype (rime::get <std::string> (vc))>));
        BOOST_MPL_ASSERT ((is_same <void * const &,
            decltype (rime::get <void *> (vc))>));
        BOOST_MPL_ASSERT ((is_same <converted const &,
            decltype (rime::get <converted> (vc))>));
        BOOST_MPL_ASSERT ((is_same <void,
            decltype (rime::get <void> (vc))>));

        BOOST_MPL_ASSERT ((is_same <int const &&,
            decltype (rime::get <int> (make_const_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <float const &&,
            decltype (rime::get <float> (make_const_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <int &,
            decltype (rime::get <int &> (make_const_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <int const &&,
            decltype (rime::get <int const> (make_const_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <int const &,
            decltype (rime::get <int const &> (make_const_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <std::string const &&,
            decltype (rime::get <std::string> (make_const_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <void * const &&,
            decltype (rime::get <void *> (make_const_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <converted const &&,
            decltype (rime::get <converted> (make_const_variant (6)))>));
        BOOST_MPL_ASSERT ((is_same <void,
            decltype (rime::get <void> (make_const_variant (6)))>));

        BOOST_MPL_ASSERT ((is_same <int const *,
            decltype (rime::get <int> (&vc))>));
        BOOST_MPL_ASSERT ((is_same <float const *,
            decltype (rime::get <float> (&vc))>));
        /*BOOST_MPL_ASSERT ((is_same <fails,
            decltype (rime::get <int &> (&vc))>));*/
        BOOST_MPL_ASSERT ((is_same <int const *,
            decltype (rime::get <int const> (&vc))>));
        /*BOOST_MPL_ASSERT ((is_same <
            decltype (rime::get <int const &> (&vc))>));*/
        BOOST_MPL_ASSERT ((is_same <std::string const *,
            decltype (rime::get <std::string> (&vc))>));
        BOOST_MPL_ASSERT ((is_same <void * const *,
            decltype (rime::get <void *> (&vc))>));
        BOOST_MPL_ASSERT ((is_same <converted const *,
            decltype (rime::get <converted> (&vc))>));
        BOOST_MPL_ASSERT ((is_same <void const *,
            decltype (rime::get <void> (&vc))>));
    }

    // index_of
    static_assert (variant::index_of <int>::value == 0, "");
    static_assert (variant::index_of <float>::value == 1, "");
    static_assert (variant::index_of <int &>::value == 2, "");
    static_assert (variant::index_of <int const>::value == 3, "");
    static_assert (variant::index_of <int const &>::value == 4, "");
    static_assert (variant::index_of <std::string>::value == 5, "");
    static_assert (variant::index_of <void *>::value == 6, "");
    static_assert (variant::index_of <char *>::value == 7, "");
    static_assert (variant::index_of <converted>::value == 8, "");
    static_assert (variant::index_of <void>::value == 9, "");
}

BOOST_AUTO_TEST_CASE (test_rime_variant) {
    // int &&
    {
        variant v (6);
        variant const vc (6);
        BOOST_CHECK_EQUAL (v.which(), 0u);
        BOOST_CHECK_EQUAL (vc.which(), 0u);
        BOOST_CHECK (v.contains <int>());
        BOOST_CHECK (vc.contains <int>());
        BOOST_CHECK (!v.contains <float>());
        BOOST_CHECK (!vc.contains <int const>());

        BOOST_CHECK_EQUAL (rime::get_unsafe <int> (v), 6);
        BOOST_CHECK_EQUAL (rime::get <int> (v), 6);
        BOOST_CHECK_EQUAL (rime::get <int> (vc), 6);

        BOOST_CHECK_THROW (rime::get <int &> (v), rime::bad_get);
        // Check that bad_get derives from std::runtime_error
        BOOST_CHECK_THROW (rime::get <int &> (v), std::runtime_error);
        BOOST_CHECK_THROW (rime::get <int const> (vc), rime::bad_get);
        BOOST_CHECK_EQUAL (rime::get <int const> (&vc), (int *) 0);

        BOOST_CHECK_EQUAL (rime::get <int> (&v), &rime::get <int> (v));
        BOOST_CHECK_EQUAL (rime::get <int> (&vc), &rime::get <int> (vc));
        BOOST_CHECK_EQUAL (*rime::get <int> (&v), 6);
        BOOST_CHECK_EQUAL (*rime::get <int> (&vc), 6);

        // Assignment
        rime::get_unsafe <int> (v) = 5;
        BOOST_CHECK_EQUAL (rime::get <int> (v), 5);
        rime::get <int> (v) = 4;
        BOOST_CHECK_EQUAL (*rime::get <int> (&v), 4);
        *rime::get <int> (&v) = 3;
        BOOST_CHECK_EQUAL (rime::get_unsafe <int> (v), 3);

        BOOST_CHECK_EQUAL (rime::get_unsafe <int> (variant (6)), 6);
    }
    {
        rime::variant <float, const int> v (6);
        BOOST_CHECK_EQUAL (v.which(), 1u);
        BOOST_CHECK (v.contains <const int>());
    }
    {
        rime::variant <float, const int &> v (6);
        // Can't be converted into const int &
        BOOST_CHECK_EQUAL (v.which(), 0u);
        BOOST_CHECK (v.contains <float>());
    }

    // int const
    {
        variant v (get_int_const());
        variant const vc (get_int_const());
        BOOST_CHECK_EQUAL (v.which(), 3u);
        BOOST_CHECK_EQUAL (vc.which(), 3u);
        BOOST_CHECK (!v.contains <int>());
        BOOST_CHECK (v.contains <int const>());

        BOOST_CHECK_EQUAL (rime::get_unsafe <int const> (v), 5);
        BOOST_CHECK_EQUAL (rime::get_unsafe <int const> (vc), 5);

        BOOST_CHECK_THROW (rime::get <int> (v), rime::bad_get);
        BOOST_CHECK_EQUAL (rime::get <int const> (v), 5);
        BOOST_CHECK_EQUAL (*rime::get <int const> (&v), 5);
    }
    {
        rime::variant <float, int> v (get_int_const());
        BOOST_CHECK_EQUAL (v.which(), 1u);
    }
    {
        rime::variant <float, const int &> v (get_int_const());
        BOOST_CHECK_EQUAL (v.which(), 0u);
    }

    // int &
    int i = 4;
    {
        variant v (i);
        BOOST_CHECK_EQUAL (v.which(), 2u);
        variant const vc (i);
        BOOST_CHECK_EQUAL (vc.which(), 2u);
        BOOST_CHECK (v.contains <int &>());
        BOOST_CHECK (!v.contains <int>());

        BOOST_CHECK_EQUAL (rime::get_unsafe <int &> (v), 4);
        BOOST_CHECK_EQUAL (rime::get <int &> (v), 4);
        BOOST_CHECK_EQUAL (&rime::get <int &> (v), &i);
        BOOST_CHECK_THROW (rime::get <int> (v), rime::bad_get);

        rime::get_unsafe <int &> (v) = 27;
        BOOST_CHECK_EQUAL (rime::get_unsafe <int &> (v), 27);
        BOOST_CHECK_EQUAL (i, 27);
        i = 4;
        BOOST_CHECK_EQUAL (rime::get_unsafe <int &> (v), 4);
    }
    {
        rime::variant <float, int, const int> v (i);
        BOOST_CHECK_EQUAL (v.which(), 1u);
        BOOST_CHECK (v.contains <int>());
    }
    {
        rime::variant <float, const int> v (i);
        BOOST_CHECK_EQUAL (v.which(), 1u);
        BOOST_CHECK (v.contains <const int>());
    }
    {
        rime::variant <float, const int &> v (i);
        // Can't be converted into const int &
        BOOST_CHECK_EQUAL (v.which(), 0u);
        BOOST_CHECK (v.contains <float>());
    }

    // int const &
    int const ic = 7;
    {
        variant v (ic);
        BOOST_CHECK_EQUAL (v.which(), 4u);
        BOOST_CHECK (v.contains <int const &>());
        variant const vc (ic);
        BOOST_CHECK_EQUAL (vc.which(), 4u);

        BOOST_CHECK_EQUAL (rime::get_unsafe <int const &> (v), 7);
        BOOST_CHECK_EQUAL (rime::get <int const &> (v), 7);
        BOOST_CHECK_THROW (rime::get <int const> (v), rime::bad_get);
        BOOST_CHECK_THROW (rime::get <int &> (v), rime::bad_get);
    }
    {
        rime::variant <float, int, const int> v (ic);
        BOOST_CHECK_EQUAL (v.which(), 2u);
        BOOST_CHECK (v.contains <const int>());
        BOOST_CHECK (!v.contains <int>());
    }
    {
        rime::variant <float, int> v (ic);
        BOOST_CHECK_EQUAL (v.which(), 1u);
        BOOST_CHECK (v.contains <int>());
        BOOST_CHECK (!v.contains <float>());
    }
    {
        rime::variant <float, int &> v (ic);
        BOOST_CHECK_EQUAL (v.which(), 0u);
    }

    // float
    float f = 3.8;
    {
        variant v (4.5f);
        BOOST_CHECK_EQUAL (v.which(), 1u);
        BOOST_CHECK (v.contains <float>());

        BOOST_CHECK_EQUAL (rime::get_unsafe <float> (v), 4.5f);
    }
    {
        variant v (f);
        BOOST_CHECK_EQUAL (v.which(), 1u);
    }
    // Ambiguous, fails at compile-time:
    /*{
        variant v (4.5);
    }*/

    // char const *
    {
        variant v ("test");
        // std::string, not a char *
        BOOST_CHECK_EQUAL (v.which(), 5u);
        BOOST_CHECK (v.contains <std::string>());

        BOOST_CHECK_EQUAL (rime::get_unsafe <std::string> (v), "test");
        BOOST_CHECK_EQUAL (rime::get <std::string> (v), "test");
        BOOST_CHECK_EQUAL (*rime::get <std::string> (&v), "test");
        BOOST_CHECK_THROW (rime::get <float> (v), rime::bad_get);
        BOOST_CHECK_EQUAL (rime::get <int> (&v), (int *) 0);

        rime::get <std::string> (v) = "test2";
        BOOST_CHECK_EQUAL (rime::get_unsafe <std::string> (v), "test2");
    }
    {
        char const * p = "test";
        variant v (p);
        // std::string, not a char *
        BOOST_CHECK_EQUAL (v.which(), 5u);
        BOOST_CHECK (v.contains <std::string>());
    }

    char c[3] = {'a', 'c', 'q'};
    // char *
    {
        char * p = &c[0];
        variant v (p);
        BOOST_CHECK_EQUAL (v.which(), 7u);
        BOOST_CHECK (v.contains <char *>());

        BOOST_CHECK (rime::get <char *> (v) == p);
        BOOST_CHECK_THROW (rime::get <void *> (v), rime::bad_get);
    }

    // void *
    {
        void * p = &c[1];
        variant v (p);
        BOOST_CHECK_EQUAL (v.which(), 6u);
        BOOST_CHECK (v.contains <void *>());
        BOOST_CHECK (!v.contains <void>());
        BOOST_CHECK_EQUAL (rime::get <void *> (v), p);
    }
    // int *
    int ia[3] = {7, 5, 10};
    {
        int * p = &ia[0];
        variant v (p);
        BOOST_CHECK_EQUAL (v.which(), 6u);
        BOOST_CHECK (v.contains <void *>());
        BOOST_CHECK_EQUAL (rime::get <void *> (v), p);
    }

    // converted
    {
        variant v (std::move (converted()));
        BOOST_CHECK_EQUAL (v.which(), 8u);
        BOOST_CHECK (v.contains <converted>());
    }
    {
        converted c;
        variant v (c);
        BOOST_CHECK_EQUAL (v.which(), 8u);
        BOOST_CHECK (v.contains <converted>());
    }
    // convertee
    {
        variant v (std::move (convertee()));
        BOOST_CHECK_EQUAL (v.which(), 8u);
        BOOST_CHECK (v.contains <converted>());
    }
    {
        convertee c;
        variant v (c);
        BOOST_CHECK_EQUAL (v.which(), 8u);
        BOOST_CHECK (v.contains <converted>());
    }

    // void
    {
        variant v;
        BOOST_CHECK_EQUAL (v.which(), 9u);
        BOOST_CHECK (v.contains <void>());
        BOOST_CHECK (!v.contains <void *>());
        rime::get <void> (v);
        // It's not clear that it makes sense to return a void * with a
        // meaningless value, but it makes it easier to write generic code.
        rime::get <void> (&v);
    }
}

BOOST_AUTO_TEST_CASE (test_rime_variant_move_construct) {
    /*
    rime:variant is geared towards unifying return values.
    Essentially, this means a stack of return values.
    This uses move construction throughout.
    Move construction from variants with the same type parameters should
    retain the type exactly.
    Move construction from const variants (as opposed to a variant with const
    content) can add a "const" qualifier to the contained type.
    */

    // 0 int
    {
        // Sanity
        BOOST_CHECK_EQUAL (make_variant (7).which(), 0u);
        BOOST_CHECK_EQUAL (make_const_variant (13).which(), 0u);

        variant moved (make_variant (7));
        BOOST_CHECK_EQUAL (moved.which(), 0u);
        BOOST_CHECK_EQUAL (rime::get <int> (moved), 7);

        variant moved_const (make_const_variant (13));
        BOOST_CHECK_EQUAL (moved_const.which(), 3u);
        BOOST_CHECK_EQUAL (rime::get <int const> (moved_const), 13);
    }
    {
        // From other variant type
        BOOST_CHECK_EQUAL (make_other_variant (7).which(), 0u);
        BOOST_CHECK_EQUAL (make_const_other_variant (13).which(), 0u);

        variant moved (make_other_variant (7));
        BOOST_CHECK_EQUAL (moved.which(), 0u);
        BOOST_CHECK_EQUAL (rime::get <int> (moved), 7);

        variant moved_const (make_const_other_variant (13));
        BOOST_CHECK_EQUAL (moved_const.which(), 3u);
        BOOST_CHECK_EQUAL (rime::get <int const> (moved_const), 13);
    }

    // 1 float
    {
        // Sanity
        BOOST_CHECK_EQUAL (make_variant (7.3f).which(), 1u);
        BOOST_CHECK_EQUAL (make_const_variant (7.3f).which(), 1u);

        // Move construction
        variant moved (make_variant (7.3f));
        BOOST_CHECK_EQUAL (moved.which(), 1u);
        BOOST_CHECK_EQUAL (rime::get <float> (moved), 7.3f);

        variant moved_const (make_const_variant (7.3f));
        BOOST_CHECK_EQUAL (moved_const.which(), 1u);
        BOOST_CHECK_EQUAL (rime::get <float> (moved_const), 7.3f);
    }
    {
        // From other variant type
        BOOST_CHECK_EQUAL (make_other_variant (7.3f).which(), 1u);
        BOOST_CHECK_EQUAL (make_const_other_variant (7.3f).which(), 1u);

        variant moved (make_other_variant (7.3f));
        BOOST_CHECK_EQUAL (moved.which(), 1u);
        BOOST_CHECK_EQUAL (rime::get <float> (moved), 7.3f);

        variant moved_const (make_const_other_variant (7.3f));
        BOOST_CHECK_EQUAL (moved_const.which(), 1u);
        BOOST_CHECK_EQUAL (rime::get <float> (moved_const), 7.3f);
    }

    // 2 int &
    {
        int i = 4;
        // Sanity
        BOOST_CHECK_EQUAL (make_variant (i).which(), 2u);
        BOOST_CHECK_EQUAL (make_const_variant (i).which(), 2u);

        variant moved (make_variant (i));
        BOOST_CHECK_EQUAL (moved.which(), 2u);
        BOOST_CHECK_EQUAL (rime::get <int &> (moved), 4);

        variant moved_const (make_const_variant (i));
        BOOST_CHECK_EQUAL (moved_const.which(), 2u);
        BOOST_CHECK_EQUAL (rime::get <int &> (moved_const), 4);
    }
    {
        // From other variant type
        int i = 4;
        BOOST_CHECK_EQUAL (make_other_variant (i).which(), 2u);
        BOOST_CHECK_EQUAL (make_const_other_variant (i).which(), 2u);

        variant moved (make_other_variant (i));
        BOOST_CHECK_EQUAL (moved.which(), 2u);
        BOOST_CHECK_EQUAL (rime::get <int &> (moved), 4);

        variant moved_const (make_const_other_variant (i));
        BOOST_CHECK_EQUAL (moved_const.which(), 2u);
        BOOST_CHECK_EQUAL (rime::get <int &> (moved_const), 4);
    }

    // 3 int const
    {
        // Sanity
        BOOST_CHECK_EQUAL (make_variant (get_int_const()).which(), 3u);
        BOOST_CHECK_EQUAL (make_const_variant (get_int_const()).which(), 3u);

        // Move construction
        variant moved (make_variant (get_int_const()));
        BOOST_CHECK_EQUAL (moved.which(), 3u);
        BOOST_CHECK_EQUAL (rime::get <int const> (moved), 5);

        variant moved_const (make_const_variant (get_int_const()));
        BOOST_CHECK_EQUAL (moved_const.which(), 3u);
        BOOST_CHECK_EQUAL (rime::get <int const> (moved_const), 5);
    }
    {
        // From other variant type
        BOOST_CHECK_EQUAL (make_other_variant (get_int_const()).which(), 3u);
        BOOST_CHECK_EQUAL (make_const_other_variant (get_int_const()).which(),
            3u);

        variant moved (make_other_variant (get_int_const()));
        BOOST_CHECK_EQUAL (moved.which(), 3u);
        BOOST_CHECK_EQUAL (rime::get <int const> (moved), 5);

        variant moved_const (make_const_other_variant (get_int_const()));
        BOOST_CHECK_EQUAL (moved_const.which(), 3u);
        BOOST_CHECK_EQUAL (rime::get <int const> (moved_const), 5);
    }

    // 4 int const &
    {
        int const ic = 7;
        // Sanity
        BOOST_CHECK_EQUAL (make_variant (ic).which(), 4u);
        BOOST_CHECK_EQUAL (make_const_variant (ic).which(), 4u);

        variant moved (make_variant (ic));
        BOOST_CHECK_EQUAL (moved.which(), 4u);
        BOOST_CHECK_EQUAL (rime::get <int const &> (moved), 7);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (moved), &ic);

        variant moved_const (make_const_variant (ic));
        BOOST_CHECK_EQUAL (moved_const.which(), 4u);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (moved_const), &ic);
    }
    {
        // From other variant type
        int const ic = 7;
        BOOST_CHECK_EQUAL (make_other_variant (ic).which(), 4u);
        BOOST_CHECK_EQUAL (make_const_other_variant (ic).which(), 4u);

        variant moved (make_other_variant (ic));
        BOOST_CHECK_EQUAL (moved.which(), 4u);
        BOOST_CHECK_EQUAL (rime::get <int const &> (moved), 7);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (moved), &ic);

        variant moved_const (make_const_other_variant (ic));
        BOOST_CHECK_EQUAL (moved_const.which(), 4u);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (moved_const), &ic);
    }

    // 5 std::string
    {
        // Sanity
        BOOST_CHECK_EQUAL (make_variant (std::string ("Hello")).which(), 5u);
        BOOST_CHECK_EQUAL (make_const_variant (std::string ("Hello")).which(),
            5u);

        // Move construction
        variant moved (make_variant (std::string ("Hello")));
        BOOST_CHECK_EQUAL (moved.which(), 5u);
        BOOST_CHECK_EQUAL (rime::get <std::string> (moved), "Hello");

        variant moved_const (make_const_variant (std::string ("Hello")));
        BOOST_CHECK_EQUAL (moved_const.which(), 5u);
        BOOST_CHECK_EQUAL (rime::get <std::string> (moved_const), "Hello");
    }

    // 6 void *
    {
        int i = 7;
        // Sanity
        BOOST_CHECK_EQUAL (make_variant (&i).which(), 6u);
        BOOST_CHECK_EQUAL (make_const_variant (&i).which(), 6u);

        // Move construction
        variant moved (make_variant (&i));
        BOOST_CHECK_EQUAL (moved.which(), 6u);
        BOOST_CHECK_EQUAL (rime::get <void *> (moved), &i);

        variant moved_const (make_const_variant (&i));
        BOOST_CHECK_EQUAL (moved_const.which(), 6u);
        BOOST_CHECK_EQUAL (rime::get <void *> (moved_const), &i);
    }

    // 7 char *
    {
        char c = 'A';
        // Sanity
        BOOST_CHECK_EQUAL (make_variant (&c).which(), 7u);
        BOOST_CHECK_EQUAL (make_const_variant (&c).which(), 7u);

        // Move construction
        variant moved (make_variant (&c));
        BOOST_CHECK_EQUAL (moved.which(), 7u);
        BOOST_CHECK_EQUAL (rime::get <char *> (moved), &c);

        variant moved_const (make_const_variant (&c));
        BOOST_CHECK_EQUAL (moved_const.which(), 7u);
        BOOST_CHECK_EQUAL (rime::get <char *> (moved_const), &c);
    }

    // 8 converted
    {
        // Sanity
        BOOST_CHECK_EQUAL (make_variant (converted()).which(), 8u);
        BOOST_CHECK_EQUAL (make_const_variant (converted()).which(), 8u);

        // Move construction
        variant moved (make_variant (converted()));
        BOOST_CHECK_EQUAL (moved.which(), 8u);
        rime::get <converted> (moved);

        variant moved_const (make_const_variant (converted()));
        BOOST_CHECK_EQUAL (moved_const.which(), 8u);
        rime::get <converted> (moved_const);
    }
    // 9 void
    {
        // Sanity
        BOOST_CHECK_EQUAL (make_variant().which(), 9u);
        BOOST_CHECK_EQUAL (make_const_variant().which(), 9u);

        // Move construction
        variant moved (make_variant());
        BOOST_CHECK_EQUAL (moved.which(), 9u);

        variant moved_const (make_const_variant());
        BOOST_CHECK_EQUAL (moved_const.which(), 9u);
    }
}

BOOST_AUTO_TEST_CASE (test_rime_variant_copy_construct) {
    // 0 int
    {
        variant v (7);
        variant const vc (7);
        // Sanity
        BOOST_CHECK_EQUAL (v.which(), 0u);
        BOOST_CHECK_EQUAL (vc.which(), 0u);

        // int becomes int const &.
        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 4u);
        BOOST_CHECK_EQUAL (rime::get <int const &> (copied), 7);
        // And the original int can be edited.
        rime::get <int> (v) = 10;
        BOOST_CHECK_EQUAL (rime::get <int const &> (copied), 10);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 4u);
        BOOST_CHECK_EQUAL (rime::get <int const &> (copied_const), 7);
    }
    {
        // From other variant type
        other_variant v (7);
        other_variant const vc (7);
        BOOST_CHECK_EQUAL (v.which(), 0u);
        BOOST_CHECK_EQUAL (vc.which(), 0u);

        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 4u);
        BOOST_CHECK_EQUAL (rime::get <int const &> (copied), 7);
        rime::get <int> (v) = 10;
        BOOST_CHECK_EQUAL (rime::get <int const &> (copied), 10);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 4u);
        BOOST_CHECK_EQUAL (rime::get <int const &> (copied_const), 7);
    }

    // 1 float
    {
        variant v (7.3f);
        variant const vc (7.3f);
        // Sanity
        BOOST_CHECK_EQUAL (v.which(), 1u);
        BOOST_CHECK_EQUAL (vc.which(), 1u);

        // float becomes float.
        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 1u);
        BOOST_CHECK_EQUAL (rime::get <float> (copied), 7.3f);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 1u);
        BOOST_CHECK_EQUAL (rime::get <float> (copied_const), 7.3f);
    }
    {
        // From other variant type
        other_variant v (7.3f);
        other_variant const vc (7.3f);
        BOOST_CHECK_EQUAL (v.which(), 1u);
        BOOST_CHECK_EQUAL (vc.which(), 1u);

        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 1u);
        BOOST_CHECK_EQUAL (rime::get <float> (copied), 7.3f);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 1u);
        BOOST_CHECK_EQUAL (rime::get <float> (copied_const), 7.3f);
    }

    // 2 int &
    {
        int i = 4;
        variant v (i);
        variant const vc (i);
        // Sanity
        BOOST_CHECK_EQUAL (v.which(), 2u);
        BOOST_CHECK_EQUAL (vc.which(), 2u);

        // int & remains int &.
        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 2u);
        BOOST_CHECK_EQUAL (rime::get <int &> (copied), 4);
        BOOST_CHECK_EQUAL (&rime::get <int &> (copied), &i);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 2u);
        BOOST_CHECK_EQUAL (&rime::get <int &> (copied_const), &i);
    }
    {
        // From other variant type
        int i = 4;
        other_variant v (i);
        other_variant const vc (i);
        BOOST_CHECK_EQUAL (v.which(), 2u);
        BOOST_CHECK_EQUAL (vc.which(), 2u);

        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 2u);
        BOOST_CHECK_EQUAL (rime::get <int &> (copied), 4);
        BOOST_CHECK_EQUAL (&rime::get <int &> (copied), &i);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 2u);
        BOOST_CHECK_EQUAL (&rime::get <int &> (copied_const), &i);
    }

    // 3 int const
    {
        variant v (get_int_const());
        variant const vc (get_int_const());
        // Sanity
        BOOST_CHECK_EQUAL (v.which(), 3u);
        BOOST_CHECK_EQUAL (vc.which(), 3u);

        // Copy construction: int const becomes int const & pointing to the
        // original int const.
        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 4u);
        BOOST_CHECK_EQUAL (rime::get <int const &> (copied), 5);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (copied),
            &rime::get <int const> (v));

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 4u);
        BOOST_CHECK_EQUAL (rime::get <int const &> (copied_const), 5);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (copied_const),
            &rime::get <int const> (vc));
    }
    {
        // From other variant type
        other_variant v (get_int_const());
        other_variant const vc (get_int_const());
        BOOST_CHECK_EQUAL (v.which(), 3u);
        BOOST_CHECK_EQUAL (vc.which(), 3u);

        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 4u);
        BOOST_CHECK_EQUAL (rime::get <int const &> (copied), 5);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (copied),
            &rime::get <int const> (v));

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 4u);
        BOOST_CHECK_EQUAL (rime::get <int const &> (copied_const), 5);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (copied_const),
            &rime::get <int const> (vc));
    }

    // 4 int const &
    {
        int const ic = 12;
        variant v (ic);
        variant const vc (ic);
        // Sanity
        BOOST_CHECK_EQUAL (v.which(), 4u);
        BOOST_CHECK_EQUAL (vc.which(), 4u);

        // int const & remains int const &.
        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 4u);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (copied), &ic);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 4u);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (copied_const), &ic);
    }
    {
        // From other variant type
        int const ic = 12;
        other_variant v (ic);
        other_variant const vc (ic);
        BOOST_CHECK_EQUAL (v.which(), 4u);
        BOOST_CHECK_EQUAL (vc.which(), 4u);

        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 4u);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (copied), &ic);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 4u);
        BOOST_CHECK_EQUAL (&rime::get <int const &> (copied_const), &ic);
    }

    // 5 std::string
    {
        variant v (std::string ("Hello"));
        variant const vc (std::string ("Hello"));
        // Sanity
        BOOST_CHECK_EQUAL (v.which(), 5u);
        BOOST_CHECK_EQUAL (vc.which(), 5u);

        // float becomes float.
        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 5u);
        BOOST_CHECK_EQUAL (rime::get <std::string> (copied), "Hello");

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 5u);
        BOOST_CHECK_EQUAL (rime::get <std::string> (copied_const), "Hello");
    }

    // 6 void *
    {
        int i = 7;
        variant v (&i);
        variant const vc (&i);
        // Sanity
        BOOST_CHECK_EQUAL (v.which(), 6u);
        BOOST_CHECK_EQUAL (vc.which(), 6u);

        // float becomes float.
        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 6u);
        BOOST_CHECK_EQUAL (rime::get <void *> (copied), &i);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 6u);
        BOOST_CHECK_EQUAL (rime::get <void *> (copied_const), &i);
    }

    // 7 char *
    {
        char c = 7;
        variant v (&c);
        variant const vc (&c);
        // Sanity
        BOOST_CHECK_EQUAL (v.which(), 7u);
        BOOST_CHECK_EQUAL (vc.which(), 7u);

        // float becomes float.
        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 7u);
        BOOST_CHECK_EQUAL (rime::get <char *> (copied), &c);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 7u);
        BOOST_CHECK_EQUAL (rime::get <char *> (copied_const), &c);
    }

    // 8 converted
    {
        converted c;
        variant v (c);
        variant const vc (c);
        // Sanity
        BOOST_CHECK_EQUAL (v.which(), 8u);
        BOOST_CHECK_EQUAL (vc.which(), 8u);

        // float becomes float.
        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 8u);
        rime::get <converted> (copied);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 8u);
        rime::get <converted> (copied_const);
    }

    // 9 void
    {
        variant v;
        variant const vc;
        // Sanity
        BOOST_CHECK_EQUAL (v.which(), 9u);
        BOOST_CHECK_EQUAL (vc.which(), 9u);

        variant copied (v);
        BOOST_CHECK_EQUAL (copied.which(), 9u);

        variant copied_const (vc);
        BOOST_CHECK_EQUAL (copied_const.which(), 9u);
    }
}

struct counts {
    int construct;
    int copy_construct;
    int move_construct;
    int destruct;
    int destruct_moved;
    int copy_assign;
    int move_assign;

    counts()
    : construct(), copy_construct(), move_construct(),
        destruct(), destruct_moved(),
        copy_assign(), move_assign() {}

    bool consistent() const {
        // All constructed objects must have been destructed.
        if (construct + copy_construct + move_construct !=
            destruct + destruct_moved)
            return false;
        // All carcasses must have been destructed.
        if (move_construct + move_assign != destruct_moved)
            return false;
        return true;
    }
};

class counted {
    counts & count;
    bool moved;

public:
    counted (counts & count)
    : count (count), moved (false)
    { ++ count.construct; }

    counted (counted const & that)
    : count (that.count), moved (false)
    { ++ count.copy_construct; }

    counted (counted && that)
    : count (that.count), moved (false) {
        BOOST_CHECK (that.valid());
        ++ count.move_construct;
        that.move();
    }

    ~counted() {
        if (this->moved)
            ++ count.destruct_moved;
        else
            ++ count.destruct;
    }

    bool valid() const { return !this->moved; }

    void move() { this->moved = true; }

    counted & operator = (counted const & that) {
        BOOST_CHECK (that.valid());
        ++ count.copy_assign;
        return *this;
    }

    counted & operator = (counted && that) {
        BOOST_CHECK (that.valid());
        ++ count.move_assign;
        that.move();
        return *this;
    }

    counts & get_counts() { return count; }
};

counted make_counted (counts & c) { return counted (c); }

struct counted_assignable {
    counted_assignable & operator = (counted const &)
    { return *this; }
};

template <typename Variant, typename Variant2>
    void test_rime_variant_objects_counted()
{
    // Construction and destruction
    {
        counts count;
        {
            Variant v (make_counted (count));
        }
        BOOST_CHECK_EQUAL (count.construct, 1);
        BOOST_CHECK_EQUAL (count.move_construct, 1);
        BOOST_CHECK_EQUAL (count.destruct, 1);
        BOOST_CHECK_EQUAL (count.destruct_moved, 1);
        BOOST_CHECK (count.consistent());
    }

    // Copy-construction
    {
        counts count;
        {
            counted c (count);
            // Copy-construct v from c

            Variant v (c);
            BOOST_CHECK_EQUAL (count.copy_construct, 1);
            BOOST_CHECK_EQUAL (&(rime::get <counted> (v).get_counts()), &count);

            // Copy-construct v2 from v.
            // Underneath, this should copy-construct the counted in v2 from
            // the counted in v.
            Variant2 v2 (v);
            BOOST_CHECK_EQUAL (count.copy_construct, 2);
            BOOST_CHECK_EQUAL (&(rime::get <counted> (v2).get_counts()),
                &count);
        }
        BOOST_CHECK_EQUAL (count.construct, 1);
        BOOST_CHECK_EQUAL (count.move_construct, 0);
        BOOST_CHECK_EQUAL (count.copy_construct, 2);
        BOOST_CHECK_EQUAL (count.destruct, 3);
        BOOST_CHECK_EQUAL (count.destruct_moved, 0);
        BOOST_CHECK (count.consistent());
    }

    // Move-construction
    {
        counts count;
        {
            counted c (count);
            BOOST_CHECK_EQUAL (count.copy_construct, 0);

            // Move-construct v from c
            Variant v (std::move (c));
            BOOST_CHECK_EQUAL (count.copy_construct, 0);
            BOOST_CHECK_EQUAL (count.move_construct, 1);
            BOOST_CHECK_EQUAL (&(rime::get <counted> (v).get_counts()), &count);

            // Move-construct v2 from v.
            // Underneath, this should move-construct the counted in v2 from
            // the counted in v.
            Variant2 v2 (std::move (v));
            BOOST_CHECK_EQUAL (count.copy_construct, 0);
            BOOST_CHECK_EQUAL (count.move_construct, 2);
            BOOST_CHECK_EQUAL (&(rime::get <counted> (v2).get_counts()),
                &count);
        }
        BOOST_CHECK_EQUAL (count.construct, 1);
        BOOST_CHECK_EQUAL (count.move_construct, 2);
        BOOST_CHECK_EQUAL (count.copy_construct, 0);
        BOOST_CHECK_EQUAL (count.destruct, 1);
        BOOST_CHECK_EQUAL (count.destruct_moved, 2);
        BOOST_CHECK (count.consistent());
    }

    // Move get.
    // If the variant is a temporary, "get" should return an rvalue reference.
    {
        counts count;
        {
            counted c (count);
            BOOST_CHECK_EQUAL (count.copy_construct, 0);

            Variant v (std::move (c));
            // Sanity check; see previous test.
            BOOST_CHECK_EQUAL (count.copy_construct, 0);
            BOOST_CHECK_EQUAL (count.move_construct, 1);

            // get
            counted c2 (rime::get <counted> (std::move (v)));
            BOOST_CHECK_EQUAL (count.copy_construct, 0);
            BOOST_CHECK_EQUAL (count.move_construct, 2);

            Variant v2 (std::move (c2));
            // unsafe_get
            counted c3 (rime::get_unsafe <counted> (std::move (v2)));
            BOOST_CHECK_EQUAL (count.copy_construct, 0);
            BOOST_CHECK_EQUAL (count.move_construct, 4);
        }
        BOOST_CHECK_EQUAL (count.construct, 1);
        BOOST_CHECK_EQUAL (count.move_construct, 4);
        BOOST_CHECK_EQUAL (count.copy_construct, 0);
        BOOST_CHECK_EQUAL (count.destruct, 1);
        BOOST_CHECK_EQUAL (count.destruct_moved, 4);
        BOOST_CHECK (count.consistent());
    }
}

// Test construction, destruction, copying, moving
BOOST_AUTO_TEST_CASE (test_rime_variant_objects) {
    typedef rime::variant <counted, int> variant1;
    typedef rime::variant <int, counted> variant2;
    typedef rime::variant <counted> variant3;
    typedef rime::variant <int, counted, float> variant4;

    test_rime_variant_objects_counted <variant1, variant1>();
    test_rime_variant_objects_counted <variant2, variant2>();
    test_rime_variant_objects_counted <variant3, variant3>();
    test_rime_variant_objects_counted <variant4, variant4>();

    test_rime_variant_objects_counted <variant1, variant2>();
    test_rime_variant_objects_counted <variant3, variant4>();
    test_rime_variant_objects_counted <variant4, variant1>();

    test_rime_variant_objects_counted <variant2, variant4>();
    test_rime_variant_objects_counted <variant3, variant1>();
    test_rime_variant_objects_counted <variant4, variant2>();

    test_rime_variant_objects_counted <variant1, variant4>();
    test_rime_variant_objects_counted <variant2, variant1>();
    test_rime_variant_objects_counted <variant3, variant2>();

    // Moving from references.
    {
        counts count;
        {
            counted c (count);
            rime::variant <counted &, int> v (c);
            BOOST_CHECK_EQUAL (count.construct, 1);
            BOOST_CHECK_EQUAL (count.copy_construct, 0);

            variant1 v2 (std::move (v));
            // c must be unaffected and not moved, just copied.
            BOOST_CHECK_EQUAL (count.construct, 1);
            BOOST_CHECK_EQUAL (count.move_construct, 0);
            BOOST_CHECK_EQUAL (count.copy_construct, 1);
        }
        BOOST_CHECK_EQUAL (count.destruct, 2);
        BOOST_CHECK (count.consistent());
    }
}

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
        counts count;
        {
            counted c (count);
            rime::variant <counted, counted_assignable> v (
                make_counted (count));

            // Sanity
            BOOST_CHECK_EQUAL (count.construct, 2);
            BOOST_CHECK_EQUAL (count.move_construct, 1);
            BOOST_CHECK_EQUAL (count.copy_assign, 0);
            BOOST_CHECK_EQUAL (count.move_assign, 0);

            // Copy assignment
            counted c2 (count);
            BOOST_CHECK_EQUAL (count.construct, 3);
            v = c2;
            BOOST_CHECK_EQUAL (count.copy_assign, 1);
            BOOST_CHECK_EQUAL (count.move_assign, 0);

            // Move assignment
            v = make_counted (count);
            BOOST_CHECK_EQUAL (count.construct, 4);
            BOOST_CHECK_EQUAL (count.move_construct, 1);
            BOOST_CHECK_EQUAL (count.copy_assign, 1);
            BOOST_CHECK_EQUAL (count.move_assign, 1);

            // From other variant type.
            // All possible content types for v2 must be convertible to
            // counted.
            rime::variant <counted, counted &> v2 (make_counted (count));
            BOOST_CHECK_EQUAL (count.construct, 5);
            BOOST_CHECK_EQUAL (count.move_construct, 2);
            v = v2;
            BOOST_CHECK_EQUAL (count.copy_assign, 2);
        }
        BOOST_CHECK (count.consistent());
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

struct plus {
public:
    template <typename Argument>
        Argument operator() (Argument argument) const
    { return argument; }

    template <typename First, typename ... Rest>
        auto operator() (First first, Rest ... rest) const
    -> decltype (first + (std::declval <plus>()) (rest ...))
    { return first + (*this) (rest ...); }
};

struct times {
public:
    template <typename Argument>
        Argument operator() (Argument argument) const
    { return argument; }

    template <typename First, typename ... Rest>
        auto operator() (First first, Rest ... rest) const
    -> decltype (first * (std::declval <plus>()) (rest ...))
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

