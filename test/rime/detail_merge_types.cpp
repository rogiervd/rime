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

#define BOOST_TEST_MODULE test_rime_detail_merge_types
#include "../boost_unit_test.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "rime/detail/merge_types.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_detail_merge_types)

//#include "check_set.hpp"

// Simple constant class to check merging
template <class Type, Type N> struct constant
{
    operator Type () const { return N; }
};

template <class Base> struct merge_constant
{
    template <class Type1, class Type2> struct apply
    : Base::template apply <Type1, Type2> {};

    template <class Type1, Type1 N1, class Type2>
        struct apply <constant <Type1, N1>, Type2>
    : Base::template apply <Type1, Type2> {};

    template <class Type1, class Type2, Type2 N2>
        struct apply <Type1, constant <Type2, N2> >
    : Base::template apply <Type1, Type2> {};

    template <class Type1, Type1 N1, class Type2, Type2 N2>
        struct apply <constant <Type1, N1>, constant <Type2, N2> >
    : Base::template apply <Type1, Type2> {};
};

BOOST_AUTO_TEST_CASE (test_merge_types_internal_merge_two) {
    using rime::detail::merge_two::is_implemented;
    using rime::detail::merge_two::is_unimplemented;
    typedef rime::detail::merge_two::same<> same;
    typedef rime::detail::merge_two::const_<> const_;
    typedef rime::detail::merge_two::reference<> reference;

    // same
    BOOST_MPL_ASSERT ((is_implemented <same, int, int>));
    BOOST_MPL_ASSERT ((is_implemented <same, long, long>));
    BOOST_MPL_ASSERT ((is_unimplemented <same, int, long>));

    BOOST_MPL_ASSERT ((std::is_same <
        same::apply <int, int>::type,
        int>));

    BOOST_MPL_ASSERT ((std::is_same <
        same::apply <long, long>::type,
        long>));

    // const
    BOOST_MPL_ASSERT ((is_implemented <const_, int, int>));
    BOOST_MPL_ASSERT ((is_implemented <const_, int const, int const>));
    BOOST_MPL_ASSERT ((is_implemented <const_, int const, int>));
    BOOST_MPL_ASSERT ((is_implemented <const_, int, int const>));
    BOOST_MPL_ASSERT ((is_unimplemented <const_, int, long>));
    BOOST_MPL_ASSERT ((is_unimplemented <const_, int, long const>));

    BOOST_MPL_ASSERT ((std::is_same <
        const_::apply <int, int>::type,
        int>));

    BOOST_MPL_ASSERT ((std::is_same <
        const_::apply <int const, int>::type,
        int const>));

    BOOST_MPL_ASSERT ((std::is_same <
        const_::apply <int const, int const>::type,
        int const>));

    // reference
    BOOST_MPL_ASSERT ((is_implemented <reference, int, int>));
    BOOST_MPL_ASSERT ((is_implemented <reference, int &, int>));
    BOOST_MPL_ASSERT ((is_implemented <reference, int, int &>));
    BOOST_MPL_ASSERT ((is_implemented <reference, int &, int &>));
    BOOST_MPL_ASSERT ((is_unimplemented <reference, int &, long &>));
    BOOST_MPL_ASSERT ((is_unimplemented <reference, int, long &>));

    BOOST_MPL_ASSERT ((std::is_same <
        reference::apply <int, int>::type,
        int>));

    BOOST_MPL_ASSERT ((std::is_same <
        reference::apply <int &, int>::type,
        int>));

    BOOST_MPL_ASSERT ((std::is_same <
        reference::apply <int &, int &>::type,
        int &>));
}

BOOST_AUTO_TEST_CASE (test_merge_types_internal_insert) {
    using rime::detail::merge::insert;
    using meta::vector;
    typedef rime::detail::merge_two::reference<> merge_two;

    BOOST_MPL_ASSERT ((std::is_same <
        insert <merge_two, int, vector<>>::type,
        vector <int> >));

    BOOST_MPL_ASSERT ((std::is_same <
        insert <merge_two, int, vector <int>>::type,
        vector <int> >));

    BOOST_MPL_ASSERT ((std::is_same <
        insert <merge_two, int, vector <long>>::type,
        vector <int, long> >));

    BOOST_MPL_ASSERT ((std::is_same <
        insert <merge_two, int const, vector <int &>>::type,
        vector <int const> >));

    BOOST_MPL_ASSERT ((std::is_same <
        insert <merge_two, int const, vector <char, int &>>::type,
        vector <int const, char> >));

    BOOST_MPL_ASSERT ((std::is_same <
        insert <merge_two, int const, vector <int &, char>>::type,
        vector <int const, char> >));
}

BOOST_AUTO_TEST_CASE (test_merge_types_reference) {
    using rime::detail::merge_types;
    using meta::vector;
    typedef rime::detail::merge_two::reference<> merge_two;

    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two, vector<> >::type,
            vector <> >));
    }

    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two,
                vector <int, int, long &, int const, int &> >::type,
            vector <int const, long &> >));
    }

    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two,
                vector <int> >::type,
            vector <int> >));
    }

    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two,
                vector <int, long> >::type,
            vector <int, long> >));
    }

    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two,
                vector <int &, long> >::type,
            vector <int &, long> >));
    }

    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two,
                vector <int const &, int> >::type,
            vector <int const> >));
    }

    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two,
                vector <long, int const &, int> >::type,
            vector <long, int const> >));
    }

    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two,
                vector <int const &, int, long> >::type,
            vector <int const, long> >));
    }

    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two,
                vector <int, int, long &, int const, int &> >::type,
            vector <int const, long &> >));
    }
}

BOOST_AUTO_TEST_CASE (test_merge_types_full) {
    using rime::detail::merge_types;
    using meta::vector;
    typedef rime::detail::merge_two::reference <rime::detail::merge_two::const_ <
        merge_constant <rime::detail::merge_two::same<> > > > merge_two;

    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two,
                vector <int, int, long &, int const, int &> >::type,
            vector <int const, long &> >));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two,
                vector <int, constant <int, 3> > >::type,
            vector <int> >));
    }
    {
        BOOST_MPL_ASSERT ((std::is_same <
            merge_types <merge_two,
                vector <int const &, constant <int, 3> > >::type,
            vector <int const> >));
    }
}

BOOST_AUTO_TEST_SUITE_END()

