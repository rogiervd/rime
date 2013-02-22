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

#define BOOST_TEST_MODULE test_rime_detail_variant_dispatch
#include "../boost_unit_test.hpp"

#include <type_traits>

#include <boost/mpl/assert.hpp>

#include "rime/detail/variant_dispatch.hpp"

#include "rime/variant.hpp"

using rime::variant_detail::possible_actual_types_for;

BOOST_AUTO_TEST_SUITE(test_rime_detail_variant_dispatch)

BOOST_AUTO_TEST_CASE (test_rime_detail_dispatch) {
    {
        typedef possible_actual_types_for <meta::vector <
            rime::variant <int, long> const &,
            rime::variant <bool, double> const>> list;
        BOOST_MPL_ASSERT ((std::is_same <list::type,
            meta::vector <
                meta::vector <int, bool>,
                meta::vector <int, double>,
                meta::vector <long, bool>,
                meta::vector <long, double> > >));
    }
    {
        typedef possible_actual_types_for <meta::vector<>> list;
        BOOST_MPL_ASSERT ((std::is_same <list::type,
            meta::vector<meta::vector<> > >));
        BOOST_CHECK_EQUAL (list::get_index(), 0u);
    }
    {
        typedef possible_actual_types_for <meta::vector<int>> list;
        BOOST_MPL_ASSERT ((std::is_same <list::type,
            meta::vector<meta::vector<int> > >));
        BOOST_CHECK_EQUAL (list::get_index (4), 0u);
    }
    {
        typedef possible_actual_types_for <meta::vector <int const &>> list;
        BOOST_MPL_ASSERT ((std::is_same <list::type,
            meta::vector<meta::vector<int const &> > >));
        BOOST_CHECK_EQUAL (list::get_index (4), 0u);
    }
    {
        typedef possible_actual_types_for <meta::vector <int, long>> list;
        BOOST_MPL_ASSERT ((std::is_same <list::type,
            meta::vector<meta::vector<int, long> > >));
        BOOST_CHECK_EQUAL (list::get_index (4, 5l), 0u);
    }
    {
        typedef possible_actual_types_for <meta::vector <int const &, long>>
            list;
        BOOST_MPL_ASSERT ((std::is_same <list::type,
            meta::vector<meta::vector<int const &, long> > >));
        BOOST_CHECK_EQUAL (list::get_index (4, 5l), 0u);
    }
    {
        typedef possible_actual_types_for <
            meta::vector <rime::variant <int, long>>> list;
        BOOST_MPL_ASSERT ((std::is_same <list::type,
            meta::vector<meta::vector<int>, meta::vector <long> > >));
        // Postpone checking get_index
    }
    {
        typedef possible_actual_types_for <meta::vector <
            rime::variant <int, long>, bool>> list;
        BOOST_MPL_ASSERT ((std::is_same <list::type,
            meta::vector <
                meta::vector <int, bool>,
                meta::vector <long, bool> > >));
    }
    {
        typedef possible_actual_types_for <meta::vector <
            bool, rime::variant <int, long>&>> list;
        BOOST_MPL_ASSERT ((std::is_same <list::type,
            meta::vector <
                meta::vector <bool, int>,
                meta::vector <bool, long> > >));
    }
    {
        typedef possible_actual_types_for <meta::vector <
            rime::variant <int, long> const &,
            rime::variant <bool, double> const>> list;
        BOOST_MPL_ASSERT ((std::is_same <list::type,
            meta::vector <
                meta::vector <int, bool>,
                meta::vector <int, double>,
                meta::vector <long, bool>,
                meta::vector <long, double> > >));
    }
}

BOOST_AUTO_TEST_CASE (test_rime_detail_dispatch_runtime) {
    rime::variant <int, long> i (5);
    rime::variant <int, long> l (5l);
    rime::variant <bool, double> b (true);
    rime::variant <bool, double> d (5.5);
    {
        typedef possible_actual_types_for <
            meta::vector <rime::variant <int, long> &>> list;
        BOOST_CHECK_EQUAL (list::get_index (i), 0u);
        BOOST_CHECK_EQUAL (list::get_index (l), 1u);
    }
    {
        typedef possible_actual_types_for <meta::vector <
            rime::variant <int, long> const &, bool>> list;
        BOOST_CHECK_EQUAL (list::get_index (i, true), 0u);
        BOOST_CHECK_EQUAL (list::get_index (l, true), 1u);
    }
    {
        typedef possible_actual_types_for <meta::vector <
            bool, rime::variant <int, long>& >> list;
        BOOST_CHECK_EQUAL (list::get_index (true, i), 0u);
        BOOST_CHECK_EQUAL (list::get_index (true, l), 1u);
    }
    {
        typedef possible_actual_types_for <meta::vector <
            rime::variant <int, long> const &, rime::variant <bool, double> &>>
            list;
        BOOST_CHECK_EQUAL (list::get_index (i, b), 0u);
        BOOST_CHECK_EQUAL (list::get_index (i, d), 1u);
        BOOST_CHECK_EQUAL (list::get_index (l, b), 2u);
        BOOST_CHECK_EQUAL (list::get_index (l, d), 3u);
    }
}

struct function {
    int operator() () { return 0; }
    int operator() (int) { return 1; }
    int operator() (double) { return 2; }
    int operator() (int, double) { return 3; }
    int operator() (int, char, double) { return 4; }
};

BOOST_AUTO_TEST_CASE (test_rime_detail_dispatch_recipient) {
    function f;
    {
        typedef meta::vector <function &, rime::variant <int, void>,
            rime::variant <void, double>> arguments_type;

        rime::variant_detail::dispatch_recipient <arguments_type,
            meta::vector <function &, int, double>> d1;
        BOOST_CHECK_EQUAL (d1 (f, 0, 3.), 3);

        rime::variant_detail::dispatch_recipient <arguments_type,
            meta::vector <function &, int, void>> d2;
        BOOST_CHECK_EQUAL (d2 (f, 3, rime::variant <void, double>()), 1);

        rime::variant_detail::dispatch_recipient <arguments_type,
            meta::vector <function &, void, double>> d3;
        BOOST_CHECK_EQUAL (d3 (f, rime::variant <int, void>(), 3.), 2);

        rime::variant_detail::dispatch_recipient <arguments_type,
            meta::vector <function &, void, void>> d4;
        BOOST_CHECK_EQUAL (d4 (f, rime::variant <int, void>(),
            rime::variant <void, double>()), 0);
    }

    {
        typedef meta::vector <function &,
            rime::variant <int, void>, rime::variant <void, char>,
            rime::variant <double, void>> arguments_type;

        rime::variant_detail::dispatch_recipient <arguments_type,
            meta::vector <function &, int, char, double>> d1;
        BOOST_CHECK_EQUAL (d1 (f, 0, 'a', 3.), 4);

        rime::variant_detail::dispatch_recipient <arguments_type,
            meta::vector <function &, int, void, double>> d2;
        BOOST_CHECK_EQUAL (d2 (f, 0, rime::variant <void, char>(), 3.), 3);

        rime::variant_detail::dispatch_recipient <arguments_type,
            meta::vector <function &, void, void, double>> d3;
        BOOST_CHECK_EQUAL (d3 (f, rime::variant <int, void>(),
            rime::variant <void, char>(), 3.), 2);

        rime::variant_detail::dispatch_recipient <arguments_type,
            meta::vector <function &, void, void, void>> d4;
        BOOST_CHECK_EQUAL (d4 (f, rime::variant <int, void>(),
            rime::variant <void, char>(), rime::variant <double, void>()), 0);
    }
}

BOOST_AUTO_TEST_SUITE_END()

