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

#define BOOST_TEST_MODULE test_rime_detail_generalise_type
#include "../boost_unit_test.hpp"

#include <boost/mpl/assert.hpp>
#include "meta/equal.hpp"

#include "rime/detail/generalise_type.hpp"

#include "meta/vector.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_detail_generalise_type)

template <int N> struct int_;

struct degenerate
{
    template <typename Type>
        struct apply
    {
        typedef Type type;
    };

    template <typename Type>
        struct apply <Type &>
    {
        typedef Type type;
    };

    template <typename Type>
        struct apply <Type const>
    {
        typedef Type type;
    };

    template <int N>
        struct apply <int_ <N> >
    {
        typedef int const type;
    };
};

BOOST_AUTO_TEST_CASE (test_rime_detail_generalise_type) {
    BOOST_MPL_ASSERT ((meta::equal <
        rime::detail::generalise_type <int, degenerate>,
        meta::vector <int> >));

    BOOST_MPL_ASSERT ((meta::equal <
        rime::detail::generalise_type <const int, degenerate>,
        meta::vector <const int, int> >));

    BOOST_MPL_ASSERT ((meta::equal <
        rime::detail::generalise_type <const int &, degenerate>,
        meta::vector <const int &, const int, int> >));

    BOOST_MPL_ASSERT ((meta::equal <
        rime::detail::generalise_type <int_ <4>, degenerate>,
        meta::vector <int_ <4>, const int, int> >));
}

BOOST_AUTO_TEST_SUITE_END()

