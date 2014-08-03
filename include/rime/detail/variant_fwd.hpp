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

#ifndef RIME_VARIANT_FWD_HPP
#define RIME_VARIANT_FWD_HPP

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/and.hpp>

#include "utility/storage.hpp"
#include "meta/contains.hpp"
#include "meta/merge_types.hpp"

namespace rime {

template <typename ... Types> class variant;

template <typename Type> struct is_variant;

template <typename Type> struct variant_types;

struct merge_two;

template <typename Types, typename MergeTwo = merge_two>
    struct make_variant_over;

template <typename Actual, typename Variant>
    inline typename boost::enable_if <
        boost::mpl::and_ <
            is_variant <Variant>,
            meta::contains <Actual, typename variant_types <Variant>::type>>,
        typename ::utility::storage::get <Actual, Variant &&>::type
    >::type
    get_unsafe (Variant && variant);

} // namespace rime

#endif  // RIME_VARIANT_FWD_HPP

