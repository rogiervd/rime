/*
Copyright 2012 Rogier van Dalen.

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

/**
Operators for variant.
operator() and operator[] must be defined inside the class.
*/

#ifndef RIME_DETAIL_VARIANT_OPERATOR_HPP_INCLUDED
#define RIME_DETAIL_VARIANT_OPERATOR_HPP_INCLUDED

#include <boost/mpl/or.hpp>

#include "variant_fwd.hpp"

namespace rime {

/* Unary operators */

#define RIME_VARIANT_DEFINE_UNARY_OPERATOR(name, operation) \
namespace variant_detail { \
    struct name { \
        template <typename Actual> \
        decltype (operation std::declval <Actual &&>()) \
            operator() (Actual && actual) const \
        { return operation std::forward <Actual> (actual); } \
    }; \
    \
    /* This class is necessary to wrap decltype() in GCC 4.6. */ \
    template <typename Variant, typename Dummy> struct name##_result { \
        typedef decltype (visit (name()) (std::declval <Variant &&>())) \
            type; \
    }; \
} /* namespace variant_detail */ \
\
template <class Variant> inline \
    typename boost::enable_if <is_variant <Variant>, \
        typename variant_detail::name##_result <Variant, void>::type \
    >::type operator operation (Variant && variant) \
{ \
    return visit (variant_detail::name()) ( \
        std::forward <Variant> (variant)); \
}

RIME_VARIANT_DEFINE_UNARY_OPERATOR(unary_minus, -)
RIME_VARIANT_DEFINE_UNARY_OPERATOR(unary_plus, +)
RIME_VARIANT_DEFINE_UNARY_OPERATOR(not_, !)
RIME_VARIANT_DEFINE_UNARY_OPERATOR(increment, ++)
RIME_VARIANT_DEFINE_UNARY_OPERATOR(decrement, --)
RIME_VARIANT_DEFINE_UNARY_OPERATOR(binary_not, ~)
RIME_VARIANT_DEFINE_UNARY_OPERATOR(dereference, *)

#undef RIME_VARIANT_DEFINE_UNARY_OPERATOR

#define RIME_VARIANT_DEFINE_UNARY_POSTFIX_OPERATOR(name, operation) \
namespace variant_detail { \
    struct name { \
        template <typename Actual> \
        decltype (std::declval <Actual &&>() operation) \
            operator() (Actual && actual) const \
        { return std::forward <Actual> (actual) operation; } \
    }; \
    \
    /* This class is necessary to wrap decltype() in GCC 4.6. */ \
    template <typename Variant> struct name##_result { \
        typedef decltype (visit (name()) (std::declval <Variant &&>())) \
            type; \
    }; \
} /* namespace variant_detail */ \
\
template <class Variant> inline \
    typename boost::enable_if <is_variant <Variant>, \
        typename variant_detail::name##_result <Variant>::type \
    >::type operator operation (Variant && variant, int) \
{ \
    return visit (variant_detail::name()) ( \
        std::forward <Variant> (variant)); \
}

RIME_VARIANT_DEFINE_UNARY_POSTFIX_OPERATOR(postfix_increment, ++)
RIME_VARIANT_DEFINE_UNARY_POSTFIX_OPERATOR(postfix_decrement, --)

#undef RIME_VARIANT_DEFINE_UNARY_POSTFIX_OPERATOR

/* Binary operators */

#define RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(name, operation) \
namespace variant_detail { \
    struct name { \
        template <typename LeftActual, typename RightActual> \
        decltype (std::declval <LeftActual &&>() operation \
            std::declval <RightActual &&>()) \
            operator() (LeftActual && left, RightActual && right) const \
        { \
            return std::forward <LeftActual &&> (left) operation \
                std::forward <RightActual &&> (right); \
        } \
    }; \
    \
    /* This class is necessary to wrap decltype() in GCC 4.6. */ \
    template <typename LeftVariant, typename RightVariant> \
        struct name##_result \
    { \
        typedef decltype (visit (name()) (std::declval <LeftVariant &&>(), \
            std::declval <RightVariant &&>())) type; \
    }; \
} /* namespace variant_detail */ \
\
template <class LeftVariant, class RightVariant> inline \
    typename boost::enable_if <boost::mpl::or_ < \
        is_variant <LeftVariant>, is_variant <RightVariant>>, \
        typename variant_detail::name##_result < \
            LeftVariant, RightVariant>::type \
    >::type operator operation (LeftVariant && left, RightVariant && right) \
{ \
    return visit (variant_detail::name()) (std::forward <LeftVariant> (left), \
        std::forward <RightVariant> (right)); \
}

RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(plus, +)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(minus, -)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(times, *)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(divides, /)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(modulo, %)

RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(equal, ==)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(not_equal, !=)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(greater, >)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(less, <)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(greater_equal, >=)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(less_equal, <=)

// What about short-circuiting?
//RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(and_, &&)
//RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(or_, ||)

RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(logical_and, &)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(logical_or, |)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(logical_xor, ^)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(shift_left, <<)
RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR(shift_right, >>)

#undef RIME_VARIANT_DEFINE_BINARY_POSTFIX_OPERATOR

} // namespace rime

#endif  // RIME_DETAIL_VARIANT_OPERATOR_HPP_INCLUDED

