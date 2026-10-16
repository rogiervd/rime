// SPDX-FileCopyrightText: Copyright 2013 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RIME_ASSERT_HPP_INCLUDED
#define RIME_ASSERT_HPP_INCLUDED

#include <cassert>

#include "core.hpp"

namespace rime {

namespace detail {

    template <class Condition, class Enable = void> struct assert_;

    template <class Condition> struct assert_<
        Condition,
        typename boost::enable_if<rime::is_constant<Condition>>::type>
    {
        void operator()(Condition const &) const
        {
            static_assert(Condition::value, "rime::assert_ failed");
        }
    };

    template <class Condition> struct assert_<
        Condition,
        typename boost::disable_if<rime::is_constant<Condition>>::type>
    {
        void operator()(Condition const & condition) const
        {
            assert(condition);
            // Avoid a warning about an unused parameter if NDEBUG is defined.
            (void) condition;
        }
    };

}  // namespace detail

template <class Condition> inline void assert_(Condition const & condition)
{
    detail::assert_<Condition> implementation;
    implementation(condition);
}

}  // namespace rime

#endif  // RIME_ASSERT_HPP_INCLUDED
