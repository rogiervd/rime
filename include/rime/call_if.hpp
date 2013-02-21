/*
Copyright 2011, 2012 Rogier van Dalen.

This file is part of Rogier van Dalen's Range library for C++.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RIME_CALL_IF_HPP_INCLUDED
#define RIME_CALL_IF_HPP_INCLUDED

#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

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
    */
    struct call_if {
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
        : make_variant_over <meta::vector <
            typename std::result_of <Function1 (Arguments ...)>::type,
            typename std::result_of <Function2 (Arguments ...)>::type
        >> {};

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

static const auto call_if = callable::call_if();

} // namespace rime

#endif  // RIME_CALL_IF_HPP_INCLUDED

