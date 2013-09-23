/*
Copyright 2013 Rogier van Dalen.

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
\file
Define an object to check the number of constructions and assignments.
This is useful to test the behaviour of containers when rvalues are involved.
This file must be included only after the Boost.Test predicates have been
included.
*/

#ifndef RIME_UTILITY_COUNTED_HPP_INCLUDED
#define RIME_UTILITY_COUNTED_HPP_INCLUDED

#include <utility>

#include <boost/utility/enable_if.hpp>

namespace utility {

template <class Contained> class counted;

class object_counter {
    int value_construct_count_;
    int copy_count_;
    int move_count_;
    int copy_assign_count_;
    int move_assign_count_;
    int swap_count_;
    int destruct_count_;
    int destruct_moved_count_;

    friend class counted <void>;
public:
    object_counter()
    : value_construct_count_(), copy_count_(), move_count_(),
        copy_assign_count_(), move_assign_count_(), swap_count_(),
        destruct_count_(), destruct_moved_count_() {}

    bool consistent() const {
        // All constructed objects must have been destructed.
        if (value_construct_count_ + copy_count_ + move_count_ !=
            destruct_count_ + destruct_moved_count_)
            return false;
        // All carcasses must have been destructed.
        if (move_count_ + move_assign_count_ != destruct_moved_count_)
            return false;
        return true;
    }

    ~object_counter() {
        // All the objects that reference this should have been destructed.
        // This is both a useful check for program sanity, but also necessary
        // because otherwise objects will be accessing this past its lifetime.
        assert (this->consistent());
    }

    int value_construct_count() const { return value_construct_count_; }
    int copy_count() const { return copy_count_; }
    int move_count() const { return move_count_; }
    int copy_assign_count() const { return copy_assign_count_; }
    int move_assign_count() const { return move_assign_count_; }
    int swap_count() const { return swap_count_; }
    int destruct_count() const { return destruct_count_; }
    int destruct_moved_count() const { return destruct_moved_count_; }

    void expect (int expected_value_construct_count,
        int expected_copy_count, int expected_move_count,
        int expected_copy_assign_count, int expected_move_assign_count,
        int expected_swap_count,
        int expected_destruct_count, int expected_destruct_moved_count) const
    {
        BOOST_CHECK_EQUAL (value_construct_count(),
            expected_value_construct_count);
        BOOST_CHECK_EQUAL (copy_count(), expected_copy_count);
        BOOST_CHECK_EQUAL (move_count(), expected_move_count);
        BOOST_CHECK_EQUAL (copy_assign_count(), expected_copy_assign_count);
        BOOST_CHECK_EQUAL (move_assign_count(), expected_move_assign_count);
        BOOST_CHECK_EQUAL (swap_count(), expected_swap_count);
        BOOST_CHECK_EQUAL (destruct_count(), expected_destruct_count);
        BOOST_CHECK_EQUAL (destruct_moved_count(),
            expected_destruct_moved_count);
    }
};

template<> class counted <void> {
    object_counter & counter;
    int validity_;

    // These values are actually from random.org, so even though they might not
    // look random, they are.
    static const int valid = 0xCBFCDDFB;
    static const int moved = 0x00633222;
    static const int invalid = 0x45AE60C4;

    bool is_valid() const { return validity_ == valid; }
    bool is_moved() const { return validity_ == moved; }
    bool is_invalid() const { return validity_ == invalid; }

    void make_moved() { validity_ = moved; }
    void invalidate() { validity_ = invalid; }
public:
    counted (object_counter & counter) : counter (counter),
        validity_ (valid)
    { ++ counter.value_construct_count_; }

    counted (counted const & other)
    : counter (other.counter), validity_ (valid)
    {
        assert (other.is_valid());
        ++ counter.copy_count_;
    }

    counted (counted && other)
    : counter (other.counter), validity_ (valid)
    {
        assert (other.is_valid());
        other.make_moved();
        ++ counter.move_count_;
    }

    ~counted() {
        if (is_valid())
            ++ counter.destruct_count_;
        else {
            assert (is_moved());
            ++ counter.destruct_moved_count_;
        }
        invalidate();
    }

    counted & operator = (counted const & other) {
        assert (&counter ==  &other.counter);
        assert (this->is_valid());
        assert (other.is_valid());
        ++ counter.copy_assign_count_;
        return *this;
    }

    counted & operator = (counted && other) {
        assert (&counter ==  &other.counter);
        assert (this->is_valid());
        assert (other.is_valid());
        other.make_moved();
        ++ counter.move_assign_count_;
        return *this;
    }

    void swap (counted & other) {
        assert (&counter ==  &other.counter);
        assert (this->is_valid());
        assert (other.is_valid());
        ++ counter.swap_count_;
    }
};

template <class Value> class counted
: public counted <void>
{
    Value value_;
public:
    template <class Argument>
    counted (object_counter & counter, Argument && argument)
    : counted <void> (counter),
        value_ (std::forward <Argument> (argument)) {}

    template <class OtherValue>
    counted (counted <OtherValue> const & other, typename
        boost::enable_if <std::is_convertible <OtherValue const &, Value>>
            ::type * = 0)
    : counted <void> (other), value_ (other.value()) {}

    template <class OtherValue>
    counted (counted <OtherValue> && other, typename
        boost::enable_if <std::is_convertible <OtherValue, Value>>
            ::type * = 0)
    : counted <void> (std::move (other)),
        value_ (std::move (other.value())) {}

    /* Default-provided constructors and assignment operators are fine. */

    void swap (counted & other) {
        counted <void>::swap (other);
        using std::swap;
        swap (this->value_, other.value_);
    }

    Value const & value() const { return value_; }
    Value & value() { return value_; }
};

template <class Value>
inline void swap (counted <Value> & c1, counted <Value> & c2)
{ c1.swap (c2); }

} // namespace utility

#endif // RIME_UTILITY_COUNTED_HPP_INCLUDED

