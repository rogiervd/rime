/*
Copyright 2013, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_call_if
#include "utility/test/boost_unit_test.hpp"

#include "rime/call_if.hpp"

#include <type_traits>
#include <boost/mpl/assert.hpp>

#include "check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_if)

// Has no call operator.
struct fail {};

struct return_as_double {
    double operator() (double d) const { return d; }
};
struct return_as_float {
    float operator() (long l) const { return float (l); }
};
struct return_as_long {
    long operator() (long l) const { return l; }
};

BOOST_AUTO_TEST_CASE (test_rime_call_if) {
    auto vd = rime::call_if (true, return_as_double(), return_as_long(), 2);
    BOOST_CHECK (vd.contains <double>());
    BOOST_CHECK_EQUAL (rime::get <double> (vd), 2.);
    auto vl = rime::call_if (false, return_as_double(), return_as_long(), 3);
    BOOST_CHECK (vl.contains <long>());
    BOOST_CHECK_EQUAL (rime::get <long> (vl), 3l);

    // Compile-time constants: operator() should never be instantiated on
    // "fail()".
    auto d = rime::call_if (rime::true_, return_as_double(), fail(), 5);
    RIME_CHECK_EQUAL (d, 5.);
    auto l = rime::call_if (rime::false_, fail(), return_as_long(), 7);
    RIME_CHECK_EQUAL (l, 7l);
}

template <class Base> struct merge_float {
    template <class Type1, class Type2, class Dummy = void> struct apply
    : Base::template apply <Type1, Type2> {};

    template <class Dummy> struct apply <float, double, Dummy>
    { typedef double type; };
    template <class Dummy> struct apply <double, float, Dummy>
    { typedef double type; };
};

BOOST_AUTO_TEST_CASE (test_rime_call_if_merge) {
    namespace merge_policy = rime::merge_policy;
    typedef merge_policy::to_variant <
        merge_policy::reference <merge_policy::const_ <
            merge_float <merge_policy::same<>>>>> policy;

    auto v1 = rime::call_if <policy> (false,
        return_as_double(), return_as_long(), 2);
    BOOST_CHECK (v1.contains <long>());
    BOOST_CHECK_EQUAL (rime::get <long> (v1), 2);

    auto v2 = rime::call_if <policy> (false,
        return_as_double(), return_as_float(), 7);
    BOOST_MPL_ASSERT ((std::is_same <decltype (v2), double>));
    BOOST_CHECK_EQUAL (v2, 7.);
}

BOOST_AUTO_TEST_SUITE_END()
