#pragma once
// http://stackoverflow.com/a/7185723

class Range
{
public:
    class iterator
    {
        friend class Range;

    public:
        long int operator *() const { return mI; }
        const iterator& operator ++() { ++mI; return *this; }
        iterator operator ++(int) { iterator copy(*this); ++mI; return copy; }

        bool operator ==(const iterator& other) const { return mI == other.mI; }
        bool operator !=(const iterator& other) const { return mI != other.mI; }

    protected:
        iterator(long int start)
            : mI(start)
        {
        }

    private:
        unsigned long mI;
    };

    Range(long int begin, long int end)
        : mBegin(begin),
          mEnd(end)
    {
    }

    iterator begin() const { return mBegin; }
    iterator end() const { return mEnd; }

private:
    iterator mBegin;
    iterator mEnd;
};

