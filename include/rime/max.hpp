// SPDX-FileCopyrightText: Copyright 2013, 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RIME_MAX_HPP_INCLUDED
#define RIME_MAX_HPP_INCLUDED

#include "core.hpp"
#include "if.hpp"
#include "sign.hpp"

#include "utility/returns.hpp"

namespace rime {

namespace callable {
    template <class MergePolicy = merge_policy::default_policy> struct max
    {
        template <class Left, class Right>
        auto operator()(Left && left, Right && right) const RETURNS(
            rime::if_<MergePolicy>(
                rime::less_sign_safe(left, right), std::forward<Right>(right),
                std::forward<Left>(left)));
    };
}  // namespace callable

/**
Return the maximum of two values, as a compile-time constant if possible.
*/
static auto const max = callable::max<>();

/**
Return the maximum of two values, as a compile-time constant if possible.
If the versions with and without merge policy have the same name, compile errors
occur when the overloads are resolved.
Therefore, this has an underscore in the name.
*/
template <class MergePolicy, class Left, class Right>
inline auto max_(Left && left, Right && right) RETURNS(
    callable::max<MergePolicy>()(
        std::forward<Left>(left), std::forward<Right>(right)));

}  // namespace rime

#endif  // RIME_MAX_HPP_INCLUDED
