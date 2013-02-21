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
Merge duplicate types (according to a stack of criteria) in a type sequence.
*/

#ifndef RIME_DETAIL_MERGE_TYPES_HPP_INCLUDED
#define RIME_DETAIL_MERGE_TYPES_HPP_INCLUDED

#include <type_traits>

#include <boost/mpl/assert.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/placeholders.hpp>

#include <boost/utility/enable_if.hpp>

#include "meta/vector.hpp"
#include "meta/range.hpp"
#include "meta/fold.hpp"

namespace rime
{
namespace detail
{

namespace mpl = ::boost::mpl;

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
namespace merge_two
{
    // Does not have "type" member type.
    struct unimplemented
    {
        // Does not have "type" member type.
        template <class Type1, class Type2> struct apply {};
    };

    BOOST_MPL_HAS_XXX_TRAIT_DEF(type)

    template <class MetafunctionClass, class Type1, class Type2>
        struct is_implemented
    : has_type <typename MetafunctionClass::template apply <Type1, Type2> > {};

    template <class MetafunctionClass, class Type1, class Type2>
        struct is_unimplemented
    : mpl::not_ <is_implemented <MetafunctionClass, Type1, Type2> > {};

    template <class Base = unimplemented>
        struct same
    {
        template <class Type1, class Type2> struct apply
        : Base::template apply <Type1, Type2> {};

        template <class Type> struct apply <Type, Type>
        {
            typedef Type type;
        };
    };

    template <class MergeTwo, class Type1, class Type2,
        template <class T> class Transform>
    struct lazy
    {
        typedef typename Transform <
                typename MergeTwo::template apply <Type1, Type2>::type
            >::type type;
    };

    // If MergeTwo::apply <Type1, Type2> is implemented, return
    // Transform <MergeTwo::apply <Type1, Type2>::type>::type.
    // Otherwise, have no type.
    template <class MergeTwo, class Type1, class Type2,
        template <class T> class Transform>
    struct merge_transform
    : mpl::if_ <is_implemented <MergeTwo, Type1, Type2>,
        lazy <MergeTwo, Type1, Type2, Transform>, unimplemented>::type {};

    template <class Base = same<> >
        struct const_
    {
        template <class Type1, class Type2>
            struct apply
        : Base::template apply <Type1, Type2> {};

        template <class Type1, class Type2>
            struct apply <Type1 const, Type2>
        : merge_transform <Base, Type1, Type2, std::add_const> {};

        template <class Type1, class Type2>
            struct apply <Type1, Type2 const>
        : merge_transform <Base, Type1, Type2, std::add_const> {};

        template <class Type1, class Type2>
            struct apply <Type1 const, Type2 const>
        : merge_transform <Base, Type1, Type2, std::add_const> {};
    };

    template <class Base = const_<> >
        struct reference
    {
        template <class Type1, class Type2>
            struct apply
        : Base::template apply <Type1, Type2> {};

        template <class Type1, class Type2>
            struct apply <Type1 &, Type2>
        : apply <Type1, Type2> {};

        template <class Type1, class Type2>
            struct apply <Type1, Type2 &>
        : apply <Type1, Type2> {};

        template <class Type1, class Type2>
            struct apply <Type1 &, Type2 &>
        : apply <Type1, Type2> {};

        template <class Type>
            struct apply <Type &, Type &>
        {
            typedef Type & type;
        };
    };

}  // namespace merge_two

namespace merge
{
    /**
    Try to merge New with any of the types in Types, a meta::vector.
    \return a mpl::pair of a new type and a meta::vector of other types.
        The types in the meta::vector are a subset of the types in Types.
    */
    /*template <typename MergeTwo, typename New, typename Types>
        struct insert_impl;

    // Trivial case
    template <typename MergeTwo, typename New>
        struct insert_impl <MergeTwo, New, meta::vector<> >
    {
        typedef New new_;
        typedef meta::vector<> rest;
    };*/

