#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Range.h"
#include "Result.h"


void Range::init(const Result& result)
{
  distribution = result.dist();
  lower = result.tricks();
  upper = result.tricks();
  minimum = result.tricks();
}


void Range::extend(const Result& result)
{
  assert(distribution == result.dist());
  if (result.tricks() < lower)
  {
    lower = result.tricks();
    minimum = result.tricks();
  }
  if (result.tricks() > upper)
    upper = result.tricks();
}


void Range::operator *= (const Range& range2)
{
  if (range2.minimum < minimum)
    minimum = range2.minimum;

  if (range2.upper < upper ||
      (range2.upper == upper && range2.lower < lower))
  {
    lower = range2.lower;
    upper = range2.upper;
  }
}


bool Range::operator < (const Range& range2) const
{
  return (upper < range2.lower ||
    (upper == range2.lower &&
      (range2.lower < range2.upper || lower < upper)));
}


bool Range::constant() const
{
  return (lower == minimum && upper == minimum);
}


string Range::strHeader() const
{
  stringstream ss;
  ss << 
    setw(4) << right << "dist" <<
    setw(4) << "lo" <<
    setw(4) << "hi" << 
    setw(4) << "min" << 
    endl;
  return ss.str();
}


unsigned char Range::dist() const
{
  return distribution;
}


unsigned char Range::min() const
{
  return minimum;
}


string Range::str() const
{
  stringstream ss;
  ss << 
    setw(4) << +distribution <<
    setw(4) << +lower <<
    setw(4) << +upper << 
    setw(4) << +minimum << 
    endl;
  return ss.str();
}
