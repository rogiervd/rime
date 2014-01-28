/*
Copyright 2011, 2012 Rogier van Dalen.

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

/**
Dispatch functions to the correct specialisation for the actual contained type
for variants.

Every time I read this code, I think this is quite complicated, which is
probably an indication that its structure could be improved.
This file should be readable top to bottom, with the most important concepts
being introduced first.
*/

#ifndef RIME_DETAIL_VARIANT_DISPATCH_HPP_INCLUDED
#define RIME_DETAIL_VARIANT_DISPATCH_HPP_INCLUDED

#include <utility>
#include <type_traits>

#include <boost/detail/workaround.hpp>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/eval_if.hpp>

#include "meta/vector.hpp"
#include "meta/transform.hpp"
#include "meta/flatten.hpp"
#include "utility/storage.hpp"
#include "rime/detail/switch.hpp"

#include "rime/detail/variant_fwd.hpp"
#include "rime/detail/variant_helpers.hpp"

namespace rime {

// Forward definition
template <typename Function> class visitor;

/**
Return a function wrapper that determines at compile time what the actual
types are of any variant's passed in and calls the appropriate compile-time
defined function.
This is equivalent to the visitor pattern applied to boost::variant.
Essentially, it functions as a switch statement that applies a different
function depending on the actual content types of the variants.

Imagine that the call is
    f (a, b)
where f has type variant <F1,F2>, a has type int, and b has type
variant<char, long>.
The operator() on visitor then essentially needs to implement

    switch (f.which()) {
    case 0:
        switch (b.which()) {
        case 0:
            return get<F1>(f) (a, get<char> (b));
        case 1:
            return get<F1>(f) (a, get<long> (b));
        }
    case 1:
        switch (b.which()) {
        case 0:
            return get<F2>(f) (a, get<char> (b));
        case 1:
            return get<F2>(f) (a, get<long> (b));
        }
    }

A parametrisable switch is implemented in ./switch.hpp.
However, it does not allow nesting, so let's first turn this into an equivalent
linear switch:

    switch (f.which()*2 + b.which()) {
    case 0:
        return get <F1>(f) (a, get<char>(b));
    case 1:
        return get <F1>(f) (a, get<long>(b));
    case 2:
        return get <F2>(f) (a, get<char>(b));
    case 3:
        return get <F2>(f) (a, get<long>(b));
    }

This file will go through this process from the inside out.
1. Convert the argument to its actual type.
2. Call the actual function.
3. List all possible combinations of actual types.
And then, put all this together.
*/
template <typename Function>
    inline visitor <Function> visit (Function && function)
{ return visitor <Function> (std::forward <Function> (function)); }

namespace variant_detail {

/**
1. Convert the argument to its actual type.

When we know the actual type of a parameter, we need to convert it to that type.
In the case of a non-variant type, say int a, both the argument type and the
actual type are int.
In the case of variant <char, long> b, say the actual type is char, then we need
to call get <char>(b).

Convert one type to its actual type as returned by get if it is a variant,
and the original type if it is not.
*/
template <typename Argument, typename Actual>
    inline
    typename boost::enable_if <std::is_same <Argument, Actual>, Actual &&>::type
convert_to_actual (Argument && argument)
{ return std::forward <Argument> (argument); }

template <typename Variant, typename Actual>
    inline
    typename boost::enable_if <is_variant <Variant>,
        typename ::detail::storage::get_as <Actual, Variant>::type
    >::type
convert_to_actual (Variant && v)
{ return rime::get_unsafe <Actual> (std::forward <Variant> (v)); }

/**
2. Call the actual function.

We need to implement calls like
    get <F2>(f) (a, get<long>(b))
for which we get a list of arguments, say,
    variant <F1, F2>, int, variant <char, long>
as Arguments, and a list of actual types, say,
    F2, int, char
as ActualArguments.
The arguments must all be converted to the correct actual type, and then the
function must be called.
*/
template <typename Arguments, typename ActualArguments>
    struct dispatch_recipient;

// Implementations for actual arguments containing void, and not, respectively.
template <typename Arguments, typename ActualArguments>
    struct dispatch_recipient_no_void;
template <class Arguments, class ActualArguments>
class dispatch_recipient_remove_void;

template <typename Function, typename ... Arguments,
        typename ActualFunction, typename ... ActualArguments>
    struct dispatch_recipient_no_void <
        meta::vector <Function, Arguments ...>,
        meta::vector <ActualFunction, ActualArguments ...> >
{
    /*
    The recipients are instantiated for the first time from here.

    MESSAGE TO USER:
    If you get an error message here:
    */
    typedef decltype (
        convert_to_actual <Function, ActualFunction> (
            std::declval <Function &&>()) (
            convert_to_actual <Arguments, ActualArguments> (
                std::declval <Arguments &&>()) ...))
        result_type;
    /*
    then you are probably attempting to call a function through a variant
    visitor with non-matching parameters.
    Which function and what parameters?
    The error happens during the instantiation of this class, which you should
    be able to find in the error message (hopefully somwhere at the top).
    Say it is
        rime::variant_detail::dispatch_recipient_no_void <
            meta::vector <double (&)(int, float), rime::variant <int, char> >,
            meta::vector <double (&)(int, float), int> >.
    The first meta::vector indicates the compile-time function call, including
    variant types, and the second meta::vector the run-time function call, with
    the actual types.
    The first type, here "double (&)(int, float)", is the function type.
    The rest are the parameters to this function.

    Note that if you passed in arguments that can be void, even if they are not
    void at run time, then they may have been removed from the list of
    parameters, and they will be invisible here.
    In this example, the second parameter was variant <void, float>, which got
    removed to implement the case where at run time it contains void.
    In the above example, the function obviously takes two parameters, and
    receives only one.

    I hope that helps!
    */

    result_type operator() (
        Function && function, Arguments && ... arguments) const
    {
        return convert_to_actual <Function, ActualFunction> (
                std::forward <Function> (function)) (
            convert_to_actual <Arguments, ActualArguments> (
                std::forward <Arguments> (arguments)) ...);
    }
};

template <typename MergedResultType, class DispatchRecipient,
    typename RecipientResultType = typename DispatchRecipient::result_type>
struct convert_result_impl;
/**
Implicitly convert result type of callable function to MergedResultType.
This class is only necessary because "void" may need to be converted to
variant <..., void, ...>.
*/
template <typename MergedResultType, class DispatchRecipient>
struct convert_result
: convert_result_impl <MergedResultType, DispatchRecipient> {};

template <typename MergedResultType, class DispatchRecipient,
    typename RecipientResultType>
struct convert_result_impl
: DispatchRecipient {};

// Specialise for void -> variant <..., void, ...>.
template <typename MergedResultType, class DispatchRecipient>
    struct convert_result_impl <MergedResultType, DispatchRecipient, void>
{
    template <typename ... Arguments>
        MergedResultType operator() (Arguments && ... arguments) const
    {
        DispatchRecipient recipient;
        recipient (std::forward <Arguments> (arguments) ...);
        return MergedResultType();
    }
};

// The standard implementation works here: C++11 allows void functions to
// return an expression that is void itself.
template <class DispatchRecipient>
    struct convert_result_impl <void, DispatchRecipient, void>
: DispatchRecipient {};

/**
3. List all possible combinations of actual types.
Represent the list of all possible combinations of actual types that a list of
types including variants can be.
This list is at the basis of multiple dispatch.
For example,
    possible_actual_types_for <meta::vector <
        variant <int, long>, variant <bool, double> > >
will have as type:
meta::vector <
    meta::vector <F1, int, char>,
    meta::vector <F1, int, long>,
    meta::vector <F2, int, char>,
    meta::vector <F2, int, long> >.

get_index computes the index into the vector "choices" implied by the actual
types.
In the example, this was:
    f.which()*2 + b.which().
*/
template <typename> class possible_actual_types_for;

template <typename Head, typename ... Tail>
    class possible_actual_types_for <meta::vector <Head, Tail ...> >
{
private:
    /**
    Enumerate choices for the first type.
    E.g. meta::vector <F1, F2>.
    */
    typedef typename variant_types <Head>::type type_first;
    typedef possible_actual_types_for <meta::vector <Tail ...> > recurse;
    /**
    E.g. meta::vector <meta::vector <int, char>, meta::vector <int, long> >
    */
    typedef typename recurse::type types_rest;

