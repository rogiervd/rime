/*
Copyright 2011, 2012 Rogier van Dalen.

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

/**
\file
Define a function with a purpose similar to the C++ switch statement.
However, it is limited to taking classes whose operator() is called.

The contents of this file are in the detail namespace because the interface
is too limited for general use.
*/

#ifndef RIME_DETAIL_SWITCH_HPP_INCLUDED
#define RIME_DETAIL_SWITCH_HPP_INCLUDED

#include "meta/vector.hpp"
#include "rime/core.hpp"

namespace rime { namespace detail {

template <class Choice, typename Result, typename ... Arguments>
    Result call_object (Arguments && ... arguments)
{
    Choice choice;
    return choice (std::forward <Arguments> (arguments) ...);
}

/**
Helper for switch_ which knows the argument types at construction time.
It provides a table of function pointers, which the constructor initialises.
A neater version would use recursion.
However, by expanding the parameter pack in Choices at once, the length of
backtraces in error messages becomes bounded.
*/
template <typename Result, class Arguments, class Choices> struct switch_impl;

template <typename Result, typename ... Arguments, class ... Choices>
    struct switch_impl <Result, meta::vector <Arguments ...>,
        meta::vector <Choices ...> >
{
    switch_impl()
    { set_pointers (&functions[0], get_function <Choices>() ...); }

    typedef Result (*function_pointer) (Arguments && ...);

    static const std::size_t choice_num
        = meta::size <meta::vector <Choices ...>>::value;
    function_pointer functions [choice_num];

private:
    template <class Choice> function_pointer get_function() const
    { return  &call_object <Choice, Result, Arguments ...>; };

    template <class FirstPointer, class ... OtherPointers>
        static void set_pointers (function_pointer * destination,
            FirstPointer first_pointer, OtherPointers ... other_pointers)
    {
        *destination = first_pointer;
        set_pointers (destination + 1, other_pointers ...);
    }

    static void set_pointers (function_pointer *) {}
};

/**
A class that functions like a switch statement.
The cases are defined at compile-time by a meta::vector of classes.
At run-time, an integer determines which class gets instantiated and called.

This class is optimised to reduce the length of error messages.
The interface is too basic for exposure to the world at large.

\todo This uses a static switch_impl object inside operator().
What does this mean for performance?
Should this be replaced by a big telescoping switch statement?
*/
template <typename ResultType, class Choices> struct switch_;

template <typename ResultType, typename ... Choices>
    struct switch_ <ResultType, meta::vector <Choices ...> >
{
    template <typename ... Arguments>
        ResultType operator() (std::size_t which, Arguments && ... arguments)
        const
    {
        typedef switch_impl <ResultType, meta::vector <Arguments...>,
            meta::vector <Choices ...> > implementation_type;
        assert (which < implementation_type::choice_num);
        static implementation_type implementation;
        return implementation.functions [which] (
            std::forward <Arguments> (arguments) ...);
    }
};

}} // namespace rime::detail

#endif  // RIME_DETAIL_SWITCH_HPP_INCLUDED

