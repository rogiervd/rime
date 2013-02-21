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
Define a variant type, similar to boost::variant.
*/

#ifndef RIME_VARIANT_HPP_INCLUDED
#define RIME_VARIANT_HPP_INCLUDED

#include <utility>
#include <stdexcept>

#include <type_traits>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/int.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/greater_equal.hpp>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/bind.hpp>

#include "utility/storage.hpp"
#include "utility/aligned_union.hpp"

#include "meta/transform.hpp"
#include "meta/filter.hpp"
#include "meta/vector.hpp"
#include "meta/enumerate.hpp"
#include "meta/flatten.hpp"
#include "meta/contains.hpp"
#include "meta/max_element.hpp"
#include "rime/detail/merge_types.hpp"
#include "rime/core.hpp"

#include "rime/detail/switch.hpp"

#include "rime/detail/variant_fwd.hpp"
#include "rime/detail/variant_helpers.hpp"
#include "rime/detail/variant_dispatch.hpp"
#include "rime/detail/variant_operator.hpp"

namespace rime
{

struct bad_get : std::runtime_error {
    bad_get()
    : std::runtime_error (
        "get (variant<...>) called with a type that was not contained") {}
};

namespace variant_detail
{
    /**
    Find the best match for the Actual type amongst the VariantTypes.
    */
    template <typename Actual, typename NumberedTypes>
        struct find_interpretation;

    /**
    Find the best candidates for the Actual type amongst the NumberedCandidates.
    This tries to match candidates by a number of criteria.
    If multiple options are equally good, all of them are returned.
    If no candidate is found, then meta::vector<> is returned.
    */
    template <typename Actual, typename NumberedTypes, typename ... Matches>
        struct find_candidates;

    template <typename Actual, typename Candidates>
        struct assert_unambiguous_conversion;

    template <typename Base> struct merge_constant;

    /**
    Metafunction that produces a variant <...> over the types in Types.
    */
    template <typename Types> struct make_variant;

