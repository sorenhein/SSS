/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Range.h"
#include "Result.h"


void Range::init(const Result& result)
{
  minimum = result;
  resultLow = result;
  resultHigh = result;
}


void Range::extend(const Result& result)
{
  // The range is in general a container for the actual Result's
  // which is as tight as we can make it, but still rounded
  // "outward" when we need to.  Any Result is within its range.

  assert(minimum.dist() == result.getDist());

  minimum *= result;
  resultLow *= result;
  resultHigh += result;
}


void Range::operator *= (const Range& range2)
{
  // To "multiply" two ranges is effectively to take the lowest
  // range (completely), accordingly to some ordering metric.
  // This "lowest" range is used for two purposes:
  //
  // 1. If the range dominates (is below) another within a
  //    set of Strategies (Nodes) with the same parent, then
  //    the dominated range can be removed.  This is only an
  //    optimization, so if we miss a domination, it's not the
  //    end of the world.
  //
  // 2. If the range is truly constant (and minimal), it
  //    dominates all Strategies for that parent.  Our detection
  //    of constancy must be perfect and must dominate all other
  //    Result's. They may not be ranked differently than
  //    the "constant", e.g. 2,5N vs 2,5S.  But we also can't
  //    keep several incommensurate winners, and we also can't
  //    combine them with only *= or |=.
  //    The solution is to widen the range in this specific
  //    case, so 2,5N and 2,5S becomes 2,5NS on the low end
  //    (declarer must use both -- best for the defense, so
  //    in a way "low"), and 2,5N/5S on the high end (declarer
  //    may use either, so in a way "high").  This both widens
  //    the range and keeps it from appearing constant.

  // We compare ranges in the first place according to upper,
  // then lower, then the winners.

  minimum *= range2.minimum;

  const Compare cHigh = resultHigh.compareComplete(range2.resultHigh);

  if (cHigh == WIN_FIRST)
  {
    // Declarer prefers * this, so the defense doesn't.
    resultLow = range2.resultLow;
    resultHigh = range2.resultHigh;
    return;
  }
  else if (cHigh == WIN_SECOND)
  {
    return;
  }
  else if (cHigh == WIN_DIFFERENT)
  {
    resultLow *= range2.resultLow;
    resultHigh += range2.resultHigh;
    return;
  }

  const Compare cLow = resultLow.compareComplete(range2.resultLow);

  if (cLow == WIN_FIRST)
  {
    resultLow = range2.resultLow;
    resultHigh = range2.resultHigh;
    return;
  }
  else if (cLow == WIN_SECOND || cLow == WIN_EQUAL)
  {
    return;
  }
  else // WIN_DIFFERENT
  {
    resultLow *= range2.resultLow;
    resultHigh += range2.resultHigh;
    return;
  }
}


bool Range::operator < (const Range& range2) const
{
  const Compare c = resultHigh.compareComplete(range2.resultLow);

  if (c == WIN_SECOND)
    return true;
  else if (c != WIN_EQUAL)
    return false;
  else
    return (resultLow != resultHigh ||
      range2.resultLow != range2.resultHigh);
}


bool Range::constant() const
{
  return (resultLow == minimum && resultHigh == minimum);
}


void Range::constantResult(Result& result) const
{
  // Either one, as range assumed constant.
  // A lot of copying happening here.  Hopefully the compiler is smart.
  resultLow.constantResult(result);
  // return resultLow.result();
}


string Range::strHeader(const bool rankFlag) const
{
  stringstream ss;
  ss << 
    setw(4) << right << "dist" <<
    minimum.strHeaderEntry(rankFlag, "Min") <<
    resultLow.strHeaderEntry(rankFlag, "Low") <<
    resultHigh.strHeaderEntry(rankFlag, "High") <<
    "\n";
  return ss.str();
}


unsigned char Range::dist() const
{
  return minimum.dist();
}


unsigned char Range::min() const
{
  return minimum.tricks();
}


string Range::str(const bool rankFlag) const
{
  stringstream ss;
  ss << 
    setw(4) << +minimum.dist() <<
    minimum.strEntry(rankFlag) <<
    resultLow.strEntry(rankFlag) <<
    resultHigh.strEntry(rankFlag) <<
    "\n";
  return ss.str();
}

