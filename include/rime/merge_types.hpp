/*
Copyright 2011, 2012, 2014 Rogier van Dalen.

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
Merge duplicate types (according to a stack of criteria) in a type sequence.
*/

#ifndef RIME_MERGE_TYPES_HPP_INCLUDED
#define RIME_MERGE_TYPES_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/placeholders.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"
#include "meta/range.hpp"
#include "meta/fold.hpp"

namespace rime {

/**
This namespace contains a number of metafunction classes that merge two types.
They contain an apply struct with two template parameters, which has type
"type" iff the two types passed in can be merged.
In that case, type "type" is the merger of the two types.

Many of the metafunctions in this namespace recursively call another
metafunction.
This recursion would allow us to surmise that, for example, const int & and
boost::reference_wrapper<int> are the same type. (Not implemented.)
*/
namespace merge_policy {

    // Does not have "type" member type.
    struct unimplemented {
        // Does not have "type" member type.
        template <class Type1, class Type2> struct apply {};
    };

    BOOST_MPL_HAS_XXX_TRAIT_DEF(type)

    template <class MetafunctionClass, class Type1, class Type2>
        struct is_implemented
    : has_type <typename MetafunctionClass::template apply <Type1, Type2> > {};

    template <class MetafunctionClass, class Type1, class Type2>
        struct is_unimplemented
    : boost::mpl::not_ <is_implemented <MetafunctionClass, Type1, Type2> > {};

    template <class Base = unimplemented> struct same {
        template <class Type1, class Type2> struct apply
        : Base::template apply <Type1, Type2> {};

        template <class Type> struct apply <Type, Type>
        { typedef Type type; };
    };

    /**
    Merge two types using MergeTwo, and apply Transforms... to the result.
    */
    template <class MergeTwo, class Type1, class Type2,
        template <class> class ... Transforms>
    struct lazy;

    template <class MergeTwo, class Type1, class Type2,
        template <class> class Transform>
    struct lazy <MergeTwo, Type1, Type2, Transform>
    {
        typedef typename Transform <
                typename MergeTwo::template apply <Type1, Type2>::type
            >::type type;
    };

    template <class MergeTwo, class Type1, class Type2,
        template <class> class Transform1, template <class> class Transform2>
    struct lazy <MergeTwo, Type1, Type2, Transform1, Transform2>
    {
        typedef typename Transform2 <typename Transform1 <
                typename MergeTwo::template apply <Type1, Type2>::type
            >::type>::type type;
    };

    // If MergeTwo::apply <Type1, Type2> is implemented, return
    // Transform <... <MergeTwo::apply <Type1, Type2>::type ...>::type.
    // Otherwise, have no type.
    template <class MergeTwo, class Type1, class Type2,
        template <class> class ... Transforms> struct merge_transform
    : boost::mpl::if_ <is_implemented <MergeTwo, Type1, Type2>,
        lazy <MergeTwo, Type1, Type2, Transforms ...>, unimplemented>::type {};

    /**
    Merge policy that forwards two types to the base policy without
    const-qualification.
    If either of the types was const-qualified, then "const" is added onto the
    type resulting from the base policy.
    */
    template <class Base = same<>> struct const_ {
        template <class Type1, class Type2> struct apply
        : Base::template apply <Type1, Type2> {};

        template <class Type1, class Type2> struct apply <Type1 const, Type2>
        : merge_transform <Base, Type1, Type2, std::add_const> {};

        template <class Type1, class Type2> struct apply <Type1, Type2 const>
        : merge_transform <Base, Type1, Type2, std::add_const> {};

        template <class Type1, class Type2>
            struct apply <Type1 const, Type2 const>
        : merge_transform <Base, Type1, Type2, std::add_const> {};
    };

    /**
    Merge policy that forwards two types to the base policy first with, then
    without reference-qualification.
    */
    template <class Base = const_<> > struct reference {
        // Try to use Base with Type1 and Type2 directly.
        // If that doesn't work, use std::remove_reference on Type1 and Type2
        // and try again.
        template <class Type1, class Type2> struct apply
        : boost::mpl::if_ <
            is_implemented <Base, Type1, Type2>,
            typename Base::template apply <Type1, Type2>,
            typename Base::template apply <
                typename std::remove_reference <Type1>::type,
                typename std::remove_reference <Type2>::type>>::type {};
    };

