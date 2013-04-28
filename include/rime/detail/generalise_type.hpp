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

