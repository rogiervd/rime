/*
Copyright 2011, 2012, 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_if
#include "utility/test/boost_unit_test.hpp"

#include "rime/if.hpp"

#include <type_traits>
#include <boost/mpl/assert.hpp>

#include "rime/check/check_equal.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_if)

struct return_as_double {
    double operator() (double d) const { return d; }
};
struct return_as_long {
    long operator() (long l) const { return l; }
};

BOOST_AUTO_TEST_CASE (test_rime_if) {
    // Run-time conditions
    RIME_CHECK_EQUAL (rime::if_ (false, 3, 10), 10);
    RIME_CHECK_EQUAL (rime::if_ (true, 3, 10), 3);
    RIME_CHECK_EQUAL (rime::if_ (true, 3., 10.), 3.);

    // Results in boost::variant
    BOOST_CHECK_EQUAL (rime::get <double> (rime::if_ (false, 3, 10.3)), 10.3);
    BOOST_CHECK_EQUAL (rime::get <int> (rime::if_ (true, 3, 10.3)), 3);

    // Compile-time conditions
    RIME_CHECK_EQUAL (rime::if_ (rime::false_, 3, 10.3), 10.3);
    RIME_CHECK_EQUAL (rime::if_ (rime::true_, 3, 10.3), 3);

    // With C++11 compile-time constants.
    RIME_CHECK_EQUAL (rime::if_ (std::false_type(), 3, 10.3), 10.3);
    RIME_CHECK_EQUAL (rime::if_ (std::true_type(), 3, 10.3), 3);

    int i = 3;
    int const & ri = i;
    double d = 3.;
    double & rd = d;

    BOOST_MPL_ASSERT ((std::is_same <
        decltype (rime::if_ (rime::true_, 1u, rd)), unsigned>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (rime::if_ (rime::false_, ri, 2.3)), double>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (rime::if_ (rime::true_, i, rd)), int &>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (rime::if_ (rime::false_, ri, rd)), double &>));
    BOOST_MPL_ASSERT ((std::is_same <
        decltype (rime::if_ (rime::true_, ri, rd)), int const &>));

    RIME_CHECK_EQUAL (
        rime::if_ (rime::true_, return_as_double(), return_as_long()) (6), 6.);
    RIME_CHECK_EQUAL (
        rime::if_ (rime::false_, return_as_double(), return_as_long()) (5), 5l);

    auto vd = rime::if_ (true, return_as_double(), return_as_long()) (2);
    BOOST_CHECK (vd.contains <double>());
    BOOST_CHECK_EQUAL (rime::get <double> (vd), 2.);
    auto vl = rime::if_ (false, return_as_double(), return_as_long()) (3);
    BOOST_CHECK (vl.contains <long>());
    BOOST_CHECK_EQUAL (rime::get <long> (vl), 3);
}

template <class Base> struct merge_float {
    template <class Type1, class Type2, class Dummy = void> struct apply
    : Base::template apply <Type1, Type2> {};

    template <class Dummy> struct apply <float, double, Dummy>
    { typedef double type; };
    template <class Dummy> struct apply <float, long double, Dummy>
    { typedef long double type; };
    template <class Dummy> struct apply <double, long double, Dummy>
    { typedef long double type; };

    template <class Dummy> struct apply <double, float, Dummy>
    { typedef double type; };
    template <class Dummy> struct apply <long double, float, Dummy>
    { typedef long double type; };
    template <class Dummy> struct apply <long double, double, Dummy>
    { typedef long double type; };
};

BOOST_AUTO_TEST_CASE (test_rime_if_merge) {
    namespace merge_policy = rime::merge_policy;
    typedef merge_policy::decay <merge_float <merge_policy::same<>>> policy;

    float four = 4;
    long double const five = 5;
    long double const six = 6;

    auto && v1 = rime::if_ <policy> (true, four, 5.f);
    BOOST_MPL_ASSERT ((std::is_same <decltype (v1), float &&>));

    auto && v2 = rime::if_ <policy> (true, 4., 5.f);
    BOOST_MPL_ASSERT ((std::is_same <decltype (v2), double &&>));

    auto && v3 = rime::if_ <policy> (true, four, 5.);
    BOOST_MPL_ASSERT ((std::is_same <decltype (v3), double &&>));

    auto && v4 = rime::if_ <policy> (true, four, five);
    BOOST_MPL_ASSERT ((std::is_same <decltype (v4), long double &&>));

    auto && v5 = rime::if_ <policy> (true, six, five);
    BOOST_MPL_ASSERT ((std::is_same <decltype (v5), long double const &>));
}

BOOST_AUTO_TEST_SUITE_END()
