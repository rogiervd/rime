// SPDX-FileCopyrightText: Copyright 2011, 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_rime_detail_generalise_type
#include "boost/test/unit_test.hpp"

#include "boost/mpl/assert.hpp"
#include "meta/equal.hpp"

#include "rime/detail/generalise_type.hpp"

#include "meta/vector.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_detail_generalise_type)

template <int N> struct int_;

struct degenerate
{
    template <typename Type> struct apply
    {
        typedef Type type;
    };

    template <typename Type> struct apply<Type &>
    {
        typedef Type type;
    };

    template <typename Type> struct apply<Type const>
    {
        typedef Type type;
    };

    template <int N> struct apply<int_<N>>
    {
        typedef int const type;
    };
};

BOOST_AUTO_TEST_CASE(test_rime_detail_generalise_type)
{
    BOOST_MPL_ASSERT((meta::equal<
                      rime::detail::generalise_type<int, degenerate>,
                      meta::vector<int>>) );

    BOOST_MPL_ASSERT((meta::equal<
                      rime::detail::generalise_type<int const, degenerate>,
                      meta::vector<int const, int>>) );

    BOOST_MPL_ASSERT((meta::equal<
                      rime::detail::generalise_type<int const &, degenerate>,
                      meta::vector<int const &, int const, int>>) );

    BOOST_MPL_ASSERT((meta::equal<
                      rime::detail::generalise_type<int_<4>, degenerate>,
                      meta::vector<int_<4>, int const, int>>) );
}

BOOST_AUTO_TEST_SUITE_END()
