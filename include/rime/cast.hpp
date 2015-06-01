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