    /**
    Compute index for the actual type of one argument.
    This is always 0 if the argument type is not a variant.
    */
    template <typename Type>
        static typename boost::disable_if <is_variant <Type>, std::size_t>::type
        which (Type &&)
    { return 0; };
    template <typename Type>
        static typename boost::enable_if <is_variant <Type>, std::size_t>::type
        which (Type && variant)
    { return variant.which(); };

    /**
    Compute actual types for one first actual type.
    E.g.
        meta::vector <
            meta::vector <F1, int, char>,
            meta::vector <F1, int, long> >.
    */
    template <typename FirstChoice> struct types_starting_with
    : meta::as_vector <meta::transform <
        meta::push <meta::front, FirstChoice, boost::mpl::_>,
        types_rest
    >>::type {};

public:
    /**
    Compute actual types for all possible first actual type.
    E.g.
        meta::vector <
            meta::vector <F1, int, char>,
            meta::vector <F1, int, long>,
            meta::vector <F2, int, char>,
            meta::vector <F2, int, long> >.
    */
    typedef typename meta::flatten <typename meta::as_vector <meta::transform <
            types_starting_with <boost::mpl::_1>, type_first
        >>::type>::type type;

    /**
    Compute the index into choices that gives the correct actual type.
    In the example above, this computes
        f.which()*2 + b.which().
    */
    static std::size_t get_index (Head && head, Tail && ... tail) {
        return which (std::forward <Head> (head))
                * meta::size <types_rest>::value
            + recurse::get_index (std::forward <Tail> (tail) ...);
    }
};
template <> class possible_actual_types_for <meta::vector<> > {
public:
    typedef meta::vector <meta::vector<> > type;
    static std::size_t get_index() { return 0; }
};

} // namespace variant_detail

/**
Function wrapper that enables multiple dispatch on its parameters and the
function itself.
*/
template <typename Function> class visitor {
    Function function;

    /**
    Dispatch a call containing variants to the appropriate recipient.
    This does the actual work.

    \internal
    By giving the types default values here, they are available as template
    packs in the specialisation that does the actual work.
    This reduces the number of instantiations and produces shorter errors.
    */
    template <typename Arguments,
        typename PossibleActualArguments
            = variant_detail::possible_actual_types_for <Arguments>,
        typename PossibleRecipients =
            typename meta::as_vector <meta::transform <
                variant_detail::dispatch_recipient <Arguments, boost::mpl::_>,
                typename PossibleActualArguments::type>>::type
        > struct variant_dispatcher;

    template <typename ... Arguments, typename PossibleActualArguments,
        typename ... PossibleRecipients>
        struct variant_dispatcher <
            meta::vector <Arguments ...>, PossibleActualArguments,
            meta::vector <PossibleRecipients ...> >
    {
        // The recipients are instantiated for the first time from here.
        typedef meta::vector <typename PossibleRecipients::result_type ...>
            possible_result_types;

        /**
        Merge the result types into one (potentially variant) type.
        */
        typedef typename make_variant_over <possible_result_types>::type
            result_type;

        result_type operator() (Arguments && ... arguments) const {
            std::size_t which = PossibleActualArguments::get_index (
                std::forward <Arguments> (arguments) ...);
            static rime::detail::switch_ <result_type,
                meta::vector <variant_detail::convert_result <
                    result_type, PossibleRecipients> ...> > s;
            return s (which, std::forward <Arguments> (arguments) ...);
        }
    };

public:
    visitor (Function && function) : function (function) {}

    template <typename ... Arguments>
    typename variant_dispatcher <meta::vector <Function, Arguments ...>
        >::result_type
    operator() (Arguments && ... arguments) {
        variant_dispatcher <meta::vector <Function, Arguments ...>>
            dispatcher;
        return dispatcher (std::forward <Function> (function),
            std::forward <Arguments> (arguments) ...);
    }

    template <typename ... Arguments>
    typename variant_dispatcher <meta::vector <Function const, Arguments ...>
        >::result_type
    operator() (Arguments && ... arguments) const {
        variant_dispatcher <meta::vector <Function const, Arguments ...>>
            dispatcher;
        return dispatcher (std::forward <Function const> (function),
            std::forward <Arguments> (arguments) ...);
    }
};

namespace variant_detail {

/*
Implementation of dispatch_recipient that can deal with actual types being void.
They get removed from the argument list.
*/

/**
Split the parallel lists of arguments and actual arguments on the first void
in the list of actual arguments.
The void and its original argument are removed.
*/
template <class Arguments, class Actual> class split_void;

template <typename FirstArgument, typename ... Arguments,
        typename FirstActual, typename ... Actual>
    class split_void <meta::vector <FirstArgument, Arguments ...>,
        meta::vector <FirstActual, Actual ...>>
{
    typedef split_void <meta::vector <Arguments...>, meta::vector <Actual...>>
        recursive_split;
public:
    /**
    Arguments before the first "void" in Actual.
    */
    typedef typename meta::push <FirstArgument,
        typename recursive_split::before_arguments>::type before_arguments;
    /**
    Arguments after the first "void" in Actual.
    */
    typedef typename recursive_split::after_arguments after_arguments;

    /**
    Actual with the first "void" removed.
    */
    typedef typename meta::push <FirstActual,
        typename recursive_split::new_actual>::type new_actual;
};

// Specialisation: actual parameters start with void.
template <typename FirstArgument, typename ... Arguments, typename ... Actual>
    class split_void <meta::vector <FirstArgument, Arguments ...>,
        meta::vector <void, Actual ...>>
{
public:
    typedef meta::vector<> before_arguments;
    typedef meta::vector <Arguments ...> after_arguments;
    // Leave out void
    typedef meta::vector <Actual ...> new_actual;
};

template <> class split_void <meta::vector<>, meta::vector<>> {
public:
    typedef meta::vector<> before_arguments;
    typedef meta::vector<> after_arguments;

    typedef meta::vector<> new_actual;
};

template <class Arguments, class Actual,
    class SplitVoid = split_void <Arguments, Actual>,
    class BeforeArguments = typename SplitVoid::before_arguments,
    class AfterArguments = typename SplitVoid::after_arguments>
class dispatch_recipient_remove_void_fast;

template <class Arguments, class Actual, class SeenArguments, class SeenActual>
class dispatch_recipient_remove_void_slow;

template <class Arguments, class Actual>
class dispatch_recipient_remove_void
#if (BOOST_CLANG && __clang_major__ == 3 && __clang_minor__ == 0)
: public dispatch_recipient_remove_void_slow <
    Arguments, Actual, meta::vector<>, meta::vector<>> {};
#else
: public dispatch_recipient_remove_void_fast <Arguments, Actual> {};
#endif

/**
Implementation of dispatch_recipient that removes arguments that at run time
have turned out to be "void".

This works recursively, by removing one void at a time.
There will be one instantiation of this class for every "void".

This implementation does not work on CLang 3.0.
*/
template <class Arguments, class Actual, class SplitVoid,
    typename ... BeforeArguments, typename ... AfterArguments>
class dispatch_recipient_remove_void_fast <Arguments, Actual, SplitVoid,
    meta::vector <BeforeArguments...>, meta::vector <AfterArguments...>>
{
    typedef typename SplitVoid::new_actual new_actual;
    typedef dispatch_recipient <
        meta::vector <BeforeArguments..., AfterArguments...>, new_actual>
        recursive_type;
public:
    typedef typename recursive_type::result_type result_type;

    template <class VoidVariant> result_type operator() (
        BeforeArguments && ... before_arguments, VoidVariant && void_variant,
        AfterArguments && ... after_arguments) const
    {
        // Trigger assertion that void_variant really contains void.
        get_unsafe <void> (void_variant);

        recursive_type recursive_call;
        // Forward arguments, leaving out void_variant.
        return recursive_call (
            std::forward <BeforeArguments> (before_arguments) ...,
            std::forward <AfterArguments> (after_arguments) ...);
    }
};

/**
Implementation of dispatch_recipient that removes arguments that at run time
have turned out to be "void".

This works recursively, by going through all arguments.
This leads to as many instantiations as there are arguments.

This implementation does work on CLang 3.0.
*/
template <typename FirstArgument, typename ... Arguments,
    typename FirstActual, typename ... Actual,
    typename ... SeenArguments, typename ... SeenActual>
class dispatch_recipient_remove_void_slow <
    meta::vector <FirstArgument, Arguments...>,
    meta::vector <FirstActual, Actual...>,
    meta::vector <SeenArguments...>, meta::vector <SeenActual...>>
{
    typedef dispatch_recipient_remove_void_slow <
        meta::vector <Arguments...>, meta::vector <Actual...>,
        meta::vector <SeenArguments..., FirstArgument>,
        meta::vector <SeenActual..., FirstActual>> recursive_type;
public:
    typedef typename recursive_type::result_type result_type;

    template <typename ... OtherArguments> result_type operator() (
        FirstArgument && first_argument, OtherArguments && ... other_arguments)
    const
    {
        recursive_type recursive_call;
        return recursive_call (
            std::forward <OtherArguments> (other_arguments) ...,
            std::forward <FirstArgument> (first_argument));
    }
};
// First is void: drop argument.
template <typename FirstArgument, typename ... Arguments,
    typename ... Actual,
    typename ... SeenArguments, typename ... SeenActual>
class dispatch_recipient_remove_void_slow <
    meta::vector <FirstArgument, Arguments...>,
    meta::vector <void, Actual...>,
    meta::vector <SeenArguments...>, meta::vector <SeenActual...>>
{
    typedef dispatch_recipient_remove_void_slow <
        meta::vector <Arguments...>, meta::vector <Actual...>,
        meta::vector <SeenArguments...>,
        meta::vector <SeenActual...>> recursive_type;
public:
    typedef typename recursive_type::result_type result_type;

    template <typename ... OtherArguments> result_type operator() (
        FirstArgument && first_argument, OtherArguments && ... other_arguments)
    const
    {
        // Assert that the first argument really contains void.
        get_unsafe <void> (first_argument);

        recursive_type recursive_call;
        // Forward arguments, leaving out the first argument.
        return recursive_call (
            std::forward <OtherArguments> (other_arguments) ...);
    }
};

template <class SeenArguments, class SeenActual>
class dispatch_recipient_remove_void_slow <
    meta::vector <>, meta::vector <>, SeenArguments, SeenActual>
: public dispatch_recipient_no_void <SeenArguments, SeenActual> {};

template <typename Arguments, typename ActualArguments>
    struct dispatch_recipient
: boost::mpl::if_ <meta::contains <void, ActualArguments>,
    dispatch_recipient_remove_void <Arguments, ActualArguments>,
    dispatch_recipient_no_void <Arguments, ActualArguments>
    >::type {};

} // namespace variant_detail

} // namespace rime

#endif  // RIME_DETAIL_VARIANT_DISPATCH_HPP_INCLUDED