    /**
    Merge policy that forwards to the base policy, applying std::decay if the
    base policy is not implemented for the two types themselves.
    */
    template <class Base = same<>> struct decay {
        // Try to use Base with Type1 and Type2 directly.
        // If that doesn't work, use std::decay on Type1 and Type2 and try
        // again.
        template <class Type1, class Type2> struct apply
        : boost::mpl::if_ <
            is_implemented <Base, Type1, Type2>,
            typename Base::template apply <Type1, Type2>,
            typename Base::template apply <
                typename std::decay <Type1>::type,
                typename std::decay <Type2>::type>>::type {};
    };

    struct base_class_implementation {
        template <class Type1, class Type2, class Enable1 = void>
            struct apply {};

        template <class Type1, class Type2>
            struct apply <Type1, Type2, typename boost::enable_if <
                std::is_base_of <Type1, Type2>>::type>
        { typedef Type1 type; };

        template <class Type1, class Type2>
            struct apply <Type1, Type2, typename boost::enable_if <
                std::is_base_of <Type2, Type1>>::type>
        { typedef Type2 type; };
    };

    /**
    Merge policy that finds the base class out of the two classes if one
    is derived from the other.
    If the types are the same, then that type is returned.
    This is unimplemented for classes that are derived from a common base.
    */
    struct base_class : same <base_class_implementation> {};

    /**
    Merge policy that collapses types, but does not convert anything.
    This means: merging qualifiers to the most conservative one, and references
    and pointers with a base class and a derived class become
    similarly-qualified base classes.
    */
    struct collapse {
        template <class Type1, class Type2, class Enable = void> struct apply
        : const_ <same<>>::template apply <Type1, Type2> {};

        // Pointers.
        template <class Type1, class Type2>
            struct apply <Type1 *, Type2 *>
        : merge_transform <const_ <base_class>, Type1, Type2, std::add_pointer>
        {};

        // One non-reference: convert to a non-reference.
        template <class Type1, class Type2> struct apply <Type1, Type2 &>
        : same<>::template apply <Type1, Type2> {};
        template <class Type1, class Type2> struct apply <Type1, Type2 const &>
        : same<>::template apply <Type1, Type2> {};
        template <class Type1, class Type2> struct apply <Type1, Type2 &&>
        : same<>::template apply <Type1, Type2> {};

        template <class Type1, class Type2>
            struct apply <Type1 const, Type2 &>
        : merge_transform <same<>, Type1, Type2, std::add_const> {};
        template <class Type1, class Type2>
            struct apply <Type1 const, Type2 const &>
        : merge_transform <same<>, Type1, Type2, std::add_const> {};
        template <class Type1, class Type2>
            struct apply <Type1 const, Type2 &&>
        : merge_transform <same<>, Type1, Type2, std::add_const> {};

        template <class Type1, class Type2>
            struct apply <Type1 &, Type2>
        : same<>::template apply <Type1, Type2> {};
        template <class Type1, class Type2>
            struct apply <Type1 const &, Type2>
        : same<>::template apply <Type1, Type2> {};
        template <class Type1, class Type2>
            struct apply <Type1 &&, Type2>
        : same<>::template apply <Type1, Type2> {};

        template <class Type1, class Type2>
            struct apply <Type1 &, Type2 const>
        : merge_transform <same<>, Type1, Type2, std::add_const> {};
        template <class Type1, class Type2>
            struct apply <Type1 const &, Type2 const>
        : merge_transform <same<>, Type1, Type2, std::add_const> {};
        template <class Type1, class Type2>
            struct apply <Type1 &&, Type2 const>
        : merge_transform <same<>, Type1, Type2, std::add_const> {};

        // All combinations of references: convert to const &
        template <class Type1, class Type2> struct apply <Type1 &, Type2 &>
        : merge_transform <base_class, Type1, Type2, std::add_lvalue_reference>
        {};
        template <class Type1, class Type2>
            struct apply <Type1 &, Type2 const &>
        : merge_transform <base_class, Type1, Type2,
            std::add_const, std::add_lvalue_reference> {};
        template <class Type1, class Type2> struct apply <Type1 &, Type2 &&>
        : merge_transform <base_class, Type1, Type2,
            std::add_const, std::add_lvalue_reference> {};

        template <class Type1, class Type2>
            struct apply <Type1 const &, Type2 &>
        : merge_transform <base_class, Type1, Type2,
            std::add_const, std::add_lvalue_reference> {};
        template <class Type1, class Type2>
            struct apply <Type1 const &, Type2 const &>
        : merge_transform <base_class, Type1, Type2,
            std::add_const, std::add_lvalue_reference> {};
        template <class Type1, class Type2>
            struct apply <Type1 const &, Type2 &&>
        : merge_transform <base_class, Type1, Type2,
            std::add_const, std::add_lvalue_reference> {};

