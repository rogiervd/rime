/*
Copyright 2013 Rogier van Dalen.

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

/** \file
Handle sign mismatch in a correct and pleasant way.
*/

#ifndef RIME_SIGN_HPP_INCLUDED
#define RIME_SIGN_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/identity.hpp>

#include <boost/utility/enable_if.hpp>

#include "utility/returns.hpp"

#include "core.hpp"

namespace rime {

namespace types {

    /*
    Wrappers for std::is_signed and std::is_unsigned that work even if
    Type is not an integral.
    */
    template <class Type> struct std_is_signed
    : boost::mpl::eval_if <std::is_integral <Type>,
        std::is_signed <Type>, std::false_type> {};
    template <class Type> struct std_is_unsigned
    : boost::mpl::eval_if <std::is_integral <Type>,
        std::is_unsigned <Type>, std::false_type> {};

    template <class Type> struct is_signed
    : rime::as_rime_constant <typename std_is_signed <
            typename rime::value <typename std::remove_cv <Type>::type>::type>
        ::type>::type {};

    template <class Type> struct is_unsigned
    : rime::as_rime_constant <typename std_is_unsigned <
            typename rime::value <typename std::remove_cv <Type>::type>::type>
        ::type>::type {};

    namespace detail {

        template <template <class> class Convert, class Type>
            struct convert_constant
        {
            typedef typename std::decay <Type>::type decayed_type;
            static_assert (rime::is_constant <decayed_type>::value, "Sanity");

            typedef typename rime::value <decayed_type>::type
                original_value_type;
            typedef typename Convert <original_value_type>::type value_type;

            typedef rime::constant <
                value_type, value_type (decayed_type::value)> type;

        };

        template <template <class> class Convert, class Type>
            struct convert_integral_value
        : boost::mpl::eval_if <rime::is_constant <Type>,
            convert_constant <Convert, Type>,
            boost::mpl::eval_if <std::is_integral <Type>,
                Convert <Type>, mpl::identity <Type>>> {};

    } // namespace detail

    template <class Type> struct make_signed
    : detail::convert_integral_value <std::make_signed, Type> {};

    template <class Type> struct make_unsigned
    : detail::convert_integral_value <std::make_unsigned, Type> {};

    template <class Type> struct make_zero {
        static_assert (rime::is_constant <Type>::value,
            "types::make_zero only makes sense for constants.");
        typedef typename rime::value <Type>::type value_type;
        typedef rime::constant <value_type, value_type (0)> type;
    };

} // namespace types

namespace callable {

    template <template <class> class Convert> struct convert_integral_value
    {
        template <class Type> struct result
        : rime::types::detail::convert_integral_value <Convert, Type> {};

        // Constant: default-initialise.
        template <class Type>
            typename boost::enable_if <is_constant <Type>,
                typename result <Type>::type>::type
            operator() (Type const &) const
        { return typename result <Type>::type(); }

        // Run-time value: construct.
        template <class Type>
            typename boost::disable_if <is_constant <Type>,
                typename result <Type>::type>::type
            operator() (Type const & v) const
        { return typename result <Type>::type (v); }
    };

    struct make_signed : convert_integral_value <std::make_signed> {};
    struct make_unsigned : convert_integral_value <std::make_unsigned> {};

    struct make_zero {
        template <class Type>
            typename boost::disable_if <rime::is_constant <Type>, Type>::type
            operator() (Type const & n) const
        { return Type(); }

        template <class Type>
            typename boost::lazy_enable_if <rime::is_constant <Type>,
                rime::types::make_zero <Type>>::type
            operator() (Type const & n) const
        { return typename rime::types::make_zero <Type>::type(); }
    };

} // namespace callable

static auto const make_signed = callable::make_signed();
static auto const make_unsigned = callable::make_unsigned();
static auto const make_zero = callable::make_zero();

namespace callable {

    class less_sign_safe {
        // Default implementation.
        template <class Left, class Right>
            static auto apply (Left const & left, Right const & right,
                typename boost::disable_if <boost::mpl::or_ <
                    boost::mpl::and_ <
                        types::is_unsigned <Left>, types::is_signed <Right>>,
                    boost::mpl::and_ <
                        types::is_signed <Left>, types::is_unsigned <Right>>
                    >
                >::type * = 0)
        RETURNS (rime::less (left, right));

        // Unsigned < signed: consider special case of signed < 0.
        template <class Unsigned, class Signed>
            static auto apply (Unsigned const & u, Signed const & s,
                typename boost::enable_if <boost::mpl::and_ <
                    types::is_unsigned <Unsigned>, types::is_signed <Signed>>
                >::type * = 0)
        RETURNS (rime::and_ (
            !rime::less (s, rime::make_zero (s)),
            rime::less (u, rime::make_unsigned (s))));

        // Signed < unsigned: consider special case of signed < 0.
        template <class Signed, class Unsigned>
            static auto apply (Signed const & s, Unsigned const & u,
                typename boost::enable_if <boost::mpl::and_ <
                    types::is_signed <Signed>, types::is_unsigned <Unsigned>>
                >::type * = 0)
        RETURNS (rime::or_ (
            rime::less (s, rime::make_zero (s)),
            rime::less (rime::make_unsigned (s), u)));

        // GCC 4.6 workaround.
        template <class Left, class Right> struct result {
            typedef decltype (
                apply (std::declval <Left>(), std::declval <Right>())) type;
        };

    public:
        template <class Left, class Right>
            typename result <Left, Right>::type
            operator() (Left const & left, Right const & right) const
        { return apply (left, right); }
    };

} // namespace callable

static auto const less_sign_safe = callable::less_sign_safe();

} // namespace rime

#endif  // RIME_SIGN_HPP_INCLUDED