    template <typename Actual> struct get;

}   // namespace variant_detail

/**
A variant type: it contains exactly one of the types from Types.
It is known only at run-time which type it contains.
It is optimised to be used as a return type.
The aim is for the variant to behave like its contents as much as possible.

The goal of this variant is different from boost::variant.
boost::variant is Assignable.
This means that an exception can be thrown while one type is being replaced
with another, and for boost::variant to guarantee it is non-empty is hard.

rime::variant, on the other hand, cannot change types, once it is initialised.
It fulfills the never-empty guarantee by not changing content type once its
constructor has finished without throwing an exception.
As a consequence, rime::variant is therefore not Assignable.
The assignment operator forwards to any type the variant can contain.

Since the variant type behaves as much as its return type as possible, it
differentiates between reference and non-reference types, and const
qualification.
When a variant is copy-constructed from a variant with the same variant type,
the contained type may change.
For example:

rime::variant <int, int&> v (6);    // v contains type "int".
rime::variant <int, int&> v2 (v);   // v2 contains type "int &".

However, when a variant is move-constructed (as should be the case in return
statements), the type doesn't change.
(This is an implied property, not an explicit one.)

Note that since variant is not Assignable, the requirements for assignment are
different from the requirements for copy-construction from a variant.
For construction from a variant, whether the same type or another, all possible
content types of the source must be convertible to exactly one of the content
types of the target.
The assignment operator requires that all of the source content types can be
assigned to all of the target content types.

Unary operators are overloaded so that they work as if on the contained types,
and return the unified type (i.e. possibly a variant) of the return types that
the operator returns for the contained types.
The following operators are not overloaded:
    operator& (get <...> (&v) would not work any more)
    operator -> and operator ->* (unclear what the return type should be).

Binary operators are overloaded if either the left or right operand is a
variant.
The following operators are not overloaded:
    operator&& and operator|| (short-circuiting would not be possible)
    operator, (makes no sense in the usual setting)
*/
template <typename ... Types> class variant
{
public:
    typedef meta::vector <Types...> types;
    typedef typename meta::as_vector <typename meta::enumerate <types>::type
        >::type numbered_types;

private:
    template <typename Type> struct sanity_check {
        typedef int dummy;

        static_assert (!is_variant <Type>::value,
            "variant<...> cannot contain a variant<..>.");

        typedef typename meta::filter <
                std::is_same <Type, boost::mpl::_>, types>::type
            duplicate_types;

        static_assert (meta::size <duplicate_types>::value == 1,
            "Type can only appear in the list of variant types once");
    };

    typedef meta::vector <typename sanity_check <Types>::dummy ...>
        trigger_sanity_check;

    std::size_t which_;

    // Set up storage size and alignment
    typedef typename meta::filter <
            mpl::not_ <std::is_same <boost::mpl::_, void> >, types
        >::type types_without_void;

    // Using transform_view does not influence compile speed noticeably.
    typedef typename meta::transform <
            ::detail::storage::store_as <boost::mpl::_>, types_without_void
        >::type stored_types;

    typedef typename ::detail::storage::aligned_union <stored_types>::type
        storage_type;
    storage_type storage;

    template <typename Actual> friend struct variant_detail::get;
    void * memory() { return &storage; }
    void const * memory() const { return &storage; }

    /**
    Find the best match for Actual.
    This produces a list of equally-likely candidates, in "candidates".
    If this list is empty, a static assertion is triggered.
    Use assert_unambiguous::dummy (a type that evaluates to int) as soon as
    possible in the call stack to trigger that static assertion and one for
    an ambiguous conversion.
    */
    template <typename Actual> struct conversion_for {

        // Match the second type of meta::vector <Index, Type>
        typedef mpl::second <mpl::_> _;

        typedef typename variant_detail::find_candidates <
            Actual, numbered_types,
            // Go through matches one by one:
            // Exact match
            std::is_same <_, Actual>,
            // Remove reference from Actual
            std::is_same <_, typename std::remove_reference <Actual>::type >,
            // Remove reference from Actual and remove const-qualification
            std::is_same <std::remove_const <_>, typename std::remove_const <
                typename std::remove_reference <Actual>::type>::type >,
            // Convertible in general?
            boost::mpl::and_ <std::is_convertible <Actual, _>,
                boost::mpl::not_ <std::is_reference <_> > >
        >::type numbered_candidates;

        static const bool conversion_possible =
            (meta::size <numbered_candidates>::value >= 1);

        static_assert (conversion_possible,
            "No conversion to variant <...> found. "
            "For details, see the type: "
            "rime::variant <...>::conversion_for <(type passed in)>.");

        /**
        This class encapsulates an assertion similar to the one above.
        It is in a class so that the error message shows the candidate targets
        that are being considered.

        The assertion is phrased as <= 1 so that either the one above is
        triggered, or the one above, so as to not clutter the compiler output
        even more.
        */
        template <typename Candidates> struct assert_unambiguous_conversion {
            typedef int dummy;
            static_assert (meta::size <Candidates>::value <= 1,
                "The conversion to the variant type is ambiguous: "
                "multiple types are equally good. "
                "For details, see the type: "
                "rime::variant <...>::conversion_for <(type passed in)>"
                "::assert_unambiguous_conversion <"
                "meta::vector <(possible candidates)>>."
                );
        };

        // For a clearer error message
        typedef typename meta::transform <
            mpl::second <mpl::_>, numbered_candidates>::type candidates;

        typedef assert_unambiguous_conversion <candidates> assert_unambiguous;
    };

    /**
    Find the best match for the contained types in OtherVariant.
    This can only be used to trigger early static assertions when converting
    from variant types.
    Use dummy (a type that evaluates to int) as soon as possible in the call
    stack to trigger a compiler error.
    */
    template <typename OtherVariant,
        class ContainedTypes = typename variant_types <OtherVariant>::type>
    struct conversion_for_contained_types;

    template <typename OtherVariant, typename ... ContainedTypes>
        struct conversion_for_contained_types <
            OtherVariant, meta::vector <ContainedTypes...>>
    {
        template <typename ... Arguments> struct int_ { typedef int type; };

        typedef typename int_ <typename conversion_for <typename
            ::detail::storage::get_as <
            ContainedTypes, OtherVariant>::type>::
            assert_unambiguous::dummy...>::type dummy;
    };

    /**
    Construct from another type.
    The other type should not be a variant, but instead a type extracted from
    it.
    This function compiles happily even if the conversion is ambiguous.
    This keeps the compiler errors from being cluttered.
    However, by using conversion_for, a static assertion should be triggered.

    There is another version of this below for the case when no conversion is
    found at all.
    This is just, again, because of the jumbled compiler errors this would
    generate.
    */
    template <typename Actual, typename NumberedCandidates
        = typename conversion_for <Actual>::numbered_candidates>
    void construct (Actual && actual, typename
        boost::disable_if <meta::empty <NumberedCandidates>>::type * = 0)
    {
        typedef typename meta::first <NumberedCandidates>::type
            interpretation;

        static const std::size_t index =
            mpl::first <interpretation>::type::value;
        typedef typename mpl::second <interpretation>::type type;
        typedef typename ::detail::storage::store_as <type>::type store_type;

        static_assert (sizeof (store_type) <= sizeof (storage),
            "Sanity check: there should be enough space to contain type");
        static_assert (alignof (store_type) <= alignof (storage),
            "Sanity check: the alignment should be great enough for type");

        // Set up index
        this->which_ = index;
        // Copy-construct or move-construct as type "store_type"
        new (memory()) store_type (std::forward <Actual> (actual));
    }

    /**
    Construct from a type when this is impossible.
    Since this is impossible, this is not implemented.
    Therefore, it does not produce compiler errors, which reduces clutter.
    */
    template <typename Actual, typename NumberedCandidates
        = typename conversion_for <Actual>::numbered_candidates>
    void construct (Actual && actual, typename
        boost::enable_if <meta::empty <NumberedCandidates>>::type * = 0);

    void construct_void() {
        static_assert (meta::contains <void, types>::value,
            "Attempt to void-construct a variant "
            "that cannot contain a void value. "
            "This is caused by calling the nullary constructor "
            "or by copy-constructing from a variant that can contain void.");

        this->which_ = index_of <void>::value;
        // Nothing needs to be stored.
    }

public:
    /* Construction */
    /**
    Construct from an object that is convertible to one (and only one) of the
    types that this variant can contain.
    This constructor is defined even when there is no conversion or when there
    are multiple conversions, so that a meaninful error message can be produced.

    \todo Document exact conversion strategy.
    */
    template <typename Actual>
        variant (Actual && actual,
            // Make sure that the copy constructor is picked up.
            typename boost::disable_if <is_variant <Actual> >::type * = 0,
            // Trigger assertion here already.
            // This keeps the backtrace in the compiler error shortish.
            int = typename conversion_for <Actual>::assert_unambiguous::dummy())
    { construct (std::forward <Actual> (actual)); }

    explicit variant() { construct_void(); }

private:
    /**
    Perform copy construction.
    This is called when it turns out, at run time, that that_variant contains an
    object of type Actual.
    */
    template <typename Actual, typename Dummy = void>
        struct construct_from_variant_containing
    {
        template <class ThatVariant> void operator() (
            variant & this_variant, ThatVariant && that_variant/*,
            // Trigger assertion here already.
            // This keeps the backtrace in the compiler error shortish.
            int = conversion_for <Actual>::assert_unambiguous::dummy()*/)
            const
        {
            this_variant.construct (get_unsafe <Actual> (
                std::forward <ThatVariant> (that_variant)));
        }
    };
    template <typename Dummy>
        struct construct_from_variant_containing <void, Dummy>
    {
        template <class ThatVariant> void operator() (
            variant & this_variant, ThatVariant && that_variant) const
        {
            // Assert that that_variant contains void.
            get_unsafe <void> (std::forward <ThatVariant> (that_variant));
            this_variant.construct_void();
        }
    };

    template <typename ThatVariant>
        void construct_from_other_variant (ThatVariant && that,
            // Trigger assertion here.
            int = typename conversion_for_contained_types <ThatVariant>
                ::dummy())
    {
        /*
        This constructs an object of type
            construct_from_variant_containing <Actual>,
        and calls it with (*this, that).
        */
        typedef typename meta::transform <
                construct_from_variant_containing <boost::mpl::_>,
                typename variant_types <ThatVariant>::type
            >::type specialisations;
        ::rime::detail::switch_ <void, specialisations> s;
        s (that.which(), *this, std::forward <ThatVariant> (that));
    }

public:
    // Explicitly enumerate the copy and move constructors to minimise
    // ambiguity.

    /// Copy constructor
    variant (variant const & that)
    { construct_from_other_variant (that); }

    /// Generalised copy constructor
    template <typename ThatVariant>
        variant (ThatVariant const & that,
            typename boost::enable_if <is_variant <ThatVariant> >::type * = 0)
    { construct_from_other_variant (that); }

    /// Move constructor
    variant (variant && that)
    { construct_from_other_variant (std::forward <variant> (that)); }

    /// Generalised move constructor
    template <typename ThatVariant>
        variant (ThatVariant && that,
            typename boost::enable_if <boost::mpl::and_ <
                is_variant <ThatVariant>,
                boost::mpl::not_ <std::is_reference <ThatVariant> >
            > >::type * = 0)
    { construct_from_other_variant (std::forward <ThatVariant> (that)); }

    /* Destruction */
private:
    /**
    Destruct the object that "this" contains.
    This is called when it turns out, at run time, that "this" contains an
    object of type Actual.
    */
    template <typename Actual, typename Dummy = void> struct destruct {
        void operator() (void * memory) const
        {
            typedef typename ::detail::storage::store_as <Actual>::type
                stored_type;
            static_cast <stored_type *> (memory)->~stored_type();
        }
    };
    // void is not stored and does not need to be destructed.
    template <typename Dummy> struct destruct <void, Dummy>
    { void operator() (void *) const {}; };

public:
    ~variant()
    {
        /*
        This constructs an object of type destruct <Actual>,
        and calls it with this->memory().
        */
        typedef typename meta::transform <
            destruct <boost::mpl::_>, types>::type specialisations;
        ::rime::detail::switch_ <void, specialisations> s;
        s (this->which(), this->memory());
    }

    /**
    \return The index, in the template parameter list, of the contained type.
    */
    std::size_t which() const { return which_; }

    /**
    Find the index of type Actual amongst the possible types of this variant.
    */
    template <typename Actual> struct index_of {
        typedef typename meta::filter <
                std::is_same <mpl::second <mpl::_>, Actual>,
                numbered_types
            >::type candidates;

        typedef typename mpl::front <candidates>::type index_and_type;

        static_assert (std::is_same <
            typename mpl::second <index_and_type>::type, Actual>::value,
            "Sanity check: should have found Actual");

        static const std::size_t value
            = mpl::first <index_and_type>::type::value;
    };

    /**
    \return true iff type Actual is contained.
    */
    template <typename Actual>
        typename boost::enable_if <meta::contains <Actual, types>, bool>::type
    contains() const
    { return (this->which() == index_of <Actual>::value); }

    /* Assignment */
private:
    /**
    Perform assignment on the object that "this" contains.
    */
    struct assign {
        template <typename ThisActual, typename That>
            void operator() (ThisActual && this_actual, That && that) const
        { this_actual = std::forward <That> (that); }
    };

public:
    /**
    rime::variant is not Assignable.
    This is the consequence of the never-empty guarantee.
    Its assignment operator calls the assignment operator on the contained
    object.
    That implies that each of the Types ... must allow assignment from That.
    If That is a variant, then the assignment operator of the type contained
    in this will be called with the type contained in the variant.

    \return *this (not the merged return types of all separate assignment
        operators).
    */
    template <class That> variant & operator = (That && that) {
        // Forward to assign.
        visit (assign()) (*this, std::forward <That> (that));
        return *this;
    }

#define RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT(name, operation) \
private: \
    struct name { \
        template <typename ThisActual, typename That> \
            void operator() (ThisActual && this_actual, That && that) const \
        { this_actual operation std::forward <That> (that); } \
    }; \
    \
public: \
    template <class That> variant & operator operation (That && that) { \
        visit (name()) (*this, std::forward <That> (that)); \
        return *this; \
    }

    RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT(plus_assign, +=)
    RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT(minus_assign, -=)
    RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT(times_assign, *=)
    RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT(divide_assign, /=)
    RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT(modulo_assign, %=)
    RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT(bitwise_and_assign, &=)
    RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT(bitwise_or_assign, |=)
    RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT(bitwise_xor_assign, ^=)
    RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT(bitwise_shift_left_assign, <<=)
    RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT(bitwise_shift_right_assign, >>=)

#undef RIME_VARIANT_DEFINE_COMPOUND_ASSIGNMENT

    /* Call operator */
private:
    /**
    Call the object that "this" contains with the parameters given
    This is called when it turns out, at run time, that "this" contains an
    object of type Actual.
    The class Variant is passed in so it can be const-qualified.
    */
    template <typename Variant, typename Actual, typename ... Arguments>
        struct call_variant_containing
    {
        typedef decltype (get_unsafe <Actual> (std::declval <Variant>()) (
            std::declval <Arguments>() ...)) result_type;

        result_type operator() (
            Variant && variant, Arguments && ... arguments) const
        {
            return get_unsafe <Actual> (
                std::forward <Variant> (variant)) (arguments ...);
        }
    };

    template <typename ... Callers> struct call_variant_specialisations_impl {
        typedef meta::vector <Callers ...> callers;
        typedef meta::vector <typename Callers::result_type ...> result_types;
        typedef typename make_variant_over <result_types>::type result_type;
    };

    template <typename Variant, typename ... Arguments>
        struct call_variant_specialisations
    : call_variant_specialisations_impl <
        // Compute a call_variant_containing for each possible actual type.
        call_variant_containing <Variant, Types, Arguments ...> ...> {};

    template <typename Variant, typename ... Arguments>
    static typename call_variant_specialisations <Variant, Arguments...>
        ::result_type
        call_with (Variant variant, Arguments && ... arguments)
    {
        /*
        This constructs an object of type
            call_variant_containing <Variant, Actual, Arguments...>,
        and calls it with *this, arguments... .
        */
        typedef call_variant_specialisations <Variant, Arguments ...>
            compute_specialisations;
        // Specialisations with different result types.
        // These could be used directly if "void" wasn't a possible return type.
        typedef typename compute_specialisations::callers specialisations;
        // Unified result type.
        typedef typename compute_specialisations::result_type result_type;
        // Specialisations with the result type converted to result_type.
        typedef typename meta::transform <
            variant_detail::convert_result <result_type, boost::mpl::_>,
            specialisations>::type coerced_specialisations;

        static rime::detail::switch_ <result_type, coerced_specialisations> s;
        return s (variant.which(),
            std::forward <Variant> (variant),
            std::forward <Arguments> (arguments) ...);
    }

public:
    /**
    Call the function contained in the variant with arguments.
    Each of the possible contained types must be callable with these arguments.
    */
    template <typename ... Arguments>
        typename call_variant_specialisations <variant &, Arguments...
            >::result_type
        operator() (Arguments && ... arguments)
    {
        return call_with <variant &> (
            *this, std::forward <Arguments> (arguments) ...);
    }

    /**
    Call the function contained in the variant with arguments.
    Each of the possible contained types must be callable with these arguments.
    This is for const variant, so the function will also be const-qualified.
    */
    template <typename ... Arguments>
        typename call_variant_specialisations <variant const &, Arguments...
            >::result_type
        operator() (Arguments && ... arguments) const
    {
        return call_with <variant const &> (
            *this, std::forward <Arguments> (arguments) ...);
    }

    /* operator[] */
private:
    struct subscript {
        template <typename Actual, typename Argument>
        decltype (std::declval <Actual &&>() [std::declval <Argument &&>()])
            operator() (Actual && actual, Argument && argument) const
        { return std::forward <Actual> (actual) [
            std::forward <Argument> (argument)]; }
    };

    // This class is necessary to wrap decltype() in GCC 4.6.
    template <typename Variant, typename Argument> struct subscript_with
    {
        typedef decltype (visit (subscript()) (
            std::declval <Variant &>(), std::declval <Argument &&>()))
            result_type;

        result_type operator() (Variant & variant, Argument && argument) const {
            return visit (subscript()) (
                variant, std::forward <Argument> (argument));
        }
    };

public:
    /**
    Forward to the contained type's operator[].
    */
    template <class Argument> typename
        subscript_with <variant, Argument>::result_type
        operator [] (Argument && argument)
    {
        subscript_with <variant, Argument> implementation;
        return implementation (*this, std::forward <Argument> (argument));
    }

    template <class Argument> typename
        subscript_with <variant const, Argument>::result_type
        operator [] (Argument && argument) const
    {
        subscript_with <variant const, Argument> implementation;
        return implementation (*this, std::forward <Argument> (argument));
    }
};

namespace variant_detail {
    /**
    Unsafe implementaton of get.

    This is a struct and not a function, so that it is possible to specialise it
    for get <void>.
    */
    template <typename Actual> struct get {
        template <class Variant>
            typename ::detail::storage::get_as <Actual, Variant>::type
            operator() (Variant && variant) const
        {
            assert (variant.template contains <Actual>());
            // This is a different pointer type than returned by the other
            // operator() if Actual is a reference type.
            typedef typename ::detail::storage::store_as <Actual, Variant>
                ::type * pointer_type;
            typedef typename ::detail::storage::get_as <Actual, Variant
                >::type result_type;
            return static_cast <result_type> (
                * static_cast <pointer_type> (variant.memory()));
        }

        template <class Variant>
            typename ::detail::storage::get_pointer <Actual, Variant>::type
            operator() (Variant * variant) const
        {
            assert (variant->template contains <Actual>());
            typedef typename ::detail::storage::store_as <Actual, Variant>
                ::type * pointer_type;
            return static_cast <pointer_type> (variant->memory());
        }
    };

