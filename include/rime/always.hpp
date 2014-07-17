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
