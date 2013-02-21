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

#ifndef RIME_IF_HPP_INCLUDED
#define RIME_IF_HPP_INCLUDED

#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"

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
    */
    struct if_ {
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
        template <class Condition, class ResultIfTrue, class ResultIfFalse>
            typename boost::enable_if <boost::mpl::not_ <
                rime::is_constant <Condition>>,
            make_variant_over <meta::vector <ResultIfTrue, ResultIfFalse>>
            >::type::type
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

static const auto if_ = callable::if_();

} // namespace rime

#endif  // RIME_IF_HPP_INCLUDED

