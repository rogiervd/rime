/*
Copyright 2014, 2015 Rogier van Dalen.

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

#define BOOST_TEST_MODULE test_rime_variant
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include "rime/variant.hpp"

#include <boost/mpl/assert.hpp>

#include <array>
#include <iostream>

#include "utility/test/throwing.hpp"
#include "utility/test/tracked.hpp"

BOOST_AUTO_TEST_SUITE(test_rime_variant_replace)

typedef utility::throwing<utility::tracked<int>, utility::always_throw>
    throwing;
typedef rime::variant<int, throwing, void> variant;

using utility::copy_count;
using utility::destruct_count;
using utility::destruct_moved_count;
using utility::move_count;
using utility::value_construct_count;

void check_replace(utility::thrower & thrower)
{
    // int is replaced by int.
    {
        variant v1(7);
        variant v2(9);
        v1.replace(v2);
        BOOST_CHECK(v1.contains<int>());
        BOOST_CHECK_EQUAL(rime::get<int>(v1), 9);
    }
    {
        variant v1(7);
        variant v2(9);
        v1.replace(std::move(v2));
        BOOST_CHECK(v1.contains<int>());
        BOOST_CHECK_EQUAL(rime::get<int>(v1), 9);
    }
    // int is replaced by throwing.
    {
        utility::tracked_registry r;
        {
            variant v1(7);
            auto before = r.counts();
            variant v2(throwing(thrower, r, 10));
            BOOST_CHECK_EQUAL(
                r.since(before),
                value_construct_count(1) + move_count(1)
                    + destruct_moved_count(1));

            before = r.counts();
            v1.replace(v2);
            BOOST_CHECK_EQUAL(r.since(before), copy_count(1));
            BOOST_CHECK(v1.contains<throwing>());
            BOOST_CHECK_EQUAL(rime::get<throwing>(v1).content().content(), 10);
        }
    }
    {
        utility::tracked_registry r;
        {
            auto before = r.counts();
            variant v1(7);
            variant v2(throwing(thrower, r, 10));
            BOOST_CHECK_EQUAL(
                r.since(before),
                value_construct_count(1) + move_count(1)
                    + destruct_moved_count(1));

            before = r.counts();
            v1.replace(std::move(v2));
            BOOST_CHECK_EQUAL(r.since(before), move_count(1));

            BOOST_CHECK(v1.contains<throwing>());
            BOOST_CHECK_EQUAL(rime::get<throwing>(v1).content().content(), 10);
        }
    }
    // int is replaced by void.
    {
        variant v1(7);
        variant v2;
        v1.replace(v2);
        BOOST_CHECK(v1.contains<void>());
    }
    {
        variant v1(7);
        variant v2;
        v1.replace(std::move(v2));
        BOOST_CHECK(v1.contains<void>());
    }

    // throwing is replaced by int.
    {
        utility::tracked_registry r;
        {
            auto before = r.counts();
            variant v1(throwing(thrower, r, 17));
            variant v2(9);
            BOOST_CHECK_EQUAL(
                r.since(before),
                value_construct_count(1) + move_count(1)
                    + destruct_moved_count(1));

            before = r.counts();
            v1.replace(v2);
            BOOST_CHECK_EQUAL(r.since(before), destruct_count(1));

            BOOST_CHECK(v1.contains<int>());
            BOOST_CHECK_EQUAL(rime::get<int>(v1), 9);
        }
    }
    {
        utility::tracked_registry r;
        {
            auto before = r.counts();
            variant v1(throwing(thrower, r, 17));
            variant v2(9);
            BOOST_CHECK_EQUAL(
                r.since(before),
                value_construct_count(1) + move_count(1)
                    + destruct_moved_count(1));

            before = r.counts();
            v1.replace(std::move(v2));
            BOOST_CHECK_EQUAL(r.since(before), destruct_count(1));

            BOOST_CHECK(v1.contains<int>());
            BOOST_CHECK_EQUAL(rime::get<int>(v1), 9);
        }
    }
    // throwing is replaced by throwing.
    {
        utility::tracked_registry r;
        {
            auto before = r.counts();
            variant v1(throwing(thrower, r, 17));
            variant v2(throwing(thrower, r, 25));
            BOOST_CHECK_EQUAL(
                r.since(before),
                value_construct_count(2) + move_count(2)
                    + destruct_moved_count(2));

            before = r.counts();
            v1.replace(v2);
            BOOST_CHECK_EQUAL(
                r.since(before), copy_count(1) + destruct_count(1));

            BOOST_CHECK(v1.contains<throwing>());
            BOOST_CHECK_EQUAL(rime::get<throwing>(v1).content().content(), 25);
        }
    }
    {
        utility::tracked_registry r;
        {
            auto before = r.counts();
            variant v1(throwing(thrower, r, 17));
            variant v2(throwing(thrower, r, 25));
            BOOST_CHECK_EQUAL(
                r.since(before),
                value_construct_count(2) + move_count(2)
                    + destruct_moved_count(2));

            before = r.counts();
            v1.replace(std::move(v2));
            BOOST_CHECK_EQUAL(
                r.since(before), move_count(1) + destruct_count(1));

            BOOST_CHECK(v1.contains<throwing>());
            BOOST_CHECK_EQUAL(rime::get<throwing>(v1).content().content(), 25);
        }
    }
    // throwing is replaced by void.
    {
        utility::tracked_registry r;
        {
            auto before = r.counts();
            variant v1(throwing(thrower, r, 17));
            variant v2;
            BOOST_CHECK_EQUAL(
                r.since(before),
                value_construct_count(1) + move_count(1)
                    + destruct_moved_count(1));

            before = r.counts();
            v1.replace(v2);
            BOOST_CHECK_EQUAL(r.since(before), destruct_count(1));

            BOOST_CHECK(v1.contains<void>());
        }
    }
    {
        utility::tracked_registry r;
        {
            auto before = r.counts();
            variant v1(throwing(thrower, r, 17));
            variant v2;
            BOOST_CHECK_EQUAL(
                r.since(before),
                value_construct_count(1) + move_count(1)
                    + destruct_moved_count(1));

            before = r.counts();
            v1.replace(std::move(v2));
            BOOST_CHECK_EQUAL(r.since(before), destruct_count(1));

            BOOST_CHECK(v1.contains<void>());
        }
    }

    // void is replaced by int.
    {
        variant v1;
        variant v2(9);
        v1.replace(v2);
        BOOST_CHECK(v1.contains<int>());
        BOOST_CHECK_EQUAL(rime::get<int>(v1), 9);
    }
    {
        variant v1;
        variant v2(9);
        v1.replace(std::move(v2));
        BOOST_CHECK(v1.contains<int>());
        BOOST_CHECK_EQUAL(rime::get<int>(v1), 9);
    }
    // void is replaced by throwing.
    {
        utility::tracked_registry r;
        {
            variant v1;
            variant v2(throwing(thrower, r, 10));
            v1.replace(v2);
            BOOST_CHECK(v1.contains<throwing>());
            BOOST_CHECK_EQUAL(rime::get<throwing>(v1).content().content(), 10);
        }
    }
    {
        utility::tracked_registry r;
        {
            variant v1;
            variant v2(throwing(thrower, r, 10));
            v1.replace(std::move(v2));
            BOOST_CHECK(v1.contains<throwing>());
            BOOST_CHECK_EQUAL(rime::get<throwing>(v1).content().content(), 10);
        }
    }
    // void is replaced by void.
    {
        variant v1;
        variant v2;
        v1.replace(v2);
        BOOST_CHECK(v1.contains<void>());
    }
    {
        variant v1;
        variant v2;
        v1.replace(std::move(v2));
        BOOST_CHECK(v1.contains<void>());
    }
}

BOOST_AUTO_TEST_CASE(test_rime_variant_replace)
{
    utility::check_all_throw_points(check_replace);
}

BOOST_AUTO_TEST_SUITE_END()
