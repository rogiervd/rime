// SPDX-FileCopyrightText: Copyright 2011, 2012, 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_rime_detail_merge_types
#include "boost/test/unit_test.hpp"

#include <type_traits>

#include "meta/vector.hpp"

#include "rime/merge_types.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_detail_merge_types)

struct base {};
struct derived : base {};

// Simple constant class to check merging
template <class Type, Type N> struct constant
{
    operator Type() const { return N; }
};

template <class Base> struct merge_constant
{
    template <class Type1, class Type2> struct apply
    : Base::template apply<Type1, Type2> {};

    template <class Type1, Type1 N1, class Type2>
    struct apply<constant<Type1, N1>, Type2>
    : Base::template apply<Type1, Type2> {};

    template <class Type1, class Type2, Type2 N2>
    struct apply<Type1, constant<Type2, N2>>
    : Base::template apply<Type1, Type2> {};

    template <class Type1, Type1 N1, class Type2, Type2 N2>
    struct apply<constant<Type1, N1>, constant<Type2, N2>>
    : Base::template apply<Type1, Type2> {};
};

BOOST_AUTO_TEST_CASE(test_merge_types_merge_policy)
{
    using rime::merge_policy::is_implemented;
    using rime::merge_policy::is_unimplemented;
    typedef rime::merge_policy::same<> same;
    typedef rime::merge_policy::const_<> const_;
    typedef rime::merge_policy::reference<> reference;
    typedef rime::merge_policy::decay<> decay;
    using rime::merge_policy::base_class;
    using rime::merge_policy::common_type;

    // same
    static_assert(is_implemented<same, int, int>::value);
    static_assert(is_implemented<same, long, long>::value);
    static_assert(is_unimplemented<same, int, long>::value);

    static_assert(std::is_same_v<same::apply<int, int>::type, int>);

    static_assert(std::is_same_v<same::apply<long, long>::type, long>);

    // const
    static_assert(is_implemented<const_, int, int>::value);
    static_assert(is_implemented<const_, int const, int const>::value);
    static_assert(is_implemented<const_, int const, int>::value);
    static_assert(is_implemented<const_, int, int const>::value);
    static_assert(is_unimplemented<const_, int, long>::value);
    static_assert(is_unimplemented<const_, int, long const>::value);

    static_assert(std::is_same_v<const_::apply<int, int>::type, int>);

    static_assert(
        std::is_same_v<const_::apply<int const, int>::type, int const>);

    static_assert(
        std::is_same_v<const_::apply<int const, int const>::type, int const>);

    // reference
    static_assert(is_implemented<reference, int, int>::value);
    static_assert(is_implemented<reference, int, int &>::value);
    static_assert(is_implemented<reference, int, int &&>::value);
    static_assert(is_implemented<reference, int &, int>::value);
    static_assert(is_implemented<reference, int &, int &>::value);
    static_assert(is_implemented<reference, int &, int &&>::value);
    static_assert(is_implemented<reference, int &&, int>::value);
    static_assert(is_implemented<reference, int &&, int &>::value);
    static_assert(is_implemented<reference, int &&, int &&>::value);

    static_assert(is_unimplemented<reference, int &, long &>::value);
    static_assert(is_unimplemented<reference, int, long &>::value);

    static_assert(std::is_same_v<reference::apply<int, int>::type, int>);

    static_assert(std::is_same_v<reference::apply<int &, int>::type, int>);

    static_assert(std::is_same_v<reference::apply<int &, int &>::type, int &>);

    // const_ underlies reference.
    static_assert(
        std::is_same_v<reference::apply<int &, int const>::type, int const>);

    // decay
    static_assert(is_implemented<decay, int, int>::value);
    static_assert(is_implemented<decay, int &, int>::value);
    static_assert(is_implemented<decay, int, int &&>::value);
    static_assert(is_implemented<decay, int const &, int &>::value);
    static_assert(is_unimplemented<decay, int, long>::value);
    static_assert(is_unimplemented<decay, int &, long &>::value);
    static_assert(is_unimplemented<decay, int, long &>::value);

    // Forward to "same".
    static_assert(std::is_same_v<decay::apply<int, int>::type, int>);
    static_assert(
        std::is_same_v<decay::apply<int const, int const>::type, int const>);
    static_assert(std::is_same_v<decay::apply<int &&, int &&>::type, int &&>);
    // Decay and forward to "same".
    static_assert(std::is_same_v<decay::apply<int &&, int &>::type, int>);
    static_assert(std::is_same_v<decay::apply<int const &, int>::type, int>);
    static_assert(std::is_same_v<decay::apply<int const &, int>::type, int>);

    // base_class.
    static_assert(std::is_same_v<base_class::apply<int, int>::type, int>);
    static_assert(is_unimplemented<base_class, int, long>::value);

    static_assert(std::is_same_v<base_class::apply<base, base>::type, base>);
    static_assert(std::is_same_v<base_class::apply<base, derived>::type, base>);
    static_assert(std::is_same_v<base_class::apply<derived, base>::type, base>);
    static_assert(
        std::is_same_v<base_class::apply<derived, derived>::type, derived>);

    // common_type.
    static_assert(std::is_same_v<common_type::apply<int, int>::type, int>);
    static_assert(std::is_same_v<common_type::apply<int, long>::type, long>);
    static_assert(std::is_same_v<
                  common_type::apply<unsigned char, unsigned>::type, unsigned>);
    static_assert(
        std::is_same_v<common_type::apply<unsigned char, float>::type, float>);
}

