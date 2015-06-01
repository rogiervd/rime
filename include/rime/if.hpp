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

#ifndef RIME_IF_HPP_INCLUDED
#define RIME_IF_HPP_INCLUDED

#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

#include "utility/returns.hpp"

#include "core.hpp"
#include "variant.hpp"

namespace rime {

namespace callable {

    /**
    Function that can is passed three parameters: the condition, the result
    if the condition is true, and the result if the condition is false.
    If the condition is a compile-time value, then the appropriate result is
    returned with the appropriate type.
    If the condition is a run-time value, then the type of the result is the
    unified type of the two possible results.

    \tparam MergePolicy (optional)
        The type that is used to merge two types.
        By default, merge constants and types that are exactly the same.
    */
    template <class MergePolicy = merge_policy::default_policy> struct if_ {
        // Condition known to be true at compile time.
        template <class Condition, class ResultIfTrue, class ResultIfFalse>
            typename boost::enable_if <boost::mpl::and_ <
                rime::is_constant <Condition>,
                typename std::decay <Condition>::type
            >, ResultIfTrue>::type
        operator() (Condition &&, ResultIfTrue && if_true, ResultIfFalse &&)
            const
        { return std::forward <ResultIfTrue> (if_true); }

        // Condition known to be false at compile time.
        template <class Condition, class ResultIfTrue, class ResultIfFalse>
            typename boost::enable_if <boost::mpl::and_ <
                rime::is_constant <Condition>,
                boost::mpl::not_ <typename std::decay <Condition>::type>
            >, ResultIfFalse>::type
        operator() (Condition &&, ResultIfTrue &&, ResultIfFalse && if_false)
            const
        { return std::forward <ResultIfFalse> (if_false); }

        // Run-time condition.
        template <class Condition, class ResultIfTrue, class ResultIfFalse,
            class Enable = typename boost::enable_if <
                boost::mpl::not_ <rime::is_constant <Condition>>>::type>
        typename MergePolicy::template apply <ResultIfTrue, ResultIfFalse>::type
        operator() (Condition && condition,
            ResultIfTrue && if_true, ResultIfFalse && if_false) const
        {
            if (condition)
                return std::forward <ResultIfTrue> (if_true);
            else
                return std::forward <ResultIfFalse> (if_false);
        }
    };

} // namespace callable

template <class Condition, class ResultIfTrue, class ResultIfFalse>
inline auto if_ (Condition && condition,
    ResultIfTrue && if_true, ResultIfFalse && if_false)
RETURNS (callable::if_<>() (std::forward <Condition> (condition),
    std::forward <ResultIfTrue> (if_true),
    std::forward <ResultIfFalse> (if_false)));

template <class MergePolicy,
    class Condition, class ResultIfTrue, class ResultIfFalse>
inline auto if_ (Condition && condition,
    ResultIfTrue && if_true, ResultIfFalse && if_false)
RETURNS (callable::if_ <MergePolicy>() (std::forward <Condition> (condition),
    std::forward <ResultIfTrue> (if_true),
    std::forward <ResultIfFalse> (if_false)));

} // namespace rime

#endif  // RIME_IF_HPP_INCLUDED
