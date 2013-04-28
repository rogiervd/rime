/*
Copyright 2011, 2012 Rogier van Dalen.

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

#ifndef RIME_VARIANT_HELPERS_HPP
#define RIME_VARIANT_HELPERS_HPP

#include <boost/mpl/bool.hpp>

#include "meta/vector.hpp"

#include "utility/storage.hpp"

#include "rime/detail/variant_fwd.hpp"

namespace rime {

/**
Compile-time constant, true iff Type is a rime::variant.
*/
template <typename Type> struct is_variant : boost::mpl::false_ {};
template <typename Type> struct is_variant <Type const> : is_variant <Type> {};
template <typename Type> struct is_variant <Type &> : is_variant <Type> {};
template <typename... Types>
    struct is_variant <variant <Types...> >
: boost::mpl::true_ {};

/**
Meta-function: return list of types that Type can be.
For a variant type, this is all the types in the variant.
For any other type, this is a list of just that type.

\todo Test explicitly.
*/
template <typename Type> struct variant_types
{ typedef meta::vector <Type> type; };

template <typename ... Types> struct variant_types <variant <Types ...> >
{ typedef meta::vector <Types ...> type; };

template <typename ... Types> struct variant_types <variant <Types ...> & >
: variant_types <variant <Types ...> > {};

template <typename ... Types> struct variant_types <variant <Types ...> const>
: variant_types <variant <Types ...> > {};

template <typename ... Types> struct variant_types <variant <Types ...> const &>
: variant_types <variant <Types ...> > {};

} // namespace rime

#endif  // RIME_VARIANT_HELPERS_HPP

