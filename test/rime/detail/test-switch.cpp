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

#define BOOST_TEST_MODULE test_rime_detail_switch
#include "utility/test/boost_unit_test.hpp"

#include <boost/mpl/assert.hpp>

#include "rime/detail/switch.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_detail_switch)

struct struct_default {
    float operator() (float f) const { return -1.0; }
};

struct struct_times_5 {
    float operator() (float f) const { return 5*f; }
};

struct struct_times_7 {
    float operator() (float f) const { return 7*f; }
};

using rime::detail::switch_;

BOOST_AUTO_TEST_CASE (test_rime_detail_switch) {
    switch_ <double,
        meta::vector <struct_default, struct_times_5, struct_times_7> > s;
    BOOST_CHECK_EQUAL (s (0, 7), -1.);
    BOOST_CHECK_EQUAL (s (1, 10), 50.);
    BOOST_CHECK_EQUAL (s (2, 11), 77.);
}

BOOST_AUTO_TEST_SUITE_END()

