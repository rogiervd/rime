/*
Copyright 2013 Rogier van Dalen.

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

#ifndef RIME_ASSERT_HPP_INCLUDED
#define RIME_ASSERT_HPP_INCLUDED

#include <cassert>

#include "core.hpp"

namespace rime {

    namespace detail {

        template <class Condition, class Enable = void> struct assert_;

        template <class Condition>
            struct assert_ <Condition, typename
                boost::enable_if <rime::is_constant <Condition>>::type>
        {
            void operator() (Condition const &) const
            { static_assert (Condition::value, "rime::assert_ failed"); }
        };

        template <class Condition>
            struct assert_ <Condition, typename
                boost::disable_if <rime::is_constant <Condition>>::type>
        {
            void operator() (Condition const & condition) const
            { assert (condition); }
        };

    } // namespace detail

    template <class Condition> inline
        void assert_ (Condition const & condition)
    {
        detail::assert_ <Condition> implementation;
        implementation (condition);
    }

} // namespace rime

#endif  // RIME_ASSERT_HPP_INCLUDED

