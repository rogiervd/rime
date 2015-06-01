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

#ifndef RIME_VARIANT_FWD_HPP
#define RIME_VARIANT_FWD_HPP

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/and.hpp>

#include "utility/storage.hpp"
#include "meta/contains.hpp"

namespace rime {

template <typename ... Types> class variant;

template <typename Type> struct is_variant;

template <typename Type> struct variant_types;

namespace merge_policy {

    struct conservative;

} // namespace merge_policy

template <typename Types, typename MergeTwo = merge_policy::conservative>
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

