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

#define BOOST_TEST_MODULE test_rime_nothing
#include "utility/test/boost_unit_test.hpp"

#include "rime/nothing.hpp"

#include <type_traits>
#include <string>

#include <boost/mpl/assert.hpp>

BOOST_AUTO_TEST_SUITE(test_rime_nothing)

BOOST_AUTO_TEST_CASE (test_rime_nothing) {
    // Call "rime::nothing" with various arguments.
    BOOST_MPL_ASSERT ((std::is_same <void, decltype (rime::nothing())>));
    rime::nothing();

    BOOST_MPL_ASSERT ((std::is_same <void, decltype (rime::nothing (5))>));
    rime::nothing (5);

    std::string hello ("hello");

    BOOST_MPL_ASSERT ((std::is_same <void,
        decltype (rime::nothing (5, hello, 6.5))>));
    rime::nothing (5, hello, 6.5);

    // Make our own "nothing".
    rime::callable::nothing nothing;
    nothing (5, hello, 6.5);
}

BOOST_AUTO_TEST_SUITE_END()
