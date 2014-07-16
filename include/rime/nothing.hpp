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

#ifndef RIME_NOTHING_HPP_INCLUDED
#define RIME_NOTHING_HPP_INCLUDED

namespace rime {

namespace callable {

    /**
    Functor that does nothing when called.
    Its <c>operator()</c> takes any arguments, always returns void, and never
    performs any action.
    This can be useful, for example, as argument to rime::call_if.
    */
    struct nothing {
        template <class ... Arguments>
            void operator() (Arguments const & ...) const {}
    };

} // namespace callable

/**
Do nothing.

This is a function object that can be called with anything, and it will never
perform any action, and will always return void.
This can be useful, for example, as argument to rime::call_if.
*/
static const auto nothing = callable::nothing();

} // namespace rime

#endif // RIME_NOTHING_HPP_INCLUDED
