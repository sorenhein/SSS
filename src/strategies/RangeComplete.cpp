#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "RangeComplete.h"
#include "Result.h"


void RangeComplete::init(const Result& result)
{
  distribution = result.dist();
  lower = result.tricks();
  upper = result.tricks();
  minimum = result.tricks();

  winnersHigh = result.winners();
  winnersLow = result.winners();

  resultLow = result;
  resultHigh = result;
}


void RangeComplete::extend(const Result& result)
{
  // The range is in general a container for the actual Result's
  // which is as tight as we can make it, but still rounded
  // "outward" when we need to.  Any Result is within its range.

  assert(distribution == result.dist());
  if (result.tricks() < lower)
  {
    minimum = result.tricks();
    lower = result.tricks();
    winnersLow = result.winners();
  }
  else if (result.tricks() == lower)
    winnersLow *= result.winners();

  resultLow *= result;

  if (result.tricks() > upper)
  {
    upper = result.tricks();
    winnersHigh = result.winners();
  }
  else if (result.tricks() == upper)
    winnersHigh += result.winners();

  resultHigh += result;
}


void RangeComplete::operator *= (const RangeComplete& range2)
{
  // To "multiply" two ranges is effectively to take the lowest
  // range (completely), accordingly to some ordering metric.
  // This "lowest" range is used for two purposes:
  // 1. If the range dominates (is below) another within a
  //    set of Strategies (Nodes) with the same parent, then
  //    the dominated range can be removed.  This is only an
  //    optimization, so if we miss a domination, it's not the
  //    end of the world.
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

  if (range2.upper > upper)
    return;

  if (range2.upper < upper || range2.lower < lower)
  {
    lower = range2.lower;
    upper = range2.upper;
    winnersHigh = range2.winnersHigh;
    winnersLow = range2.winnersLow;

    resultLow = range2.resultLow;
    resultHigh = range2.resultHigh;
    return;
  }

  if (range2.lower > lower)
    return;

  // Now the two ranges have the same trick interval, but it may
  // or may not be a constant interval (lower == upper).

  const Compare c = winnersHigh.compareForDeclarer(range2.winnersHigh);
  const Compare c1 = resultHigh.compareCompletely(range2.resultHigh);
  assert(c == c1);

  if (lower < upper)
  {
    // We can just pick one of the ranges.  This is only used for an 
    // optimization anyway.
    if (c == WIN_SECOND || c == WIN_DIFFERENT)
      return;
    else if (c == WIN_FIRST)
    {
      // If declarer prefers the first winner, then the defenders don't.
      winnersHigh = range2.winnersHigh;
      winnersLow = range2.winnersLow;

      resultHigh = range2.resultHigh;
      resultLow = range2.resultLow;
      return;
    }

    // In case of a tie, we prefer the lowest winner on the other end.
    const Compare d = winnersLow.compareForDeclarer(range2.winnersLow);

    if (d == WIN_FIRST)
    {
      // Same idea: We only give up on our range if it loses.
      winnersHigh = range2.winnersHigh;
      winnersLow = range2.winnersLow;

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
    winnersHigh = range2.winnersHigh;
    winnersLow = range2.winnersLow;

    resultHigh = range2.resultHigh;
    resultLow = range2.resultLow;
    return;
  }
  else if (c == WIN_DIFFERENT)
  {
    // Make the extension.
    winnersHigh += range2.winnersHigh;
    winnersLow *= range2.winnersLow;
    
    resultHigh += range2.resultHigh;
    resultLow *= range2.resultLow;
  }

  // Now the high winners are the same.
  const Compare d = winnersLow.compareForDeclarer(range2.winnersLow);
  const Compare d1 = resultLow.compareCompletely(range2.resultLow);
  assert(d == d1);

  if (d == WIN_SECOND || d == WIN_EQUAL)
    return;
  else if (d == WIN_FIRST)
  {
    winnersHigh = range2.winnersHigh;
    winnersLow = range2.winnersLow;
    
    resultHigh = range2.resultHigh;
    resultLow = range2.resultLow;
    return;
  }
  else
  {
    // Make the extension.
    winnersHigh += range2.winnersHigh;
    winnersLow *= range2.winnersLow;
    
    resultHigh += range2.resultHigh;
    resultLow *= range2.resultLow;
  }
}


bool RangeComplete::operator < (const RangeComplete& range2) const
{
  if (upper != range2.lower)
    return (upper < range2.lower);

  // The Winners method is from declarer's perspective, so c is
  // WIN_FIRST if declarer prefers our own winnersHigh.  
  // In this method we are taking the defenders' perspective.
  const Compare c = winnersHigh.compareForDeclarer(range2.winnersLow);
  const Compare c1 = resultHigh.compareCompletely(range2.resultLow);
  assert(c == c1);

  if (c == WIN_SECOND)
    return true;
  else if (c != WIN_EQUAL)
    return false;
  else
    return (range2.lower < range2.upper || lower < upper);
}


bool RangeComplete::constant() const
{
  const bool b1 = (lower == minimum && upper == minimum && resultHigh == resultLow);
  const bool b2 = (lower == minimum && upper == minimum && winnersHigh == winnersLow);
  assert(b1 == b2);

  return (lower == minimum && 
    upper == minimum &&
    winnersHigh == winnersLow);
}


const Winners& RangeComplete::constantWinners() const
{
  return winnersLow;  // Either one, as range assumed constant
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
