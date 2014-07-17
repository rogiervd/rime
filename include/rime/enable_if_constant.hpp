/*
Copyright 2014 Rogier van Dalen.

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

#ifndef RIME_ENABLE_IF_CONSTANT_HPP_INCLUDED
#define RIME_ENABLE_IF_CONSTANT_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/not.hpp>
#include <boost/mpl/and.hpp>
#include <boost/utility/enable_if.hpp>

#include "core.hpp"

namespace rime {

/**
Contain a type \a type iff \a Constant is a compile-time constant.
*/
template <class Constant, class Result = void> struct enable_if_constant
: boost::enable_if <is_constant <Constant>, Result> {};

/**
Contain a type \a type iff \a Constant is a not compile-time constant.
*/
template <class Constant, class Result = void> struct disable_if_constant
: boost::disable_if <is_constant <Constant>, Result> {};

/**
Contain a type \a type iff \a Constant is a compile-time constant with value
\a true.
*/
template <class Constant, class Result = void> struct enable_if_constant_true
: boost::enable_if <boost::mpl::and_ <
    is_constant <Constant>, typename std::decay <Constant>::type>, Result> {};

/**
Contain a type \a type iff \a Constant is not a compile-time constant with value
\a true.
*/
template <class Constant, class Result = void> struct enable_if_constant_false
: boost::enable_if <boost::mpl::and_ <
    is_constant <Constant>,
    boost::mpl::not_ <typename std::decay <Constant>::type>>, Result> {};

/**
Contain a type \a type iff \a Constant is a compile-time constant with value
\a false.
*/
template <class Constant, class Result = void> struct disable_if_constant_true
: boost::disable_if <boost::mpl::and_ <
    is_constant <Constant>, typename std::decay <Constant>::type>, Result> {};

/**
Contain a type \a type iff \a Constant is not a compile-time constant with value
\a false.
*/
template <class Constant, class Result = void> struct disable_if_constant_false
: boost::disable_if <boost::mpl::and_ <
    is_constant <Constant>,
    boost::mpl::not_ <typename std::decay <Constant>::type>>, Result> {};

} // namespace rime

#endif // RIME_ENABLE_IF_CONSTANT_HPP_INCLUDED
