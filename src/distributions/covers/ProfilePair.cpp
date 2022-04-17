/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include "ProfilePair.h"


void ProfilePair::init(const Profile& sumProfile)
{
  lowerProfile.tops.resize(sumProfile.size(), 0);
  upperProfile = sumProfile;

  topNext = 0;
}


void ProfilePair::setLength(
  const unsigned char lenLow,
  const unsigned char lenHigh)
{
  lowerProfile.length = lenLow;
  upperProfile.length = lenHigh;
}


void ProfilePair::addTop(
  const unsigned char topNumber,
  const unsigned char topCountLow,
  const unsigned char topCountHigh)
{
  lowerProfile.tops[topNumber] = topCountLow;
  upperProfile.tops[topNumber] = topCountHigh;
}


bool ProfilePair::active(
  const unsigned char maxValue,
  const unsigned char actualLow,
  const unsigned char actualHigh,
  const unsigned char impliedLow,
  const unsigned char impliedHigh) const
{
  if (actualLow > impliedLow)
  {
    // Either strictly inside, or of the form ">=" where the
    // lower limit is strictly inside.
    if (actualHigh <= impliedHigh)
      return true;
    else if (actualHigh == maxValue)
    {
      // If high, then == would do as well as >=.
      return (actualLow < impliedHigh ? true : false);
    }
    else
      return false;
  }
  else if (actualLow == impliedLow)
  {
    // Strictly inside.
    if (actualHigh < impliedHigh)
      return true;
    else
      return false;
  }
  else if (actualLow == 0)
  {
    if (actualHigh < impliedHigh)
    {
      // If low, then == would do as well as <=.
      return (actualHigh > impliedLow ? true : false);
    }
    else
      return false;
  }
  else
    return false;
}


bool ProfilePair::punchTop(
  const Profile& sumProfile,
  const unsigned char topNumber,
  const unsigned char sumLower,
  const unsigned char sumHigher) const
{
  const unsigned char maxTops = sumProfile.tops[topNumber];

  if (lowerProfile.tops[topNumber] == 0 &&
      upperProfile.tops[topNumber] == maxTops)
  {
    // As the top is unused, it cannot be punched out.
    return false;
  }

  // Find the range of lengths ignoring this top.
  const unsigned char partialSumMin = 
    sumLower - lowerProfile.tops[topNumber];
  const unsigned char partialSumMax = 
    sumHigher - upperProfile.tops[topNumber];

  // Find the bounds on this top assuming the lengths set,
  // but ignoring our actual knowledge of the bounds on the top.
  const unsigned char topMin =
    (lowerProfile.length <= partialSumMax ? 
    0 : lowerProfile.length - partialSumMax);

  const unsigned char topMax =
    (upperProfile.length >= partialSumMin + maxTops ?
    maxTops : upperProfile.length - partialSumMin);

  return ! active(
    maxTops,
    lowerProfile.tops[topNumber],
    upperProfile.tops[topNumber],
    topMin,
    topMax);
}


bool ProfilePair::minimal(
  const Profile& sumProfile,
  const unsigned char sumLower,
  const unsigned char sumHigher) const
{
  // Does the length constraint add anything?
  if (! active(
    sumProfile.length, 
    lowerProfile.length,
    upperProfile.length,
    sumLower, 
    sumHigher))
  {
    return false;
  }

  for (unsigned char t = 1; t <= topNext; t++)
  {
    // Given the length constraint, do we need the top?
    if (punchTop(sumProfile, t, sumLower, sumHigher))
      return false;
  }

  return true;
}


void ProfilePair::getLengthRange(
  unsigned char& sumLower,
  unsigned char& sumHigher) const
{
  sumLower = 0;
  sumHigher = 0;

  for (unsigned n = 0; n < lowerProfile.tops.size(); n++)
  {
    sumLower += lowerProfile.tops[n];
    sumHigher += upperProfile.tops[n];
  }
}


void ProfilePair::incrNextTopNo()
{
  topNext++;
}


unsigned char ProfilePair::getNextTopNo() const
{
  return topNext;
}


unsigned long long ProfilePair::getCode(const Profile& sumProfile) const
{
  return (lowerProfile.getCode() << 32) | 
    upperProfile.getCode(sumProfile, lowerProfile);
}


string ProfilePair::strLines() const
{
  return lowerProfile.strLine() + upperProfile.strLine();
}
