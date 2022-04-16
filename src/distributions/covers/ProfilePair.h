/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PROFILEPAIR_H
#define SSS_PROFILEPAIR_H

#include "Profile.h"
#include "RunningBounds.h"

using namespace std;


class ProfilePair
{
  friend class Product;

  private:

    Profile lowerProfile;
    Profile upperProfile;

    RunningBounds bounds;

    unsigned char topNext; // Running top number


    bool active(
      const unsigned char maxValue,
      const unsigned char actualLow,
      const unsigned char actualHigh,
      const unsigned char impliedLow,
      const unsigned char impliedHigh) const
    {
      if (actualLow > impliedLow)
      {
        if (actualHigh <= impliedHigh || actualHigh == maxValue)
        {
          // Either strictly inside, or of the form ">=" where the
          // lower limit is strictly inside.
          return true;
        }
        else
          return false;
      }
      else if (actualLow == impliedLow || actualLow == 0)
      {
        if (actualHigh < impliedHigh)
          return true;
        else
          return false;
      }
      else
        return false;
    }


    bool punchTop(
      const Profile& sumProfile,
      const unsigned char topNumber,
      const unsigned char sumMin,
      const unsigned char sumMax) const
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
        sumMin - lowerProfile.tops[topNumber];
      const unsigned char partialSumMax = 
        sumMax - upperProfile.tops[topNumber];

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
    };


  public:

    void init(const Profile& sumProfile)
    {
      lowerProfile.tops.resize(sumProfile.size(), 0);
      upperProfile = sumProfile;

      bounds.reset(sumProfile.getLength());
      topNext = 0;
    };


    void setLength(
      const unsigned char lenLow,
      const unsigned char lenHigh)
    {
      lowerProfile.length = lenLow;
      upperProfile.length = lenHigh;
    };


    void addTop(
      const unsigned char topNumber,
      const unsigned char topCountLow,
      const unsigned char topCountHigh)
    {
      lowerProfile.tops[topNumber] = topCountLow;
      upperProfile.tops[topNumber] = topCountHigh;
    };


    bool minimal(
      const Profile& sumProfile,
      const unsigned char topNumber) const
    {
      unsigned char sumMin = 0;
      unsigned char sumMax = 0;

      for (unsigned char n: lowerProfile.tops)
        sumMin += n;

      for (unsigned char n: upperProfile.tops)
        sumMax += n;

      // Does the length constraint add anything?
      if (! active(
        sumProfile.length, 
        lowerProfile.length,
        upperProfile.length,
        sumMin, 
        sumMax))
      {
        return false;
      }

      /* */
      for (unsigned t = 1; t <= topNumber; t++)
      {
        // Given the length constraint, do we need the top?
        if (punchTop(sumProfile, topNumber, sumMin, sumMax))
          return false;
      }
      /* */

      return true;
    };


    unsigned char lengthWestLow() const
    {
      unsigned char sumMin = 0;
      for (unsigned char n: lowerProfile.tops)
        sumMin += n;
      return sumMin;
    };


    unsigned char lengthWestHigh() const
    {
      unsigned char sumMax = 0;
      for (unsigned char n: upperProfile.tops)
        sumMax += n;
      return sumMax;
    };


    void setBounds(const RunningBounds& boundsIn)
    {
      bounds = boundsIn;
    };


    const RunningBounds& getBounds() const
    {
      return bounds;
    };


    void incrNextTopNo()
    {
      topNext++;
    };


    unsigned char getNextTopNo() const
    {
      return topNext;
    };


    unsigned long long getCode(const Profile& sumProfile) const
    {
      return (lowerProfile.getCode() << 32) | 
        upperProfile.getCode(sumProfile, lowerProfile);
    };


    string strLines() const
    {
      return lowerProfile.strLine() + upperProfile.strLine();
    }
};

#endif
