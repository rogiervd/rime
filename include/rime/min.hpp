/*
Copyright 2013 Rogier van Dalen.

This file is part of Rogier van Dalen's Range library for C++.

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

#ifndef RIME_MIN_HPP_INCLUDED
#define RIME_MIN_HPP_INCLUDED

#include "core.hpp"
#include "if.hpp"
#include "sign.hpp"

#include "utility/returns.hpp"

namespace rime {
namespace callable {

    struct min {
        template <class Left, class Right>
            auto operator() (Left && left, Right && right) const
        RETURNS (rime::if_ (rime::less_sign_safe (left, right),
            std::forward <Left> (left), std::forward <Right> (right)))
    };

} // namespace callable

/**
Return the minimum of two values, as a compile-time constant if possible.
*/
static const auto min = callable::min();

} // namespace rime

#endif  // RIME_MIN_HPP_INCLUDED