    // Specialise for void.
    template <> struct get <void> {
        template <class Variant> void operator() (Variant && variant) const
        { assert (variant.template contains <void>()); }

        template <class Variant> void * operator() (Variant * variant) const {
            assert (variant->template contains <void>());
            return nullptr;
        }
    };

} // namespace variant_detail

template <typename Actual, typename Variant>
    inline typename boost::enable_if <
        boost::mpl::and_ <
            is_variant <Variant>,
            meta::contains <Actual, typename variant_types <Variant>::type>>,
        typename::detail::storage::get_as <Actual, Variant>::type
    >::type
    get_unsafe (Variant && variant)
{
    variant_detail::get <Actual> implementation;
    return implementation (std::forward <Variant> (variant));
}

template <typename Actual, typename Variant>
    inline typename boost::enable_if <
        boost::mpl::and_ <
            is_variant <Variant>,
            meta::contains <Actual, typename variant_types <Variant>::type>>,
        typename::detail::storage::get_as <Actual, Variant>::type
    >::type
    get (Variant && variant)
{
    if (!variant.template contains <Actual>())
        throw bad_get();
    return get_unsafe <Actual, Variant> (std::forward <Variant> (variant));
}

template <typename Actual, typename Variant>
    inline typename boost::enable_if <
        boost::mpl::and_ <
            is_variant <Variant>,
            meta::contains <Actual, typename variant_types <Variant>::type>>,
        typename ::detail::storage::get_pointer <Actual, Variant>::type
    >::type
    get (Variant * variant)
{
    if (!variant->template contains <Actual>())
        return 0;

    variant_detail::get <Actual> implementation;
    return implementation (variant);
}

/**
Metafunction that returns a type that can represent any of Types.
In the process, it merges the types according to meta::merge_types.
If the types merge into one type, the returned type is just that type.
*/
template <typename Types, typename MergeTwo>
    struct make_variant_over
{
private:
    // E.g. vector <int, variant <int, float>, double>
    typedef typename meta::as_vector <Types>::type input_types;

