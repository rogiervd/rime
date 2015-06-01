/*
Copyright 2014 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_check_equal_fail
#include "utility/test/boost_unit_test.hpp"

#include "rime/check/check_equal.hpp"

BOOST_AUTO_TEST_SUITE(fail_rime_check_equal)

BOOST_AUTO_TEST_CASE (fail_rime_check_equal) {
    RIME_CHECK_EQUAL (4, 5);
}

BOOST_AUTO_TEST_SUITE_END()
