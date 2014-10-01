#include "stdafx.h"
#include "Range.h"

const RuntimeRange<unsigned int> MakeRange(unsigned int begin, unsigned int end)
{
    return RuntimeRange<unsigned int>(begin, end);
}