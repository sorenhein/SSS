#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "RangeComplete.h"
#include "Result.h"


void RangeComplete::init(const Result& result)
{
  distribution = result.dist();
  minimum = result.tricks();

  resultLow = result;
  resultHigh = result;
}


void RangeComplete::extend(const Result& result)
{
  // The range is in general a container for the actual Result's
  // which is as tight as we can make it, but still rounded
  // "outward" when we need to.  Any Result is within its range.

  assert(distribution == result.dist());
  if (result.tricks() < minimum)
    minimum = result.tricks();

  resultLow *= result;
  resultHigh += result;
}


void RangeComplete::operator *= (const RangeComplete& range2)
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


  if (range2.minimum < minimum)
    minimum = range2.minimum;
  // minimum *= range2.minimum;

  /*
  const Compare cHigh = resultHigh.compareCompletely(range2.resultHigh);

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

  const Compare cLow = resultLow.compareCompletely(range2.resultLow);

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
  */


/* */
  if (range2.resultHigh.tricks() > resultHigh.tricks())
    return;

  if (range2.resultHigh.tricks() < resultHigh.tricks() || 
      range2.resultLow.tricks() < resultLow.tricks())
  {
    resultLow = range2.resultLow;
    resultHigh = range2.resultHigh;
    return;
  }

  if (range2.resultLow.tricks() > resultLow.tricks())
    return;

  // Now the two ranges have the same trick interval, but it may
  // or may not be a constant interval (lower == upper).

  const Compare c = resultHigh.compareCompletely(range2.resultHigh);

  if (resultLow.tricks() < resultHigh.tricks())
  {
    // We can just pick one of the ranges.  This is only used for an 
    // optimization anyway.
    if (c == WIN_SECOND || c == WIN_DIFFERENT)
      return;
    else if (c == WIN_FIRST)
    {
      // If declarer prefers the first result, then the defenders don't.
      resultHigh = range2.resultHigh;
      resultLow = range2.resultLow;
      return;
    }

    // In case of a tie, we prefer the lowest winner on the other end.
    const Compare d = resultLow.compareCompletely(range2.resultLow);

    if (d == WIN_FIRST)
    {
      // Same idea: We only give up on our range if it loses.
      resultHigh = range2.resultHigh;
      resultLow = range2.resultLow;
    }
    return;
  }

  // Now the two ranges have a constant trick number.  In some
  // case we effectively expand the range as if it had been
  // extend()'ed above.
  // (a) 5N vs (b) 5S becomes (a*b) 5NS to (a|b) 5N/5S.
  // (a) 5N vs (b) 5NS stays 5NS as (b) < (a).
  // (a) 5N vs (b) 5N/5S stays 5N as (a) < (b).
  // (a) 5NS vs (b) 5N/5S stays 5NS as (a) < (b).

  if (c == WIN_SECOND)
    return;
  else if (c == WIN_FIRST)
  {
    resultHigh = range2.resultHigh;
    resultLow = range2.resultLow;
    return;
  }
  else if (c == WIN_DIFFERENT)
  {
    // Make the extension.
    resultHigh += range2.resultHigh;
    resultLow *= range2.resultLow;
  }

  // Now the high winners are the same.
  const Compare d = resultLow.compareCompletely(range2.resultLow);

  if (d == WIN_SECOND || d == WIN_EQUAL)
    return;
  else if (d == WIN_FIRST)
  {
    resultHigh = range2.resultHigh;
    resultLow = range2.resultLow;
    return;
  }
  else
  {
    // Make the extension.
    resultHigh += range2.resultHigh;
    resultLow *= range2.resultLow;
  }
  /* */
}


bool RangeComplete::operator < (const RangeComplete& range2) const
{
  if (resultHigh.tricks() != range2.resultLow.tricks())
    return (resultHigh.tricks() < range2.resultLow.tricks());

  // Compare from declarer's perspective, so c is WIN_FIRST if 
  // declarer prefers our own winnersHigh.  In this method we are 
  // taking the defenders' perspective.
  const Compare c = resultHigh.compareCompletely(range2.resultLow);

  if (c == WIN_SECOND)
    return true;
  else if (c != WIN_EQUAL)
    return false;
  else
    return (range2.resultLow.tricks() < range2.resultHigh.tricks() || 
        resultLow.tricks() < resultHigh.tricks());
}


bool RangeComplete::constant() const
{
  return (resultLow.tricks() == minimum && 
    resultHigh.tricks() == minimum &&
    resultHigh == resultLow);
}


const Result& RangeComplete::constantResult() const
{
  return resultLow;  // Either one, as range assumed constant
}


string RangeComplete::strHeader(const bool rankFlag) const
{
  stringstream ss;
  ss << 
    setw(4) << right << "dist" <<
    setw(4) << "min" << 
    resultLow.strHeaderEntry(rankFlag, "Low") <<
    resultHigh.strHeaderEntry(rankFlag, "High") <<
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


string RangeComplete::str(const bool rankFlag) const
{
  stringstream ss;
  ss << 
    setw(4) << +distribution <<
    setw(4) << +minimum << 
    resultLow.strEntry(rankFlag) <<
    resultHigh.strEntry(rankFlag) <<
    endl;
  return ss.str();
}

