// SPDX-FileCopyrightText: Copyright 2011, 2012, 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RIME_VARIANT_FWD_HPP
#define RIME_VARIANT_FWD_HPP

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/and.hpp>

#include "meta/contains.hpp"
#include "utility/storage.hpp"

namespace rime {

template <typename... Types> class variant;

template <typename Type> struct is_variant;

template <typename Type> struct variant_types;

namespace merge_policy {

    struct conservative;

}  // namespace merge_policy

template <typename Types, typename MergeTwo = merge_policy::conservative>
struct make_variant_over;

template <typename Actual, typename Variant> inline typename boost::enable_if<
    boost::mpl::and_<
        is_variant<Variant>,
        meta::contains<Actual, typename variant_types<Variant>::type>>,
    typename ::utility::storage::get<Actual, Variant &&>::type>::type
    get_unsafe(Variant && variant);

}  // namespace rime

#endif  // RIME_VARIANT_FWD_HPP
