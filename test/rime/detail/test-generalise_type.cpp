/*
Copyright 2011, 2012 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_detail_generalise_type
#include "utility/test/boost_unit_test.hpp"

#include <boost/mpl/assert.hpp>
#include "meta/equal.hpp"

#include "rime/detail/generalise_type.hpp"

#include "meta/vector.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_detail_generalise_type)

template <int N> struct int_;

struct degenerate {
    template <typename Type> struct apply
    { typedef Type type; };

    template <typename Type> struct apply <Type &>
    { typedef Type type; };

    template <typename Type> struct apply <Type const>
    { typedef Type type; };

    template <int N> struct apply <int_ <N>>
    { typedef int const type; };
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

