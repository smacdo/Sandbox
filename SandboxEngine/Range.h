#pragma once
// http://stackoverflow.com/a/7185723
// TODO: Consider converting this into int rather than unsigned int.

/**
 * \brief Templated base class for range based iterators.
 */
template<typename T>
class Range
{
public:
    class const_iterator
    {
        friend class Range;

    public:
        explicit const_iterator(T start)
            : mI(start)
        {
        }

        T operator *() const { return mI; }
        const const_iterator& operator ++() { ++mI; return *this; }
        const_iterator operator ++(int) { iteconst_iteratorrator copy(*this); ++mI; return copy; }

        bool operator == (const const_iterator& other) const { return mI == other.mI; }
        bool operator != (const const_iterator& other) const { return mI != other.mI; }

    private:
        T mI;
    };
};

/**
 * \brief Templated implementation of a range concept, useful for for loops. Begin and end values are compile time
 *        constants, which may provide speed benefits over the non templated Range.
 */
template<typename T, T BeginIndex, T EndIndex>
class ConstantRange : public Range<T>
{
public:
    const_iterator begin() const { return const_iterator(BeginIndex); }
    const_iterator end() const { return const_iterator(EndIndex); }
};

/**
* \brief Return a compile time constant range object for [begin, end).
*/
template<unsigned int TBegin, unsigned int TEnd>
const ConstantRange<unsigned int, TBegin, TEnd> MakeRange()
{
    return ConstantRange<unsigned int, TBegin, TEnd>();
}

/**
 * A non-templated implementation of the range concept.
 */
template<typename T>
class RuntimeRange
{
public:
    class const_iterator
    {
        friend class RuntimeRange;

    public:
        T operator *() const { return mI; }
        const const_iterator& operator ++() { ++mI; return *this; }
        const_iterator operator ++(int) { const_iterator copy(*this); ++mI; return copy; }

        bool operator ==(const const_iterator& other) const { return mI == other.mI; }
        bool operator !=(const const_iterator& other) const { return mI != other.mI; }

    protected:
        explicit const_iterator(T start)
            : mI(start)
        {
        }

    private:
        T mI;
    };

    RuntimeRange(T begin, T end)
        : mBeginIndex(begin),
          mEndIndex(end)
    {
    }

    const_iterator begin() const { return mBeginIndex; }
    const_iterator end() const { return mEndIndex; }

private:
    const_iterator mBeginIndex;
    const_iterator mEndIndex;
};

/**
 * \brief Return a runtime range object for [begin, end).
 */
const RuntimeRange<unsigned int> MakeRange(unsigned int begin, unsigned int end);