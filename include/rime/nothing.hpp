// SPDX-FileCopyrightText: Copyright 2014 Rogier van Dalen
//
// SPDX-License-Identifier: Apache-2.0

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
    struct nothing
    {
        template <class... Arguments>
        void operator()(Arguments const &...) const
        {}
    };

}  // namespace callable

/**
Do nothing.

This is a function object that can be called with anything, and it will never
perform any action, and will always return void.
This can be useful, for example, as argument to rime::call_if.
*/
static auto const nothing = callable::nothing();

}  // namespace rime

#endif  // RIME_NOTHING_HPP_INCLUDED
