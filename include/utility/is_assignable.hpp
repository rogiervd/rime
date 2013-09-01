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

/**
\file
std::is_assignable is not available on GCC 4.6.
This provides an equivalent class.
*/

#ifndef RIME_UTILITY_IS_ASSIGNABLE_HPP_INCLUDED
#define RIME_UTILITY_IS_ASSIGNABLE_HPP_INCLUDED

#include <type_traits>
#include <utility>

#include "rime/core.hpp"

namespace utility {

    namespace detail {
        template <class Destination, class Source> struct is_assignable {
        private:
            struct no {};
            struct yes {};
            template <class Type> struct make_yes { typedef yes type; };

            // Use SFINAE: if the argument to make_yes can be instantiated,
            // assignment is possible.
            template <class Destination2, class Source2>
                static typename make_yes <decltype (
                    std::declval <Destination2>() = std::declval <Source2>())
                >::type
            test (int);

            template <class, class> static no test (...);
        public:
            static const bool value = std::is_same <
                decltype (test <Destination, Source> (1)), yes>::value;
        };
    } // namespace detail

    template <class Destination, class Source> struct is_assignable
    : rime::bool_ <detail::is_assignable <Destination, Source>::value> {};

} // namespace utility

#endif // RIME_UTILITY_IS_ASSIGNABLE_HPP_INCLUDED

