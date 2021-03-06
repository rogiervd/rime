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

#ifndef RIME_CALL_IF_HPP_INCLUDED
#define RIME_CALL_IF_HPP_INCLUDED

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
    Function that calls one function or another.
    \param condition The condition.
    \param if_true Function to be called if the condition is true.
    \param if_false Function to be called if the condition is false.
    \param Arguments Arguments to be passed to the function.

    If the condition is a compile-time value, then the appropriate function
    is called and its value is returned as is.
    The function call to the other function is not even instantiated.
    If the condition is a run-time value, then it is decided at run-time which
    function is called, and the return type is the unified type of the two
    possible results.

    \tparam MergePolicy (optional)
        The type that is used to merge two return types. types.
        By default, merge constants and types that are exactly the same.
    */
    template <class MergePolicy = merge_policy::default_policy> struct call_if {
        // Condition known to be true at compile time.
        template <class Condition, class IfTrue, class IfFalse,
                class ... Arguments>
            typename boost::lazy_enable_if <boost::mpl::and_ <
                    rime::is_constant <Condition>,
                    typename std::decay <Condition>::type
                >,
                std::result_of <IfTrue (Arguments ...)>
            >::type
        operator() (Condition &&, IfTrue && if_true, IfFalse &&,
            Arguments && ... arguments) const
        {
            return std::forward <IfTrue> (if_true) (
                std::forward <Arguments> (arguments) ...);
        }

        // Condition known to be false at compile time.
        template <class Condition, class IfTrue, class IfFalse,
                class ... Arguments>
            typename boost::lazy_enable_if <boost::mpl::and_ <
                    rime::is_constant <Condition>,
                    boost::mpl::not_ <typename std::decay <Condition>::type>
                >,
                std::result_of <IfFalse (Arguments ...)>
            >::type
        operator() (Condition &&, IfTrue &&, IfFalse && if_false,
            Arguments && ... arguments) const
        {
            return std::forward <IfFalse> (if_false) (
                std::forward <Arguments> (arguments) ...);
        }

        // This class is instantiated only if the condition is a run-time
        // value.
        template <class Function1, class Function2, class ... Arguments>
            struct merged_result_type
        : MergePolicy::template apply <
            typename std::result_of <Function1 (Arguments ...)>::type,
            typename std::result_of <Function2 (Arguments ...)>::type> {};

        // Run-time condition.
        template <class Condition, class IfTrue, class IfFalse,
                class ... Arguments>
            typename boost::lazy_enable_if <
                boost::mpl::not_ <rime::is_constant <Condition>>,
                merged_result_type <IfTrue, IfFalse, Arguments ...>
            >::type
        operator() (Condition && condition,
            IfTrue && if_true, IfFalse && if_false, Arguments && ... arguments)
            const
        {
            if (condition)
                return std::forward <IfTrue> (if_true) (
                    std::forward <Arguments> (arguments) ...);
            else
                return std::forward <IfFalse> (if_false) (
                    std::forward <Arguments> (arguments) ...);
        }
    };
} // namespace callable

// Without MergePolicy.
template <class Condition, class FunctionIfTrue, class FunctionIfFalse,
    class ... Arguments>
inline auto call_if (Condition && condition,
    FunctionIfTrue && if_true, FunctionIfFalse && if_false,
    Arguments && ... arguments)
RETURNS (callable::call_if <>() (std::forward <Condition> (condition),
    std::forward <FunctionIfTrue> (if_true),
    std::forward <FunctionIfFalse> (if_false),
    std::forward <Arguments> (arguments) ...));

// With MergePolicy.
template <class MergePolicy, class Condition,
    class FunctionIfTrue, class FunctionIfFalse, class ... Arguments>
inline auto call_if (Condition && condition,
    FunctionIfTrue && if_true, FunctionIfFalse && if_false,
    Arguments && ... arguments)
RETURNS (callable::call_if <MergePolicy>() (
    std::forward <Condition> (condition),
    std::forward <FunctionIfTrue> (if_true),
    std::forward <FunctionIfFalse> (if_false),
    std::forward <Arguments> (arguments) ...));

} // namespace rime

#endif  // RIME_CALL_IF_HPP_INCLUDED