    /**
    Merge New and First.
    Then check whether the merger merges with anything in Rest.
    */
    /*template <typename MergeTwo, typename New, typename First, typename Rest>
        struct insert_merge
    : insert_impl <MergeTwo,
        typename MergeTwo::template apply <New, First>::type, Rest> {};*/

    /**
    If New fuses with First, try to insert it in Rest.
    Otherwise, just return New and vector <First, Rest ...>.
    */
    /*template <typename MergeTwo, typename New, typename First, typename Rest>
        struct insert_first
    : boost::mpl::if_ <
        merge_two::is_implemented <MergeTwo, New, First>,*/


    /**
    Recursively call insert.
    */
    /*template <typename MergeTwo, typename New, typename First, typename Rest>
        struct insert_dont_merge
    {
        typedef insert_impl <MergeTwo, New, Rest> merge_rest;
        typedef typename merge_rest::new_ merged_new;
        typedef typename merge_rest::rest merged_rest;

        typedef typename mpl::eval_if <
            merge_two::is_implemented <MergeTwo, new_new, First>
            insert_merge <MergeTwo, new_new, First, new_rest>,
            identity <mpl::pair <new_new, meta::vector <First, new_rest ...> > >
        >::type type;
    };

    template <typename MergeTwo, typename New,
            typename First, typename ... Rest>
        struct insert_impl <MergeTwo, New, meta::vector <First, Rest...> >
    : boost::mpl::eval_if <
        merge_two::is_implemented <MergeTwo, New, First>,
        insert_merge <MergeTwo, New, First, Rest...>,
        insert_dont_merge <MergeTwo, New, First, Rest ...>
    > {};

    template <typename MergeTwo, typename New, typename Types>
        struct insert
    {
        // mpl::pair <LastType, meta::vector <Rest ...> >
        typedef typename insert_impl <MergeTwo, New, Types>::type pair;
        typedef typename meta::push <
            typename pair::first, typename pair::second>::type type;
    };*/








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
    {
        typedef mpl::pair <New, meta::vector<> > type;
    };

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
        struct merge_first_else <MergeTwo, New, meta::vector <First, Rest...>, Else>
    : mpl::eval_if <
        merge_two::is_implemented <MergeTwo, New, First>,
        do_merge_and_recurse <MergeTwo, New, First, meta::vector <Rest...> >,
        Else>
    {};

    template <typename MergeTwo, typename New, typename Current>
        struct insert_skip_first;
    template <typename MergeTwo, typename New, typename First, typename ... Rest>
        struct insert_skip_first <MergeTwo, New, meta::vector <First, Rest ...> >
    {
        typedef insert_impl <MergeTwo, New, meta::vector <Rest...> >
            merge_rest;
        typedef typename merge_rest::type::first merged_new;
        typedef typename merge_rest::type::second merged_rest;

        typedef typename merge_first_else <
                MergeTwo, merged_new,
                    typename meta::push <First, merged_rest>::type,
                    mpl::pair <merged_new,
                        typename meta::push <First, merged_rest>::type>
            >::type type;
    };

    template <typename MergeTwo, typename New, typename First, typename ... Rest>
        struct insert_impl <MergeTwo, New, meta::vector <First, Rest ...> >
    : merge_first_else <MergeTwo, New, meta::vector <First, Rest ...>,
        // Else =
        insert_skip_first <MergeTwo, New, meta::vector <First, Rest ...> > > {};

    template <typename MergeTwo, typename New, typename Types>
        struct insert
    {
        // mpl::pair <LastType, meta::vector <Rest ...> >
        typedef typename insert_impl <MergeTwo, New, Types>::type pair;
        typedef typename meta::push <
            typename pair::first, typename pair::second>::type type;
    };
}   // namespace merge

/**
Return a compile-time list of types, where all types that can be merged
by metafunction class MergeTwo are merged.
*/
template <typename MergeTwo, typename Types>
    struct merge_types
: meta::fold <meta::back, merge::insert <MergeTwo, mpl::_2, mpl::_1>,
    meta::vector<>, Types> {};

}}   // namespace rime::detail

#endif // RIME_DETAIL_MERGE_TYPES_HPP_INCLUDED

