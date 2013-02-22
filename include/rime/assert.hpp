/*
Copyright 2013 Rogier van Dalen.

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

