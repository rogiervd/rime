/*
Copyright 2014 Rogier van Dalen.

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

#ifndef RIME_ALWAYS_HPP_INCLUDED
#define RIME_ALWAYS_HPP_INCLUDED

#include <type_traits>
#include <boost/utility/enable_if.hpp>

#include "core.hpp"

namespace rime {

namespace callable {

    /**
    Function object that always returns compile-time value \a value, of type
    \a Result, whatever arguments it is called with.
    \tparam Result The type of the return value.
    \param value The value.
    */
    template <class Result, Result value>
    struct always {
        template <class ... Arguments>
            Result operator() (Arguments const & ...) const { return value; }
    };

    /**
    Function object that always returns a default-constructed object of type
    \a Result, whatever arguments it is called with.
    \tparam Result The type of the return value.
    */
    template <class Result> struct always_default {
        template <class ... Arguments>
            Result operator() (Arguments const & ...) const { return Result(); }
    };

    /**
    Function object that always returns a value of type \a Result, whatever
    arguments it is called with.
    \tparam Result The type of the return value.
    */
    template <class Result>
    struct always_value {
        Result value;
    public:
        /**
        Construct an object of this class.
        \param value The value that this should always return when it is called.
        */
        template <class Argument>
            always_value (Argument && value)
        : value (std::forward <Argument> (value)) {}

        template <class ... Arguments>
            Result operator() (Arguments const & ...) const { return value; }
    };

} // namespace callable

/**
\return
    A function object that always returns the default-constructed object of type
    \a Result, whatever arguments it is called with.
\tparam Result The type returned by the function object.
*/
template <class Result>
    inline callable::always_default <Result> always()
{ return callable::always_default <Result>(); }

/**
\return
    A function object that always returns value \a value of type \a Result,
    whatever arguments it is called with.
    The value is given at compile time.
\tparam Result The type returned by the function object.
\tparam value The value returned by the function object.
*/
template <class Result, Result value>
    inline callable::always <Result, value> always()
{ return callable::always <Result, value>(); }

/**
\return
    A function object that always returns value \a value of type \a Result,
    whatever arguments it is called with.
    The value is given at run time.
\tparam Result The type returned by the function object.
\tparam value The value returned by the function object.

\note
It is not normally necessary to supply \a Result explicitly.
For example, <c>always (5)</c> will return a function object that always returns
an int.
However, if the returned value must be cv-qualified or be a reference, \a Result
can be specified explicitly.

\internal
Because <c>int & const</c> is just <c>int &</c>, this works if \a Result is a
reference type.
*/
template <class Result>
    inline callable::always_value <Result> always (Result const & value)
{ return callable::always_value <Result> (value); }

} // namespace rime

#endif // RIME_ALWAYS_HPP_INCLUDED
