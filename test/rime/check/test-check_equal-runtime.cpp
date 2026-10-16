// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#define BOOST_TEST_MODULE test_rime_check_equal_fail
#include <boost/test/unit_test.hpp>

#include "rime/check/check_equal.hpp"

BOOST_AUTO_TEST_SUITE(fail_rime_check_equal)

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(fail_rime_check_equal, 1)
BOOST_AUTO_TEST_CASE(fail_rime_check_equal) { RIME_CHECK_EQUAL(4, 5); }

BOOST_AUTO_TEST_SUITE_END()
