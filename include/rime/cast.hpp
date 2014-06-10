/*
Copyright 2013 Rogier van Dalen.

This file is part of Rogier van Dalen's Rime library for C++.

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

#ifndef RIME_CAST_HPP_INCLUDED
#define RIME_CAST_HPP_INCLUDED

#include "core.hpp"

#include "utility/returns.hpp"

namespace rime {

namespace types {

    namespace detail {

        template <class Target, class Type> struct cast_constant {
            static_assert (rime::is_constant <Type>::value,
                "Type must be a constant.");
            typedef rime::constant <Target, Target (Type::value)> type;
        };

    } // namespace detail

    template <class Target, class Type> struct cast
    : boost::mpl::eval_if <rime::is_constant <Type>,
        detail::cast_constant <Target, Type>,
        mpl::identity <Target>> {};

    template <class Target, class Type> struct cast_value
    : cast <typename rime::value <Target>::type, Type> {};

} // namespace types

namespace callable {

    template <class Target> struct cast {
        template <class Type>
            typename boost::disable_if <rime::is_constant <Type>, Target>::type
            operator() (Type const & v) const
        { return Target (v); }

        template <class Type>
            typename boost::lazy_enable_if <rime::is_constant <Type>,
                types::detail::cast_constant <Target, Type>>::type
            operator() (Type const &) const
        { return typename types::detail::cast_constant <Target, Type>::type(); }
    };

    template <class Target> struct cast_value
    : cast <typename rime::value <Target>::type> {};

} // namespace callable

/**
Cast a constant or a run-time value to a different type.
If v is a constant, the result will be a constant with value type Target.
If v is a run-time value, the result will be of type Target.
*/
template <class Target, class Type> inline auto cast (Type const & v)
RETURNS (callable::cast <Target>() (v));

/**
Cast a constant or a run-time value to the value type of Target.
*/
template <class Target, class Type> inline auto cast_value (Type const & v)
RETURNS (callable::cast_value <Target>() (v));

} // namespace rime

#endif  // RIME_CAST_HPP_INCLUDED