BOOST_AUTO_TEST_CASE(test_merge_policy_collapse)
{
    using rime::merge_policy::is_implemented;
    using rime::merge_policy::is_unimplemented;
    typedef rime::merge_policy::collapse collapse;

#define SHOULD_NOT_COLLAPSE(Type1, Type2) \
    BOOST_MPL_ASSERT_NOT(                 \
        (is_implemented<rime::merge_policy::collapse, Type1, Type2>) )

#define SHOULD_COLLAPSE_TO(Type1, Type2, Result) \
    BOOST_MPL_ASSERT(                            \
        (std::is_same<collapse::apply<Type1, Type2>::type, Result>) )

    // Check that no numeric conversions happen.
    SHOULD_NOT_COLLAPSE(int, long);
    SHOULD_NOT_COLLAPSE(int *, long *);
    SHOULD_NOT_COLLAPSE(int const &, long const &);

    // Check that no strange conversions happen.
    SHOULD_NOT_COLLAPSE(int, derived);
    SHOULD_NOT_COLLAPSE(derived, int);

    SHOULD_NOT_COLLAPSE(int &, derived &);
    SHOULD_NOT_COLLAPSE(derived &, int &);

    SHOULD_NOT_COLLAPSE(int const *, derived const *);
    SHOULD_NOT_COLLAPSE(derived const *, int const *);

    // Check all combinations of
    // int, int const, int &, int const &, int &&, int *, int const *.
    SHOULD_COLLAPSE_TO(int, int, int);
    SHOULD_COLLAPSE_TO(int, int const, int const);
    SHOULD_COLLAPSE_TO(int, int &, int);
    SHOULD_COLLAPSE_TO(int, int const &, int);
    SHOULD_COLLAPSE_TO(int, int &&, int);
    SHOULD_NOT_COLLAPSE(int, int *);
    SHOULD_NOT_COLLAPSE(int, int const *);

    SHOULD_COLLAPSE_TO(int const, int, int const);
    SHOULD_COLLAPSE_TO(int const, int const, int const);
    SHOULD_COLLAPSE_TO(int const, int &, int const);
    SHOULD_COLLAPSE_TO(int const, int const &, int const);
    SHOULD_COLLAPSE_TO(int const, int &&, int const);
    SHOULD_NOT_COLLAPSE(int const, int *);
    SHOULD_NOT_COLLAPSE(int const, int const *);

    SHOULD_COLLAPSE_TO(int &, int, int);
    SHOULD_COLLAPSE_TO(int &, int const, int const);
    SHOULD_COLLAPSE_TO(int &, int &, int &);
    SHOULD_COLLAPSE_TO(int &, int const &, int const &);
    SHOULD_COLLAPSE_TO(int &, int &&, int const &);
    SHOULD_NOT_COLLAPSE(int &, int *);
    SHOULD_NOT_COLLAPSE(int &, int const *);

    SHOULD_COLLAPSE_TO(int const &, int, int);
    SHOULD_COLLAPSE_TO(int const &, int const, int const);
    SHOULD_COLLAPSE_TO(int const &, int &, int const &);
    SHOULD_COLLAPSE_TO(int const &, int const &, int const &);
    SHOULD_COLLAPSE_TO(int const &, int &&, int const &);
    SHOULD_NOT_COLLAPSE(int const &, int *);
    SHOULD_NOT_COLLAPSE(int const &, int const *);

    SHOULD_COLLAPSE_TO(int &&, int, int);
    SHOULD_COLLAPSE_TO(int &&, int const, int const);
    SHOULD_COLLAPSE_TO(int &&, int &, int const &);
    SHOULD_COLLAPSE_TO(int &&, int const &, int const &);
    SHOULD_COLLAPSE_TO(int &&, int &&, int &&);
    SHOULD_NOT_COLLAPSE(int &&, int *);
    SHOULD_NOT_COLLAPSE(int &&, int const *);

    SHOULD_NOT_COLLAPSE(int *, int);
    SHOULD_NOT_COLLAPSE(int *, int const);
    SHOULD_NOT_COLLAPSE(int *, int &);
    SHOULD_NOT_COLLAPSE(int *, int const &);
    SHOULD_NOT_COLLAPSE(int *, int &&);
    SHOULD_COLLAPSE_TO(int *, int *, int *);
    SHOULD_COLLAPSE_TO(int *, int const *, int const *);

    SHOULD_NOT_COLLAPSE(int const *, int);
    SHOULD_NOT_COLLAPSE(int const *, int const);
    SHOULD_NOT_COLLAPSE(int const *, int &);
    SHOULD_NOT_COLLAPSE(int const *, int const &);
    SHOULD_NOT_COLLAPSE(int const *, int &&);
    SHOULD_COLLAPSE_TO(int const *, int *, int const *);
    SHOULD_COLLAPSE_TO(int const *, int const *, int const *);

    // Check all combinations of
    // T, T const, T &, T const &, T &&, T *, T const * for base and derived.

    // base and base.
    SHOULD_COLLAPSE_TO(base, base, base);
    SHOULD_COLLAPSE_TO(base, base const, base const);
    SHOULD_COLLAPSE_TO(base, base &, base);
    SHOULD_COLLAPSE_TO(base, base const &, base);
    SHOULD_COLLAPSE_TO(base, base &&, base);
    SHOULD_NOT_COLLAPSE(base, base *);
    SHOULD_NOT_COLLAPSE(base, base const *);

    SHOULD_COLLAPSE_TO(base const, base, base const);
    SHOULD_COLLAPSE_TO(base const, base const, base const);
    SHOULD_COLLAPSE_TO(base const, base &, base const);
    SHOULD_COLLAPSE_TO(base const, base const &, base const);
    SHOULD_COLLAPSE_TO(base const, base &&, base const);
    SHOULD_NOT_COLLAPSE(base const, base *);
    SHOULD_NOT_COLLAPSE(base const, base const *);

    SHOULD_COLLAPSE_TO(base &, base, base);
    SHOULD_COLLAPSE_TO(base &, base const, base const);
    SHOULD_COLLAPSE_TO(base &, base &, base &);
    SHOULD_COLLAPSE_TO(base &, base const &, base const &);
    SHOULD_COLLAPSE_TO(base &, base &&, base const &);
    SHOULD_NOT_COLLAPSE(base &, base *);
    SHOULD_NOT_COLLAPSE(base &, base const *);

    SHOULD_COLLAPSE_TO(base const &, base, base);
    SHOULD_COLLAPSE_TO(base const &, base const, base const);
    SHOULD_COLLAPSE_TO(base const &, base &, base const &);
    SHOULD_COLLAPSE_TO(base const &, base const &, base const &);
    SHOULD_COLLAPSE_TO(base const &, base &&, base const &);
    SHOULD_NOT_COLLAPSE(base const &, base *);
    SHOULD_NOT_COLLAPSE(base const &, base const *);

    SHOULD_COLLAPSE_TO(base &&, base, base);
    SHOULD_COLLAPSE_TO(base &&, base const, base const);
    SHOULD_COLLAPSE_TO(base &&, base &, base const &);
    SHOULD_COLLAPSE_TO(base &&, base const &, base const &);
    SHOULD_COLLAPSE_TO(base &&, base &&, base &&);
    SHOULD_NOT_COLLAPSE(base &&, base *);
    SHOULD_NOT_COLLAPSE(base &&, base const *);

    SHOULD_NOT_COLLAPSE(base *, base);
    SHOULD_NOT_COLLAPSE(base *, base const);
    SHOULD_NOT_COLLAPSE(base *, base &);
    SHOULD_NOT_COLLAPSE(base *, base const &);
    SHOULD_NOT_COLLAPSE(base *, base &&);
    SHOULD_COLLAPSE_TO(base *, base *, base *);
    SHOULD_COLLAPSE_TO(base *, base const *, base const *);

    SHOULD_NOT_COLLAPSE(base const *, base);
    SHOULD_NOT_COLLAPSE(base const *, base const);
    SHOULD_NOT_COLLAPSE(base const *, base &);
    SHOULD_NOT_COLLAPSE(base const *, base const &);
    SHOULD_NOT_COLLAPSE(base const *, base &&);
    SHOULD_COLLAPSE_TO(base const *, base *, base const *);
    SHOULD_COLLAPSE_TO(base const *, base const *, base const *);

    // derived and base.
    SHOULD_NOT_COLLAPSE(derived, base);
    SHOULD_NOT_COLLAPSE(derived, base const);
    SHOULD_NOT_COLLAPSE(derived, base &);
    SHOULD_NOT_COLLAPSE(derived, base);
    SHOULD_NOT_COLLAPSE(derived, base &&);
    SHOULD_NOT_COLLAPSE(derived, base *);
    SHOULD_NOT_COLLAPSE(derived, base const *);

    SHOULD_NOT_COLLAPSE(derived const, base);
    SHOULD_NOT_COLLAPSE(derived const, base const);
    SHOULD_NOT_COLLAPSE(derived const, base &);
    SHOULD_NOT_COLLAPSE(derived const, base);
    SHOULD_NOT_COLLAPSE(derived const, base &&);
    SHOULD_NOT_COLLAPSE(derived const, base *);
    SHOULD_NOT_COLLAPSE(derived const, base const *);

    SHOULD_NOT_COLLAPSE(derived &, base);
    SHOULD_NOT_COLLAPSE(derived &, base const);
    SHOULD_COLLAPSE_TO(derived &, base &, base &);
    SHOULD_COLLAPSE_TO(derived &, base const &, base const &);
    SHOULD_COLLAPSE_TO(derived &, base &&, base const &);
    SHOULD_NOT_COLLAPSE(derived &, base *);
    SHOULD_NOT_COLLAPSE(derived &, base const *);

    SHOULD_NOT_COLLAPSE(derived const &, base);
    SHOULD_NOT_COLLAPSE(derived const &, base const);
    SHOULD_COLLAPSE_TO(derived const &, base &, base const &);
    SHOULD_COLLAPSE_TO(derived const &, base const &, base const &);
    SHOULD_COLLAPSE_TO(derived const &, base &&, base const &);
    SHOULD_NOT_COLLAPSE(derived const &, base *);
    SHOULD_NOT_COLLAPSE(derived const &, base const *);

    SHOULD_NOT_COLLAPSE(derived &&, base);
    SHOULD_NOT_COLLAPSE(derived &&, base const);
    SHOULD_COLLAPSE_TO(derived &&, base &, base const &);
    SHOULD_COLLAPSE_TO(derived &&, base const &, base const &);
    SHOULD_COLLAPSE_TO(derived &&, base &&, base &&);
    SHOULD_NOT_COLLAPSE(derived &&, base *);
    SHOULD_NOT_COLLAPSE(derived &&, base const *);

    SHOULD_NOT_COLLAPSE(derived *, base);
    SHOULD_NOT_COLLAPSE(derived *, base const);
    SHOULD_NOT_COLLAPSE(derived *, base &);
    SHOULD_NOT_COLLAPSE(derived *, base const &);
    SHOULD_NOT_COLLAPSE(derived *, base &&);
    SHOULD_COLLAPSE_TO(derived *, base *, base *);
    SHOULD_COLLAPSE_TO(derived *, base const *, base const *);

    SHOULD_NOT_COLLAPSE(derived const *, base);
    SHOULD_NOT_COLLAPSE(derived const *, base const);
    SHOULD_NOT_COLLAPSE(derived const *, base &);
    SHOULD_NOT_COLLAPSE(derived const *, base const &);
    SHOULD_NOT_COLLAPSE(derived const *, base &&);
    SHOULD_COLLAPSE_TO(derived const *, base *, base const *);
    SHOULD_COLLAPSE_TO(derived const *, base const *, base const *);

    // base and derived.
    SHOULD_NOT_COLLAPSE(base, derived);
    SHOULD_NOT_COLLAPSE(base, derived const);
    SHOULD_NOT_COLLAPSE(base, derived &);
    SHOULD_NOT_COLLAPSE(base, derived);
    SHOULD_NOT_COLLAPSE(base, derived &&);
    SHOULD_NOT_COLLAPSE(base, derived *);
    SHOULD_NOT_COLLAPSE(base, derived const *);

    SHOULD_NOT_COLLAPSE(base const, derived);
    SHOULD_NOT_COLLAPSE(base const, derived const);
    SHOULD_NOT_COLLAPSE(base const, derived &);
    SHOULD_NOT_COLLAPSE(base const, derived);
    SHOULD_NOT_COLLAPSE(base const, derived &&);
    SHOULD_NOT_COLLAPSE(base const, derived *);
    SHOULD_NOT_COLLAPSE(base const, derived const *);

    SHOULD_NOT_COLLAPSE(base &, derived);
    SHOULD_NOT_COLLAPSE(base &, derived const);
    SHOULD_COLLAPSE_TO(base &, derived &, base &);
    SHOULD_COLLAPSE_TO(base &, derived const &, base const &);
    SHOULD_COLLAPSE_TO(base &, derived &&, base const &);
    SHOULD_NOT_COLLAPSE(base &, derived *);
    SHOULD_NOT_COLLAPSE(base &, derived const *);

    SHOULD_NOT_COLLAPSE(base const &, derived);
    SHOULD_NOT_COLLAPSE(base const &, derived const);
    SHOULD_COLLAPSE_TO(base const &, derived &, base const &);
    SHOULD_COLLAPSE_TO(base const &, derived const &, base const &);
    SHOULD_COLLAPSE_TO(base const &, derived &&, base const &);
    SHOULD_NOT_COLLAPSE(base const &, derived *);
    SHOULD_NOT_COLLAPSE(base const &, derived const *);

    SHOULD_NOT_COLLAPSE(base &&, derived);
    SHOULD_NOT_COLLAPSE(base &&, derived const);
    SHOULD_COLLAPSE_TO(base &&, derived &, base const &);
    SHOULD_COLLAPSE_TO(base &&, derived const &, base const &);
    SHOULD_COLLAPSE_TO(base &&, derived &&, base &&);
    SHOULD_NOT_COLLAPSE(base &&, derived *);
    SHOULD_NOT_COLLAPSE(base &&, derived const *);

    SHOULD_NOT_COLLAPSE(base *, derived);
    SHOULD_NOT_COLLAPSE(base *, derived const);
    SHOULD_NOT_COLLAPSE(base *, derived &);
    SHOULD_NOT_COLLAPSE(base *, derived const &);
    SHOULD_NOT_COLLAPSE(base *, derived &&);
    SHOULD_COLLAPSE_TO(base *, derived *, base *);
    SHOULD_COLLAPSE_TO(base *, derived const *, base const *);

    SHOULD_NOT_COLLAPSE(base const *, derived);
    SHOULD_NOT_COLLAPSE(base const *, derived const);
    SHOULD_NOT_COLLAPSE(base const *, derived &);
    SHOULD_NOT_COLLAPSE(base const *, derived const &);
    SHOULD_NOT_COLLAPSE(base const *, derived &&);
    SHOULD_COLLAPSE_TO(base const *, derived *, base const *);
    SHOULD_COLLAPSE_TO(base const *, derived const *, base const *);

#undef SHOULD_NOT_COLLAPSE
#undef SHOULD_COLLAPSE_TO
}

