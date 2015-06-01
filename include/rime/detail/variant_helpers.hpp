/*
Copyright 2011, 2012 Rogier van Dalen.

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

