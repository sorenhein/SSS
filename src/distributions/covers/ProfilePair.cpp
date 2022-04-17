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


#include <iostream>
bool ProfilePair::active(
  const unsigned char maxValue,
  const unsigned char actualLow,
  const unsigned char actualHigh,
  const unsigned char impliedLow,
  const unsigned char impliedHigh) const
{
/*
cout << "maxValue    " << +maxValue << "\n";
cout << "actualLow   " << +actualLow << "\n";
cout << "actualHigh  " << +actualHigh << "\n";
cout << "impliedLow  " << +impliedLow << "\n";
cout << "impliedHigh " << +impliedHigh << "\n";
*/

  if (actualLow > impliedLow)
  {
    if (actualHigh < impliedHigh)
    {
      /* Strictly inside on both ends:
         impliedLow                    impliedHigh
              I        A            A       I
                   actualLow  actualHigh
       */

// cout << "B0\n";
      return true;
    }
    else if (actualHigh == maxValue)
    {
      /* Strictly inside on low end: Can be written >= actualLow.
         But if actualLow == impliedHigh, can also be written ==.
         impliedLow                    impliedHigh  maxValue
              I        A                    I          A
                   actualLow                       actualHigh
       */

// cout << "B1\n";
      return (actualLow == actualHigh || actualLow < impliedHigh ? 
        true : false);
    }
    else if (actualHigh == impliedHigh)
    {
      /* Strictly inside on low end: Can be written >= actualLow.
         But if actualLow == impliedHigh, can also be written ==.
         impliedLow                    impliedHigh
              I        A                    IA
                   actualLow           actualHigh
       */

      // return true;
// cout << "B2\n";
      // return (actualLow == actualHigh || actualLow < impliedHigh ? 
      return (actualLow == actualHigh ?  true : false);
    }
    else
    {
// cout << "B3\n";
      return false;
    }
  }
  else if (actualLow == 0)
  {
    if (actualHigh >= impliedLow && actualHigh < impliedHigh)
    {
      /* Strictly inside on one end: Can be written <= actualHigh.
         But if actualHigh == impliedLow, can also be written ==.
             Zero                      impliedHigh
              0A                    A       I
         actualLow            actualHigh
       */

// cout << "B4\n";
      return (actualHigh == actualLow || actualHigh > impliedLow ? 
        true : false);
    }
    else
    {
// cout << "B5\n";
      return false;
    }
  }
  else if (actualLow == impliedLow)
  {
    if (actualHigh < impliedHigh)
    {
      /* Strictly inside on high end: Can be written <= actualHigh,
         but then actualLow should have been 0.
         impliedLow                    impliedHigh
              IA                    A       I
         actualLow            actualHigh
       */

      // return true;
      // return (actualLow == 0 ? true : false);
// cout << "B6\n";
      return (actualHigh == actualLow ?  true : false);
    }
    else
    {
// cout << "B7\n";
      return false;
    }
  }
  else
  {
// cout << "B8\n";
    return false;
  }
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


bool ProfilePair::last() const
{
  return (static_cast<unsigned>(topNext+1) >= lowerProfile.size());
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
