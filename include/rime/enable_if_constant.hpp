/*
Copyright 2014 Rogier van Dalen.

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