    // E.g. vector <vector <int>, vector <int, float>, vector <double> >
    typedef typename meta::transform <
            variant_types <boost::mpl::_>, input_types
        >::type type_sequences;

    // E.g. vector <int, int, float, double>
    typedef typename meta::flatten <type_sequences>::type type_sequence;

    // E.g. mpl::set <int, float, double>
    typedef typename rime::detail::merge_types <MergeTwo, type_sequence>::type
        types;

public:
    typedef typename boost::mpl::eval_if <
        boost::mpl::greater <boost::mpl::size <types>, boost::mpl::int_ <1> >,
        variant_detail::make_variant <types>,
        boost::mpl::front <types>
    >::type type;
};


namespace variant_detail
{
    namespace mpl = boost::mpl;

    template <typename Actual, typename NumberedTypes, typename ... Matches>
        struct find_candidates;

    // No matches found
    template <typename Actual, typename NumberedTypes>
        struct find_candidates <Actual, NumberedTypes>
    {
        typedef meta::vector<> type;
    };

    template <typename Actual, typename NumberedTypes,
        typename FirstMatch, typename ... OtherMatches>
    struct find_candidates <
        Actual, NumberedTypes, FirstMatch, OtherMatches ...>
    {
        typedef typename meta::filter <
                FirstMatch,
                NumberedTypes
            >::type numbered_candidates;

        typedef typename mpl::eval_if <
            mpl::greater_equal <mpl::size <numbered_candidates>, mpl::int_ <1>>,
            // One or more candidates found.
            mpl::identity <numbered_candidates>,
            // No matches found; continue trying.
            find_candidates <Actual, NumberedTypes, OtherMatches ...>
        >::type type;
    };

    /**
    MergeTwo class for constants.
    This forwards to Base the value types of the two types.
    If Type1 or Type2 are constants, therefore, this will cause the merge to be
    attempted on their value types.
    However, if the constants are of the same type and value, then Type1 and
    Type2 will be merged as is.
    */
    template <class Base> struct merge_constant {
        template <typename Type1, typename Type2> struct apply
        : mpl::if_ <rime::same_constant <Type1, Type2>,
            typename Base::template apply <Type1, Type2>,
            typename Base::template apply <
                typename rime::value <Type1>::type,
                typename rime::value <Type2>::type>
        >::type {};
    };

    template <typename Types>
        struct make_variant
    : make_variant <typename meta::as_vector <Types>::type> {};

    template <typename ... Types>
        struct make_variant <meta::vector <Types ...> >
    {
        typedef variant <Types ...> type;
    };

}   // namespace variant_detail

struct merge_two
: variant_detail::merge_constant <rime::detail::merge_two::same<> > {};

}   // namespace rime

#endif  // RIME_VARIANT_HPP_INCLUDED

