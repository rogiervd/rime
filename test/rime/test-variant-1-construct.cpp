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

#define BOOST_TEST_MODULE test_rime_variant
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

struct tracked_assignable {
    tracked_assignable & operator = (utility::tracked<> const &)
    { return *this; }
};

template <typename Variant, typename Variant2>
    void test_rime_variant_objects_tracked()
{
    using utility::tracked;
    // Construction and destruction
    {
        utility::tracked_registry registry;
        {
            Variant v = tracked<> (registry);
        }
        BOOST_CHECK_EQUAL (registry.value_construct_count(), 1);
        BOOST_CHECK_EQUAL (registry.move_count(), 1);
        BOOST_CHECK_EQUAL (registry.destruct_count(), 1);
        BOOST_CHECK_EQUAL (registry.destruct_moved_count(), 1);
        BOOST_CHECK (registry.consistent());
    }
    // Copy-construction
    {
        utility::tracked_registry registry;
        {
            tracked<> c (registry);
            // Copy-construct v from c

            Variant v (c);
            BOOST_CHECK_EQUAL (registry.copy_count(), 1);

            // Copy-construct v2 from v.
            // Underneath, this should copy-construct the tracked in v2 from
            // the tracked in v.
            Variant2 v2 (v);
            BOOST_CHECK_EQUAL (registry.copy_count(), 2);
        }
        BOOST_CHECK_EQUAL (registry.value_construct_count(), 1);
        BOOST_CHECK_EQUAL (registry.move_count(), 0);
        BOOST_CHECK_EQUAL (registry.copy_count(), 2);
        BOOST_CHECK_EQUAL (registry.destruct_count(), 3);
        BOOST_CHECK_EQUAL (registry.destruct_moved_count(), 0);
        BOOST_CHECK (registry.consistent());
    }

    // Move-construction
    {
        utility::tracked_registry registry;
        {
            tracked<> c (registry);
            BOOST_CHECK_EQUAL (registry.copy_count(), 0);

            // Move-construct v from c
            Variant v (std::move (c));
            BOOST_CHECK_EQUAL (registry.copy_count(), 0);
            BOOST_CHECK_EQUAL (registry.move_count(), 1);

            // Move-construct v2 from v.
            // Underneath, this should move-construct the tracked in v2 from
            // the tracked in v.
            Variant2 v2 (std::move (v));
            BOOST_CHECK_EQUAL (registry.copy_count(), 0);
            BOOST_CHECK_EQUAL (registry.move_count(), 2);
        }
        BOOST_CHECK_EQUAL (registry.value_construct_count(), 1);
        BOOST_CHECK_EQUAL (registry.move_count(), 2);
        BOOST_CHECK_EQUAL (registry.copy_count(), 0);
        BOOST_CHECK_EQUAL (registry.destruct_count(), 1);
        BOOST_CHECK_EQUAL (registry.destruct_moved_count(), 2);
        BOOST_CHECK (registry.consistent());
    }

    // Move get.
    // If the variant is a temporary, "get" should return an rvalue reference.
    {
        utility::tracked_registry registry;
        {
            tracked<> c (registry);
            BOOST_CHECK_EQUAL (registry.copy_count(), 0);

            Variant v (std::move (c));
            // Sanity check; see previous test.
            BOOST_CHECK_EQUAL (registry.copy_count(), 0);
            BOOST_CHECK_EQUAL (registry.move_count(), 1);

            // get
            tracked<> c2 (rime::get <tracked<>> (std::move (v)));
            BOOST_CHECK_EQUAL (registry.copy_count(), 0);
            BOOST_CHECK_EQUAL (registry.move_count(), 2);

            Variant v2 (std::move (c2));
            // unsafe_get
            tracked<> c3 (rime::get_unsafe <tracked<>> (std::move (v2)));
            BOOST_CHECK_EQUAL (registry.copy_count(), 0);
            BOOST_CHECK_EQUAL (registry.move_count(), 4);
        }
        BOOST_CHECK_EQUAL (registry.value_construct_count(), 1);
        BOOST_CHECK_EQUAL (registry.move_count(), 4);
        BOOST_CHECK_EQUAL (registry.copy_count(), 0);
        BOOST_CHECK_EQUAL (registry.destruct_count(), 1);
        BOOST_CHECK_EQUAL (registry.destruct_moved_count(), 4);
        BOOST_CHECK (registry.consistent());
    }
}

// Test construction, destruction, copying, moving
BOOST_AUTO_TEST_CASE (test_rime_variant_objects) {
    typedef rime::variant <utility::tracked<>, int> variant1;
    typedef rime::variant <int, utility::tracked<>> variant2;
    typedef rime::variant <utility::tracked<>> variant3;
    typedef rime::variant <int, utility::tracked<>, float> variant4;

    test_rime_variant_objects_tracked <variant1, variant1>();
    test_rime_variant_objects_tracked <variant2, variant2>();
    test_rime_variant_objects_tracked <variant3, variant3>();
    test_rime_variant_objects_tracked <variant4, variant4>();

    test_rime_variant_objects_tracked <variant1, variant2>();
    test_rime_variant_objects_tracked <variant3, variant4>();
    test_rime_variant_objects_tracked <variant4, variant1>();

    test_rime_variant_objects_tracked <variant2, variant4>();
    test_rime_variant_objects_tracked <variant3, variant1>();
    test_rime_variant_objects_tracked <variant4, variant2>();

    test_rime_variant_objects_tracked <variant1, variant4>();
    test_rime_variant_objects_tracked <variant2, variant1>();
    test_rime_variant_objects_tracked <variant3, variant2>();

    // Moving from references.
    {
        utility::tracked_registry registry;
        {
            utility::tracked<> c (registry);
            rime::variant <utility::tracked<> &, int> v (c);
            BOOST_CHECK_EQUAL (registry.value_construct_count(), 1);
            BOOST_CHECK_EQUAL (registry.copy_count(), 0);

            variant1 v2 (std::move (v));
            // c must be unaffected and not moved, just copied.
            BOOST_CHECK_EQUAL (registry.value_construct_count(), 1);
            BOOST_CHECK_EQUAL (registry.move_count(), 0);
            BOOST_CHECK_EQUAL (registry.copy_count(), 1);
        }
        BOOST_CHECK_EQUAL (registry.destruct_count(), 2);
        BOOST_CHECK (registry.consistent());
    }
}

BOOST_AUTO_TEST_SUITE_END()
