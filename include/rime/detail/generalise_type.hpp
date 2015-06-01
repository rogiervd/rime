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

#ifndef RIME_DETAIL_GENERALISE_TYPE_HPP_INCLUDED
#define RIME_DETAIL_GENERALISE_TYPE_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>

#include "meta/range.hpp"

namespace rime { namespace detail {

    template <typename Type, typename Degenerate> struct generalise_type
    { typedef generalise_type type; };

    struct empty_generalise_type
    { typedef empty_generalise_type type; };

    struct generalise_type_tag;

}} // namespace rime::detail

namespace meta {

    template <typename Type, typename Degenerate>
        struct range_tag < ::rime::detail::generalise_type <Type, Degenerate> >
    { typedef ::rime::detail::generalise_type_tag type; };

    template <> struct range_tag < ::rime::detail::empty_generalise_type>
    { typedef ::rime::detail::generalise_type_tag type; };

    namespace operation {

        template <>
            struct empty <rime::detail::generalise_type_tag, meta::front>
        {
            template <typename Range, typename Void = void> struct apply;

            template <typename Type, typename Degenerate>
                struct apply <
                    ::rime::detail::generalise_type <Type, Degenerate> >
            : mpl::false_ {};

            template <typename Void>
                struct apply <
                    ::rime::detail::empty_generalise_type, Void>
            : mpl::true_ {};
        };

        template <>
            struct first <rime::detail::generalise_type_tag, meta::front>
        {
            template <typename Range> struct apply;

            template <typename Type, typename Degenerate>
                struct apply <
                    ::rime::detail::generalise_type <Type, Degenerate> >
            { typedef Type type; };
        };

        template <>
            struct drop_one <rime::detail::generalise_type_tag, meta::front>
        {
            template <typename Range> struct apply;

            template <typename Type, typename Degenerate>
                struct apply <
                    ::rime::detail::generalise_type <Type, Degenerate> >
            {
                typedef typename mpl::apply <Degenerate, Type>::type next_type;
                typedef typename mpl::if_ <
                        std::is_same <Type, next_type>,
                        ::rime::detail::empty_generalise_type,
                        ::rime::detail::generalise_type <next_type, Degenerate>
                    >::type type;
            };
        };

    } // namespace operation

} // namespace meta

#endif  // RIME_DETAIL_GENERALISE_TYPE_HPP_INCLUDED

