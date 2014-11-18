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

#define BOOST_TEST_MODULE test_rime_detail_merge_types
#include "utility/test/boost_unit_test.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "rime/merge_types.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_detail_merge_types)

struct base {};
struct derived : base {};

// Simple constant class to check merging
template <class Type, Type N> struct constant {
    operator Type () const { return N; }
};

template <class Base> struct merge_constant {
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

BOOST_AUTO_TEST_CASE (test_merge_types_merge_policy) {
    using rime::merge_policy::is_implemented;
    using rime::merge_policy::is_unimplemented;
    typedef rime::merge_policy::same<> same;
    typedef rime::merge_policy::const_<> const_;
    typedef rime::merge_policy::reference<> reference;
    typedef rime::merge_policy::decay<> decay;
    using rime::merge_policy::base_class;
    using rime::merge_policy::common_type;

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
    BOOST_MPL_ASSERT ((is_implemented <reference, int, int &>));
    BOOST_MPL_ASSERT ((is_implemented <reference, int, int &&>));
    BOOST_MPL_ASSERT ((is_implemented <reference, int &, int>));
    BOOST_MPL_ASSERT ((is_implemented <reference, int &, int &>));
    BOOST_MPL_ASSERT ((is_implemented <reference, int &, int &&>));
    BOOST_MPL_ASSERT ((is_implemented <reference, int &&, int>));
    BOOST_MPL_ASSERT ((is_implemented <reference, int &&, int &>));
    BOOST_MPL_ASSERT ((is_implemented <reference, int &&, int &&>));

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

    // const_ underlies reference.
    BOOST_MPL_ASSERT ((std::is_same <
        reference::apply <int &, int const>::type,
        int const>));

    // decay
    BOOST_MPL_ASSERT ((is_implemented <decay, int, int>));
    BOOST_MPL_ASSERT ((is_implemented <decay, int &, int>));
    BOOST_MPL_ASSERT ((is_implemented <decay, int, int &&>));
    BOOST_MPL_ASSERT ((is_implemented <decay, int const &, int &>));
    BOOST_MPL_ASSERT ((is_unimplemented <decay, int, long>));
    BOOST_MPL_ASSERT ((is_unimplemented <decay, int &, long &>));
    BOOST_MPL_ASSERT ((is_unimplemented <decay, int, long &>));

    // Forward to "same".
    BOOST_MPL_ASSERT ((std::is_same <
        decay::apply <int, int>::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <
        decay::apply <int const, int const>::type, int const>));
    BOOST_MPL_ASSERT ((std::is_same <
        decay::apply <int &&, int &&>::type, int &&>));
    // Decay and forward to "same".
    BOOST_MPL_ASSERT ((std::is_same <
        decay::apply <int &&, int &>::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <
        decay::apply <int const &, int>::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <
        decay::apply <int const &, int>::type, int>));

    // base_class.
    BOOST_MPL_ASSERT ((std::is_same <
        base_class::apply <int, int>::type, int>));
    BOOST_MPL_ASSERT ((is_unimplemented <base_class, int, long>));

    BOOST_MPL_ASSERT ((std::is_same <
        base_class::apply <base, base>::type, base>));
    BOOST_MPL_ASSERT ((std::is_same <
        base_class::apply <base, derived>::type, base>));
    BOOST_MPL_ASSERT ((std::is_same <
        base_class::apply <derived, base>::type, base>));
    BOOST_MPL_ASSERT ((std::is_same <
        base_class::apply <derived, derived>::type, derived>));

    // common_type.
    BOOST_MPL_ASSERT ((std::is_same <
        common_type::apply <int, int>::type, int>));
    BOOST_MPL_ASSERT ((std::is_same <
        common_type::apply <int, long>::type, long>));
    BOOST_MPL_ASSERT ((std::is_same <
        common_type::apply <unsigned char, unsigned>::type, unsigned>));
    BOOST_MPL_ASSERT ((std::is_same <
        common_type::apply <unsigned char, float>::type, float>));
}

BOOST_AUTO_TEST_CASE (test_merge_policy_collapse) {
    using rime::merge_policy::is_implemented;
    using rime::merge_policy::is_unimplemented;
    typedef rime::merge_policy::collapse collapse;

#define SHOULD_NOT_COLLAPSE(Type1, Type2) \
    BOOST_MPL_ASSERT_NOT ((is_implemented <\
        rime::merge_policy::collapse, Type1, Type2>))

#define SHOULD_COLLAPSE_TO(Type1, Type2, Result) \
    BOOST_MPL_ASSERT ((std::is_same < \
        collapse::apply <Type1, Type2>::type, Result>))

    // Check that no numeric conversions happen.
    SHOULD_NOT_COLLAPSE (int, long);
    SHOULD_NOT_COLLAPSE (int *, long *);
    SHOULD_NOT_COLLAPSE (int const &, long const &);

    // Check that no strange conversions happen.
    SHOULD_NOT_COLLAPSE (int, derived);
    SHOULD_NOT_COLLAPSE (derived, int);

    SHOULD_NOT_COLLAPSE (int &, derived &);
    SHOULD_NOT_COLLAPSE (derived &, int &);

    SHOULD_NOT_COLLAPSE (int const *, derived const *);
    SHOULD_NOT_COLLAPSE (derived const *, int const *);

    // Check all combinations of
    // int, int const, int &, int const &, int &&, int *, int const *.
    SHOULD_COLLAPSE_TO  (int        , int        , int);
    SHOULD_COLLAPSE_TO  (int        , int const  , int const);
    SHOULD_COLLAPSE_TO  (int        , int &      , int);
    SHOULD_COLLAPSE_TO  (int        , int const &, int);
    SHOULD_COLLAPSE_TO  (int        , int &&     , int);
    SHOULD_NOT_COLLAPSE (int        , int *);
    SHOULD_NOT_COLLAPSE (int        , int const *);

    SHOULD_COLLAPSE_TO  (int const  , int        , int const);
    SHOULD_COLLAPSE_TO  (int const  , int const  , int const);
    SHOULD_COLLAPSE_TO  (int const  , int &      , int const);
    SHOULD_COLLAPSE_TO  (int const  , int const &, int const);
    SHOULD_COLLAPSE_TO  (int const  , int &&     , int const);
    SHOULD_NOT_COLLAPSE (int const  , int *);
    SHOULD_NOT_COLLAPSE (int const  , int const *);

    SHOULD_COLLAPSE_TO  (int &      , int        , int);
    SHOULD_COLLAPSE_TO  (int &      , int const  , int const);
    SHOULD_COLLAPSE_TO  (int &      , int &      , int &);
    SHOULD_COLLAPSE_TO  (int &      , int const &, int const &);
    SHOULD_COLLAPSE_TO  (int &      , int &&     , int const &);
    SHOULD_NOT_COLLAPSE (int &      , int *);
    SHOULD_NOT_COLLAPSE (int &      , int const *);

    SHOULD_COLLAPSE_TO  (int const &, int        , int);
    SHOULD_COLLAPSE_TO  (int const &, int const  , int const);
    SHOULD_COLLAPSE_TO  (int const &, int &      , int const &);
    SHOULD_COLLAPSE_TO  (int const &, int const &, int const &);
    SHOULD_COLLAPSE_TO  (int const &, int &&     , int const &);
    SHOULD_NOT_COLLAPSE (int const &, int *);
    SHOULD_NOT_COLLAPSE (int const &, int const *);

    SHOULD_COLLAPSE_TO  (int &&     , int        , int);
    SHOULD_COLLAPSE_TO  (int &&     , int const  , int const);
    SHOULD_COLLAPSE_TO  (int &&     , int &      , int const &);
    SHOULD_COLLAPSE_TO  (int &&     , int const &, int const &);
    SHOULD_COLLAPSE_TO  (int &&     , int &&     , int &&);
    SHOULD_NOT_COLLAPSE (int &&     , int *);
    SHOULD_NOT_COLLAPSE (int &&     , int const *);

    SHOULD_NOT_COLLAPSE (int *      , int);
    SHOULD_NOT_COLLAPSE (int *      , int const);
    SHOULD_NOT_COLLAPSE (int *      , int &);
    SHOULD_NOT_COLLAPSE (int *      , int const &);
    SHOULD_NOT_COLLAPSE (int *      , int &&);
    SHOULD_COLLAPSE_TO  (int *      , int *, int *);
    SHOULD_COLLAPSE_TO  (int *      , int const *, int const *);

    SHOULD_NOT_COLLAPSE (int const *, int);
    SHOULD_NOT_COLLAPSE (int const *, int const);
    SHOULD_NOT_COLLAPSE (int const *, int &);
    SHOULD_NOT_COLLAPSE (int const *, int const &);
    SHOULD_NOT_COLLAPSE (int const *, int &&);
    SHOULD_COLLAPSE_TO  (int const *, int *, int const *);
    SHOULD_COLLAPSE_TO  (int const *, int const *, int const *);

    // Check all combinations of
    // T, T const, T &, T const &, T &&, T *, T const * for base and derived.

    // base and base.
    SHOULD_COLLAPSE_TO  (base        , base        , base);
    SHOULD_COLLAPSE_TO  (base        , base const  , base const);
    SHOULD_COLLAPSE_TO  (base        , base &      , base);
    SHOULD_COLLAPSE_TO  (base        , base const &, base);
    SHOULD_COLLAPSE_TO  (base        , base &&     , base);
    SHOULD_NOT_COLLAPSE (base        , base *);
    SHOULD_NOT_COLLAPSE (base        , base const *);

    SHOULD_COLLAPSE_TO  (base const  , base        , base const);
    SHOULD_COLLAPSE_TO  (base const  , base const  , base const);
    SHOULD_COLLAPSE_TO  (base const  , base &      , base const);
    SHOULD_COLLAPSE_TO  (base const  , base const &, base const);
    SHOULD_COLLAPSE_TO  (base const  , base &&     , base const);
    SHOULD_NOT_COLLAPSE (base const  , base *);
    SHOULD_NOT_COLLAPSE (base const  , base const *);

    SHOULD_COLLAPSE_TO  (base &      , base        , base);
    SHOULD_COLLAPSE_TO  (base &      , base const  , base const);
    SHOULD_COLLAPSE_TO  (base &      , base &      , base &);
    SHOULD_COLLAPSE_TO  (base &      , base const &, base const &);
    SHOULD_COLLAPSE_TO  (base &      , base &&     , base const &);
    SHOULD_NOT_COLLAPSE (base &      , base *);
    SHOULD_NOT_COLLAPSE (base &      , base const *);

    SHOULD_COLLAPSE_TO  (base const &, base        , base);
    SHOULD_COLLAPSE_TO  (base const &, base const  , base const);
    SHOULD_COLLAPSE_TO  (base const &, base &      , base const &);
    SHOULD_COLLAPSE_TO  (base const &, base const &, base const &);
    SHOULD_COLLAPSE_TO  (base const &, base &&     , base const &);
    SHOULD_NOT_COLLAPSE (base const &, base *);
    SHOULD_NOT_COLLAPSE (base const &, base const *);

    SHOULD_COLLAPSE_TO  (base &&     , base        , base);
    SHOULD_COLLAPSE_TO  (base &&     , base const  , base const);
    SHOULD_COLLAPSE_TO  (base &&     , base &      , base const &);
    SHOULD_COLLAPSE_TO  (base &&     , base const &, base const &);
    SHOULD_COLLAPSE_TO  (base &&     , base &&     , base &&);
    SHOULD_NOT_COLLAPSE (base &&     , base *);
    SHOULD_NOT_COLLAPSE (base &&     , base const *);

    SHOULD_NOT_COLLAPSE (base *      , base);
    SHOULD_NOT_COLLAPSE (base *      , base const);
    SHOULD_NOT_COLLAPSE (base *      , base &);
    SHOULD_NOT_COLLAPSE (base *      , base const &);
    SHOULD_NOT_COLLAPSE (base *      , base &&);
    SHOULD_COLLAPSE_TO  (base *      , base *, base *);
    SHOULD_COLLAPSE_TO  (base *      , base const *, base const *);

    SHOULD_NOT_COLLAPSE (base const *, base);
    SHOULD_NOT_COLLAPSE (base const *, base const);
    SHOULD_NOT_COLLAPSE (base const *, base &);
    SHOULD_NOT_COLLAPSE (base const *, base const &);
    SHOULD_NOT_COLLAPSE (base const *, base &&);
    SHOULD_COLLAPSE_TO  (base const *, base *, base const *);
    SHOULD_COLLAPSE_TO  (base const *, base const *, base const *);

    // derived and base.
    SHOULD_NOT_COLLAPSE (derived        , base);
    SHOULD_NOT_COLLAPSE (derived        , base const);
    SHOULD_NOT_COLLAPSE (derived        , base &);
    SHOULD_NOT_COLLAPSE (derived        , base);
    SHOULD_NOT_COLLAPSE (derived        , base &&);
    SHOULD_NOT_COLLAPSE (derived        , base *);
    SHOULD_NOT_COLLAPSE (derived        , base const *);

    SHOULD_NOT_COLLAPSE (derived const  , base);
    SHOULD_NOT_COLLAPSE (derived const  , base const);
    SHOULD_NOT_COLLAPSE (derived const  , base &);
    SHOULD_NOT_COLLAPSE (derived const  , base);
    SHOULD_NOT_COLLAPSE (derived const  , base &&);
    SHOULD_NOT_COLLAPSE (derived const  , base *);
    SHOULD_NOT_COLLAPSE (derived const  , base const *);

    SHOULD_NOT_COLLAPSE (derived &      , base);
    SHOULD_NOT_COLLAPSE (derived &      , base const);
    SHOULD_COLLAPSE_TO  (derived &      , base &      , base &);
    SHOULD_COLLAPSE_TO  (derived &      , base const &, base const &);
    SHOULD_COLLAPSE_TO  (derived &      , base &&     , base const &);
    SHOULD_NOT_COLLAPSE (derived &      , base *);
    SHOULD_NOT_COLLAPSE (derived &      , base const *);

    SHOULD_NOT_COLLAPSE (derived const &, base);
    SHOULD_NOT_COLLAPSE (derived const &, base const);
    SHOULD_COLLAPSE_TO  (derived const &, base &      , base const &);
    SHOULD_COLLAPSE_TO  (derived const &, base const &, base const &);
    SHOULD_COLLAPSE_TO  (derived const &, base &&     , base const &);
    SHOULD_NOT_COLLAPSE (derived const &, base *);
    SHOULD_NOT_COLLAPSE (derived const &, base const *);

    SHOULD_NOT_COLLAPSE (derived &&     , base);
    SHOULD_NOT_COLLAPSE (derived &&     , base const);
    SHOULD_COLLAPSE_TO  (derived &&     , base &      , base const &);
    SHOULD_COLLAPSE_TO  (derived &&     , base const &, base const &);
    SHOULD_COLLAPSE_TO  (derived &&     , base &&     , base &&);
    SHOULD_NOT_COLLAPSE (derived &&     , base *);
    SHOULD_NOT_COLLAPSE (derived &&     , base const *);

    SHOULD_NOT_COLLAPSE (derived *      , base);
    SHOULD_NOT_COLLAPSE (derived *      , base const);
    SHOULD_NOT_COLLAPSE (derived *      , base &);
    SHOULD_NOT_COLLAPSE (derived *      , base const &);
    SHOULD_NOT_COLLAPSE (derived *      , base &&);
    SHOULD_COLLAPSE_TO  (derived *      , base *, base *);
    SHOULD_COLLAPSE_TO  (derived *      , base const *, base const *);

    SHOULD_NOT_COLLAPSE (derived const *, base);
    SHOULD_NOT_COLLAPSE (derived const *, base const);
    SHOULD_NOT_COLLAPSE (derived const *, base &);
    SHOULD_NOT_COLLAPSE (derived const *, base const &);
    SHOULD_NOT_COLLAPSE (derived const *, base &&);
    SHOULD_COLLAPSE_TO  (derived const *, base *, base const *);
    SHOULD_COLLAPSE_TO  (derived const *, base const *, base const *);

    // base and derived.
    SHOULD_NOT_COLLAPSE (base        , derived);
    SHOULD_NOT_COLLAPSE (base        , derived const);
    SHOULD_NOT_COLLAPSE (base        , derived &);
    SHOULD_NOT_COLLAPSE (base        , derived);
    SHOULD_NOT_COLLAPSE (base        , derived &&);
    SHOULD_NOT_COLLAPSE (base        , derived *);
    SHOULD_NOT_COLLAPSE (base        , derived const *);

    SHOULD_NOT_COLLAPSE (base const  , derived);
    SHOULD_NOT_COLLAPSE (base const  , derived const);
    SHOULD_NOT_COLLAPSE (base const  , derived &);
    SHOULD_NOT_COLLAPSE (base const  , derived);
    SHOULD_NOT_COLLAPSE (base const  , derived &&);
    SHOULD_NOT_COLLAPSE (base const  , derived *);
    SHOULD_NOT_COLLAPSE (base const  , derived const *);

    SHOULD_NOT_COLLAPSE (base &      , derived);
    SHOULD_NOT_COLLAPSE (base &      , derived const);
    SHOULD_COLLAPSE_TO  (base &      , derived &      , base &);
    SHOULD_COLLAPSE_TO  (base &      , derived const &, base const &);
    SHOULD_COLLAPSE_TO  (base &      , derived &&     , base const &);
    SHOULD_NOT_COLLAPSE (base &      , derived *);
    SHOULD_NOT_COLLAPSE (base &      , derived const *);

    SHOULD_NOT_COLLAPSE (base const &, derived);
    SHOULD_NOT_COLLAPSE (base const &, derived const);
    SHOULD_COLLAPSE_TO  (base const &, derived &      , base const &);
    SHOULD_COLLAPSE_TO  (base const &, derived const &, base const &);
    SHOULD_COLLAPSE_TO  (base const &, derived &&     , base const &);
    SHOULD_NOT_COLLAPSE (base const &, derived *);
    SHOULD_NOT_COLLAPSE (base const &, derived const *);

    SHOULD_NOT_COLLAPSE (base &&     , derived);
    SHOULD_NOT_COLLAPSE (base &&     , derived const);
    SHOULD_COLLAPSE_TO  (base &&     , derived &      , base const &);
    SHOULD_COLLAPSE_TO  (base &&     , derived const &, base const &);
    SHOULD_COLLAPSE_TO  (base &&     , derived &&     , base &&);
    SHOULD_NOT_COLLAPSE (base &&     , derived *);
    SHOULD_NOT_COLLAPSE (base &&     , derived const *);

    SHOULD_NOT_COLLAPSE (base *      , derived);
    SHOULD_NOT_COLLAPSE (base *      , derived const);
    SHOULD_NOT_COLLAPSE (base *      , derived &);
    SHOULD_NOT_COLLAPSE (base *      , derived const &);
    SHOULD_NOT_COLLAPSE (base *      , derived &&);
    SHOULD_COLLAPSE_TO  (base *      , derived *, base *);
    SHOULD_COLLAPSE_TO  (base *      , derived const *, base const *);

    SHOULD_NOT_COLLAPSE (base const *, derived);
    SHOULD_NOT_COLLAPSE (base const *, derived const);
    SHOULD_NOT_COLLAPSE (base const *, derived &);
    SHOULD_NOT_COLLAPSE (base const *, derived const &);
    SHOULD_NOT_COLLAPSE (base const *, derived &&);
    SHOULD_COLLAPSE_TO  (base const *, derived *, base const *);
    SHOULD_COLLAPSE_TO  (base const *, derived const *, base const *);

#undef SHOULD_NOT_COLLAPSE
#undef SHOULD_COLLAPSE_TO
}

BOOST_AUTO_TEST_CASE (test_merge_types_internal_insert) {
    using rime::merge_detail::insert;
    using meta::vector;
    typedef rime::merge_policy::reference<> merge_two;

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
    using rime::merge_types;
    using meta::vector;
    typedef rime::merge_policy::reference<> merge_two;

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
    using rime::merge_types;
    using meta::vector;
    typedef rime::merge_policy::reference <
        rime::merge_policy::const_ <
            merge_constant <rime::merge_policy::same<> > > > merge_two;

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
