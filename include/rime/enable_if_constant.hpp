// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RIME_ENABLE_IF_CONSTANT_HPP_INCLUDED
#define RIME_ENABLE_IF_CONSTANT_HPP_INCLUDED

#include <type_traits>

#include "core.hpp"

namespace rime {

/**
Contain a type \a type iff \a Constant is a compile-time constant.
*/
template <class Constant, class Result = void> struct enable_if_constant
: std::enable_if<is_constant<Constant>::value, Result> {};

/**
Contain a type \a type iff \a Constant is a not compile-time constant.
*/
template <class Constant, class Result = void> struct disable_if_constant
: std::enable_if<!is_constant<Constant>::value, Result> {};

/**
Contain a type \a type iff \a Constant is a compile-time constant with value
\a true.
*/
template <class Constant, class Result = void> struct enable_if_constant_true
: std::enable_if<
      std::conjunction_v<is_constant<Constant>, std::decay_t<Constant>>,
      Result> {};

/**
Contain a type \a type iff \a Constant is not a compile-time constant with value
\a true.
*/
template <class Constant, class Result = void> struct enable_if_constant_false
: std::enable_if<
      std::conjunction_v<
          is_constant<Constant>, std::negation<std::decay_t<Constant>>>,
      Result> {};

/**
Contain a type \a type iff \a Constant is a compile-time constant with value
\a false.
*/
template <class Constant, class Result = void> struct disable_if_constant_true
: std::enable_if<
      !std::conjunction_v<is_constant<Constant>, std::decay_t<Constant>>,
      Result> {};

/**
Contain a type \a type iff \a Constant is not a compile-time constant with value
\a false.
*/
template <class Constant, class Result = void> struct disable_if_constant_false
: std::enable_if<
      !std::conjunction_v<
          is_constant<Constant>, std::negation<std::decay_t<Constant>>>,
      Result> {};

}  // namespace rime

#endif  // RIME_ENABLE_IF_CONSTANT_HPP_INCLUDED
