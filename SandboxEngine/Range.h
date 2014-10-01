#pragma once
#include <iterator>

// for a full implementation, see:
// https://bitbucket.org/AraK/range/src/13012260410794b9f5320b4bdbb39b41cfec05f7/range.hpp?at=default

/**
 * \brief Templated base class for range based iterators.
 *
 * The range class is designed to allow developers to iterate over a range easily, especially when used in
 * conjunction with C++11's new for loop. The concept was inspired from a StackOverflow discussion linked below, but
 * has seen multiple improvements since.
 *
 * Range is designed to be as similiar and compatible as possible with the STL. It is built from STL concepts and
 * classes, and should work smoothly in STL algorithms.
 *
 * The Range class satisfies the following requirements:
 *  - Immutable
 *  - Forward incrementing
 *  - Constant time complexity
 *
 * The original inspiration was from Stack Overflow: http ://stackoverflow.com/a/7185723
 * Improvements to the proposed class include:
 *  - Included both compile time constant (template version) and runtime (non template version) range functionality
 *    with two classes sharing a common templated base.
 *  - Change the range and iterator classes to use a templated value type to facilitate future type swapping.
 *  - Made the iterator inherit from a STL forward iterator base.
 *  - Added a const_iterator which is aliased to iterator (since iterator is constant anyway).
 *  - Added cbegin() and cend() functions to the ranges.
 */
template<typename IntegerType>
class Range
{
public:
    class iterator : public std::iterator<std::forward_iterator_tag, IntegerType>
    {
    public:
        explicit iterator(IntegerType start)
            : mI(start)
        {
        }

        IntegerType operator *() const { return mI; }
        iterator& operator ++() { ++mI; return *this; }
        iterator operator ++(int) { iterator copy(*this); ++mI; return copy; }

        bool operator == (const iterator& other) const { return mI == other.mI; }
        bool operator != (const iterator& other) const { return mI != other.mI; }

    private:
        IntegerType mI;
    };

    using const_iterator = iterator;
};

/**
 * Templated implementation of a range concept, useful for for loops. Begin and end values are compile time constants,
 * which may provide speed benefits over the runtime range version.
 */
template<typename IntegerType, IntegerType BeginIndex, IntegerType EndIndex>
class ConstantRange : public Range<IntegerType>
{
public:
    iterator begin() const { return iterator(BeginIndex); }
    iterator end() const { return iterator(EndIndex); }
    const_iterator cbegin() const { return iterator(BeginIndex); }
    const_iterator cend() const { return iterator(EndIndex); }
};

/**
 * A non compile time constant version of the range concept, useful for loops.
 */
template<typename IntegerType>
class RuntimeRange : public Range<IntegerType>
{
public:
    RuntimeRange(IntegerType begin, IntegerType end)
        : mBeginIndex(begin),
          mEndIndex(end)
    {
    }

    iterator begin() const { return mBeginIndex; }
    iterator end() const { return mEndIndex; }
    const_iterator cbegin() const { return mBeginIndex; }
    const_iterator cend() const { return mEndIndex; }

private:
    const_iterator mBeginIndex;
    const_iterator mEndIndex;
};


/**
 * \brief Return a range object for [begin, end).
 * The returned range object has begin and end values as compiled time constants. This may or may not speed up
 * performance of the loop.
*/
template<unsigned int BeginIndex, unsigned int EndIndex>
const ConstantRange<unsigned int, BeginIndex, EndIndex> MakeRange()
{
    return ConstantRange<unsigned int, BeginIndex, EndIndex>();
}

/**
 * \brief Return a runtime range object for [begin, end).
 */
const RuntimeRange<unsigned int> MakeRange(unsigned int begin, unsigned int end);