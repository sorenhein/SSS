/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <cassert>

#include "ProfilePair.h"
#include "Product.h"


ProfilePair::ProfilePair()
{
}


ProfilePair::ProfilePair(const Profile& sumProfile)
{
  ProfilePair::init(sumProfile);
}


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
  lowerProfile.lengthInt = lenLow;
  upperProfile.lengthInt = lenHigh;
}


void ProfilePair::setTop(
  const unsigned char topNumber,
  const unsigned char topCountLow,
  const unsigned char topCountHigh)
{
  assert(topNumber < lowerProfile.size());
  lowerProfile.tops[topNumber] = topCountLow;
  upperProfile.tops[topNumber] = topCountHigh;
}


void ProfilePair::setProduct(
  Product& product,
  const Profile& sumProfile,
  const unsigned long long code) const
{
  product.set(sumProfile, lowerProfile, upperProfile, code);
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


unsigned char ProfilePair::getTopLength(const Profile& sumProfile) const
{
  return sumProfile[topNext];
}


unsigned char ProfilePair::getNextTopNo() const
{
  return topNext;
}


void ProfilePair::incrTop()
{
  topNext++;
}


bool ProfilePair::last() const
{
  return (static_cast<unsigned>(topNext+1) >= lowerProfile.size());
}


bool ProfilePair::symmetricAgainst(const Profile& sumProfile) const
{
  return lowerProfile.symmetricAgainst(upperProfile, sumProfile);
}


bool ProfilePair::active(
  const unsigned char maxValue,
  const unsigned char actualLow,
  const unsigned char actualHigh,
  const unsigned char impliedLow,
  const unsigned char impliedHigh) const
{
  // This is the method that eliminates algorithmic duplicates.

  if (actualLow > impliedLow)
  {
    if (actualHigh < impliedHigh)
    {
      /*       |        |        |        |        |        |
       *       |        |        |        |        |        |
       *  impliedLow                                   impliedHigh
       *            actualLow                  actualHigh
       *
       * Strictly inside on both ends.
       */

      return true;
    }
    else if (actualHigh == maxValue)
    {
      /*       |        |        |        |        |        |
       *       |        |        |        |        |        |
       *  impliedLow                          impliedHigh
       *            actualLow                           actualHigh
       *                                                maxValue
       * 
       * Strictly inside on low end.
       * If actualLow == actualHigh, can be written == which is OK.
       * If actualLow == impliedHigh, would be == impliedHigh, not OK.
       * If actualLow <  impliedHigh, can be written >= which is OK.
       */

      return (actualLow == actualHigh || actualLow < impliedHigh ? 
        true : false);
    }
    else if (actualHigh == impliedHigh)
    {
      /*       |        |        |        |        |        |
       *       |        |        |        |        |        |
       *  impliedLow                                   impliedHigh
       *            actualLow                           actualHigh
       *
       * Strictly inside on low end.
       * If actualLow == actualHigh, can be written == which is OK.
       * Otherwise it would be >= which needs actualHigh == maxValue.
       */

      return (actualLow == actualHigh ?  true : false);
    }
    else
      return false;
  }
  else if (actualLow == 0)
  {
    if (actualHigh >= impliedLow && actualHigh < impliedHigh)
    {
      /*       |        |        |        |        |        |
       *       |        |        |        |        |        |
       *            (impliedLow <= actualHigh)         impliedHigh
       *   actualLow                           actualHigh
       *     Zero
       *
       * Strictly inside on high end.
       * If actualLow == actualHigh, can be written == 0 which is OK.
       * If actualHigh == impliedLow, == impliedLow which is not OK.
       */

      return (actualHigh == actualLow || actualHigh > impliedLow ? 
        true : false);
    }
    else
      return false;
  }
  else if (actualLow == impliedLow)
  {
    if (actualHigh < impliedHigh)
    {
      /*       |        |        |        |        |        |
       *       |        |        |        |        |        |
       *     Zero  impliedLow                          impliedHigh
       *            actualLow                  actualHigh
       *
       * Strictly inside on high end.
       * If actualLow == actualHigh, can be written == which is OK.
       * Otherwise can be written <= actualHigh,
       * but then actualLow should have been zero.
       */

      return (actualHigh == actualLow ?  true : false);
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
    (lowerProfile.lengthInt <= partialSumMax ? 
    0 : lowerProfile.lengthInt - partialSumMax);

  const unsigned char topMax =
    (upperProfile.lengthInt >= partialSumMin + maxTops ?
    maxTops : upperProfile.lengthInt - partialSumMin);

  return ! ProfilePair::active(
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
    sumProfile.lengthInt, 
    lowerProfile.lengthInt,
    upperProfile.lengthInt,
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


unsigned char ProfilePair::getCanonicalShift(
  const Profile& sumProfile) const
{
  // Only the basic top is present.
  if (sumProfile.size() == 1)
    return 0;

  for (unsigned char i = 1; i < sumProfile.size(); i++)
  {
    if (lowerProfile.tops[i] != 0 ||
        upperProfile.tops[i] != sumProfile[i])
      return i-1;
  }

  // No tops are used, so we can reduce to the basic one.
  return static_cast<unsigned char>(sumProfile.size() - 1);
}


unsigned long long ProfilePair::getCode(const Profile& sumProfile) const
{
  return (lowerProfile.getLowerCode() << 32) | 
    upperProfile.getUpperCode(sumProfile, lowerProfile);
}


unsigned long long ProfilePair::getCanonicalCode(
  const unsigned long long code,
  const unsigned canonicalShift) const
{
  return
    ((code >> 4*canonicalShift) & 0xffff'ffff'0000'0000) |
    ((code & 0x0000'0000'ffff'ffff) >> 4*canonicalShift);
}


string ProfilePair::strLines() const
{
  return lowerProfile.strLine() + upperProfile.strLine();
}
