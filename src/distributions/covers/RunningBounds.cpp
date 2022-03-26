/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "RunningBounds.h"


void RunningBounds::reset(const unsigned char lengthIn)
{
  minWest = 0;
  minEast = 0;
  maxDiff = 0;
  maxWest = 0;
  maxEast = 0;

  length = lengthIn;
}


void RunningBounds::step(
  const RunningBounds& stackBounds,
  const unsigned char topCountActual,
  const unsigned char topCountLow,
  const unsigned char topCountHigh)
{
  // Step stackBounds forward using the top data.

  const unsigned char minEastIncrement = topCountActual - topCountHigh;
  const unsigned char maxEastIncrement = topCountActual - topCountLow;
  const unsigned char diff = topCountHigh - topCountLow;

  minWest = stackBounds.minWest + topCountLow;
  minEast = stackBounds.minEast + minEastIncrement;
  maxDiff = max(stackBounds.maxDiff, diff);
    
  if (topCountLow == 0 && topCountHigh == topCountActual)
  {
    // The top is not actually used.
    maxWest = stackBounds.maxWest;
    maxEast = stackBounds.maxEast;
  }
  else
  {
    maxWest = max(stackBounds.maxWest, topCountHigh);
    maxEast = max(stackBounds.maxEast, maxEastIncrement);
  }
}


bool RunningBounds::busted() const
{
  if (minWest + maxDiff > length)
  {
    // There is no room for this worst-case single maximum,
    // so we skip the entire set, as there will be a more
    // accurate other set.
    return true;
  }

  if (minEast + maxDiff > length)
    return true;

  if (minWest + minEast > length)
    return true;
  
  return false;
}


bool RunningBounds::unnecessaryLength(
  const unsigned char lengthWestLow,
  const unsigned char lengthWestHigh) const
{
  if (lengthWestHigh < maxWest)
  {
    // There is a tighter way to specify this cover, as the upper
    // West length doesn't even reach the maximum of the single
    // highest top count.
    return true;
  }

  if (length - lengthWestLow < maxEast)
    return true;

  return false;
}


unsigned char RunningBounds::lengthWestLow() const
{
  return minWest;
}


unsigned char RunningBounds::lengthWestHigh() const
{
  return length - minEast;
}


string RunningBounds::str() const
{
  stringstream ss;

  ss << "West: " << +minWest << " to " << +maxWest << "\n";
  ss << "East: " << +minEast << " to " << +maxEast << "\n";
  ss << "Diff: " << +maxDiff << "\n";
  
  return ss.str();
}