BOOST_AUTO_TEST_CASE(test_merge_types_internal_insert)
{
    using meta::vector;
    using rime::merge_detail::insert;
    typedef rime::merge_policy::reference<> merge_two;

    static_assert(
        std::is_same_v<insert<merge_two, int, vector<>>::type, vector<int>>);

    static_assert(
        std::is_same_v<insert<merge_two, int, vector<int>>::type, vector<int>>);

    static_assert(
        std::is_same_v<
            insert<merge_two, int, vector<long>>::type, vector<int, long>>);

    static_assert(std::is_same_v<
                  insert<merge_two, int const, vector<int &>>::type,
                  vector<int const>>);

    static_assert(std::is_same_v<
                  insert<merge_two, int const, vector<char, int &>>::type,
                  vector<int const, char>>);

    static_assert(std::is_same_v<
                  insert<merge_two, int const, vector<int &, char>>::type,
                  vector<int const, char>>);
}

BOOST_AUTO_TEST_CASE(test_merge_types_reference)
{
    using meta::vector;
    using rime::merge_types;
    typedef rime::merge_policy::reference<> merge_two;

    {
        static_assert(
            std::is_same_v<merge_types<merge_two, vector<>>::type, vector<>>);
    }

    {
        static_assert(std::is_same_v<
                      merge_types<
                          merge_two,
                          vector<int, int, long &, int const, int &>>::type,
                      vector<int const, long &>>);
    }

    {
        static_assert(std::is_same_v<
                      merge_types<merge_two, vector<int>>::type, vector<int>>);
    }

    {
        static_assert(std::is_same_v<
                      merge_types<merge_two, vector<int, long>>::type,
                      vector<int, long>>);
    }

    {
        static_assert(std::is_same_v<
                      merge_types<merge_two, vector<int &, long>>::type,
                      vector<int &, long>>);
    }

    {
        static_assert(std::is_same_v<
                      merge_types<merge_two, vector<int const &, int>>::type,
                      vector<int const>>);
    }

    {
        static_assert(
            std::is_same_v<
                merge_types<merge_two, vector<long, int const &, int>>::type,
                vector<long, int const>>);
    }

    {
        static_assert(
            std::is_same_v<
                merge_types<merge_two, vector<int const &, int, long>>::type,
                vector<int const, long>>);
    }

    {
        static_assert(std::is_same_v<
                      merge_types<
                          merge_two,
                          vector<int, int, long &, int const, int &>>::type,
                      vector<int const, long &>>);
    }
}

BOOST_AUTO_TEST_CASE(test_merge_types_full)
{
    using meta::vector;
    using rime::merge_types;
    typedef rime::merge_policy::reference<
        rime::merge_policy::const_<merge_constant<rime::merge_policy::same<>>>>
        merge_two;

    {
        static_assert(std::is_same_v<
                      merge_types<
                          merge_two,
                          vector<int, int, long &, int const, int &>>::type,
                      vector<int const, long &>>);
    }
    {
        static_assert(
            std::is_same_v<
                merge_types<merge_two, vector<int, constant<int, 3>>>::type,
                vector<int>>);
    }
    {
        static_assert(
            std::is_same_v<
                merge_types<
                    merge_two, vector<int const &, constant<int, 3>>>::type,
                vector<int const>>);
    }
}

BOOST_AUTO_TEST_SUITE_END()