        template <class Type1, class Type2> struct apply <Type1 &&, Type2 &>
        : merge_transform <base_class, Type1, Type2,
            std::add_const, std::add_lvalue_reference> {};
        template <class Type1, class Type2>
            struct apply <Type1 &&, Type2 const &>
        : merge_transform <base_class, Type1, Type2,
            std::add_const, std::add_lvalue_reference> {};
        template <class Type1, class Type2> struct apply <Type1 &&, Type2 &&>
        : merge_transform <base_class, Type1, Type2, std::add_rvalue_reference>
        {};
    };

    /**
    Merge policy that uses std::common_type to merge two types.
    */
    struct common_type {
        template <class Type1, class Type2> struct apply
        : std::common_type <Type1, Type2> {};
    };

} // namespace merge_policy

namespace merge_detail {

    /*
    In pseudo-code:
    insert (MergeTwo, New, (First, Rest...)):
        if MergeTwo (New, First):
            merged_new = MergeTwo (New, First)
            return insert (MergeTwo, merged_new, (Rest...))
        else:
            (merged_new, merged_rest) = insert (MergeTwo, merged_new, (Rest...))
            if MergeTwo (merged_new, First):
                merged_new_2 = MergeTwo (merged_new, First)
                return insert (MergeTwo, merged_new_2, merged_rest)
            else:
                return (merged_new, (First, merged_rest ...))

    Note that the if .. else occurs twice: merge_first_else.
    */

    template <typename MergeTwo, typename New, typename Current>
        struct insert_impl;

    // Trivial case
    template <typename MergeTwo, typename New>
        struct insert_impl <MergeTwo, New, meta::vector<> >
    { typedef boost::mpl::pair <New, meta::vector<>> type; };

    template <typename MergeTwo, typename New, typename First, typename Rest>
        struct do_merge_and_recurse
    : insert_impl <MergeTwo,
        // New =
        typename MergeTwo::template apply <New, First>::type,
        // Current =
        Rest> {};


    template <typename MergeTwo, typename New, typename Current, typename Else>
        struct merge_first_else;
    template <typename MergeTwo, typename New,
            typename First, typename ... Rest, typename Else>
        struct merge_first_else <
            MergeTwo, New, meta::vector <First, Rest...>, Else>
    : boost::mpl::eval_if <
        merge_policy::is_implemented <MergeTwo, New, First>,
        do_merge_and_recurse <MergeTwo, New, First, meta::vector <Rest...> >,
        Else>
    {};

    template <typename MergeTwo, typename New, typename Current>
        struct insert_skip_first;
    template <typename MergeTwo, typename New, typename First,
            typename ... Rest>
    struct insert_skip_first <MergeTwo, New, meta::vector <First, Rest ...>>
    {
        typedef insert_impl <MergeTwo, New, meta::vector <Rest...> >
            merge_rest;
        typedef typename merge_rest::type::first merged_new;
        typedef typename merge_rest::type::second merged_rest;

        typedef typename merge_first_else <
                MergeTwo, merged_new,
                    typename meta::push <First, merged_rest>::type,
                    boost::mpl::pair <merged_new,
                        typename meta::push <First, merged_rest>::type>
            >::type type;
    };

    template <typename MergeTwo, typename New, typename First,
            typename ... Rest>
    struct insert_impl <MergeTwo, New, meta::vector <First, Rest ...>>
    : merge_first_else <MergeTwo, New, meta::vector <First, Rest ...>,
        // Else =
        insert_skip_first <MergeTwo, New, meta::vector <First, Rest ...> > > {};

    template <typename MergeTwo, typename New, typename Types> struct insert {
        // boost::mpl::pair <LastType, meta::vector <Rest ...> >
        typedef typename insert_impl <MergeTwo, New, Types>::type pair;
        typedef typename meta::push <
            typename pair::first, typename pair::second>::type type;
    };

} // namespace merge_detail

/**
Return a compile-time list of types, where all types that can be merged
by metafunction class MergeTwo are merged.
*/
template <typename MergeTwo, typename Types>
    struct merge_types
: meta::fold <meta::back,
    merge_detail::insert <MergeTwo, boost::mpl::_2, boost::mpl::_1>,
    meta::vector<>, Types> {};

} // namespace rime

#endif // RIME_MERGE_TYPES_HPP_INCLUDED
