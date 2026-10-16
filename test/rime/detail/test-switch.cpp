// SPDX-FileCopyrightText: Copyright 2011, 2012 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_rime_detail_switch
#include <boost/test/unit_test.hpp>

#include <boost/mpl/assert.hpp>

#include "rime/detail/switch.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_detail_switch)

struct struct_default
{
    float operator()(float) const { return -1.0; }
};

struct struct_times_5
{
    float operator()(float f) const { return 5 * f; }
};

struct struct_times_7
{
    float operator()(float f) const { return 7 * f; }
};

using rime::detail::switch_;

BOOST_AUTO_TEST_CASE(test_rime_detail_switch)
{
    switch_<
        double, meta::vector<struct_default, struct_times_5, struct_times_7>>
        s;
    BOOST_CHECK_EQUAL(s(0, 7), -1.);
    BOOST_CHECK_EQUAL(s(1, 10), 50.);
    BOOST_CHECK_EQUAL(s(2, 11), 77.);
}

BOOST_AUTO_TEST_SUITE_END()
