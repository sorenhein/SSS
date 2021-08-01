#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "RangeComplete.h"
#include "Result.h"


void RangeComplete::init(const Result& result)
{
  distribution = result.dist;
  lower = result.tricks;
  upper = result.tricks;
  minimum = result.tricks;

  winnersHigh = result.winners;
  winnersLow = result.winners;
}


void RangeComplete::extend(const Result& result)
{
  assert(distribution == result.dist);
  if (result.tricks < lower)
  {
    lower = result.tricks;
    minimum = result.tricks;
    winnersLow = result.winners;
  }
  else if (result.tricks == lower)
    winnersLow *= result.winners;

  if (result.tricks > upper)
  {
    upper = result.tricks;
    winnersHigh = result.winners;
  }
  else if (result.tricks == upper)
    winnersHigh |= result.winners;
}


void RangeComplete::operator *= (const RangeComplete& range2)
{
  // We compare ranges in the first place according to upper.
  // So we define an ordering relationship on ranges where upper
  // is the first criterion.
  // In case the two have the same lower and upper, it would be 
  // possible to check whether winnersHigh and winnersLow are
  // lower (i.e. one of them < and the other <=).
  // This would no longer be a complete ordering, but there might
  // be a few cases where we get better reductions out of it.

  if (range2.minimum < minimum)
    minimum = range2.minimum;

  if (range2.upper < upper ||
      (range2.upper == upper && range2.lower < lower))
  {
    lower = range2.lower;
    upper = range2.upper;
    winnersHigh = range2.winnersHigh;
    winnersLow = range2.winnersLow;
  }
}


bool RangeComplete::operator < (const RangeComplete& range2) const
{
  if (upper != range2.lower)
    return (upper < range2.lower);

  // The Winners method is from declarer's perspective, so c is
  // WIN_FIRST if declarer prefers our own winnersHigh.  
  // In this method we are taking the defenders' perspective.
  const WinnerCompare c = 
    winnersHigh.compareForDeclarer(range2.winnersLow);

  if (c == WIN_SECOND)
    return true;
  else if (c != WIN_EQUAL)
    return false;
  else
    return (range2.lower < range2.upper || lower < upper);
}


bool RangeComplete::constant() const
{
  return (lower == minimum && 
    upper == minimum &&
    winnersHigh == winnersLow);
}


const Winners& RangeComplete::constantWinners() const
{
  return winnersLow;  // Either one, as range assumed constant
}


string RangeComplete::strHeader() const
{
  stringstream ss;
  ss << 
    setw(4) << right << "dist" <<
    setw(4) << "lo" <<
    setw(4) << "hi" << 
    setw(4) << "min" << 
    setw(6) << "Wlo" << 
    setw(6) << "Whi" << 
    endl;
  return ss.str();
}


unsigned char RangeComplete::dist() const
{
  return distribution;
}


unsigned char RangeComplete::min() const
{
  return minimum;
}


string RangeComplete::str() const
{
  stringstream ss;
  ss << 
    setw(4) << +distribution <<
    setw(4) << +lower <<
    setw(4) << +upper << 
    setw(4) << +minimum << 
    setw(6) << winnersLow.strEntry() << 
    setw(6) << winnersHigh.strEntry() << 
    endl;
  return ss.str();
}
