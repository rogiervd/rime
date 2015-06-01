/*
Copyright 2011, 2012 Rogier van Dalen.

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
\file
Define a function with a purpose similar to the C++ switch statement.
However, it is limited to taking classes whose operator() is called.

The contents of this file are in the detail namespace because the interface
is too limited for general use.
*/

#ifndef RIME_DETAIL_SWITCH_HPP_INCLUDED
#define RIME_DETAIL_SWITCH_HPP_INCLUDED

#include <cassert>

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
template <typename ResultType, class Choices> struct switch_
: switch_ <ResultType, typename meta::as_vector <Choices>::type> {};

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

