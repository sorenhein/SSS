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


    bool lengthActive(
      const unsigned char maxCards,
      const unsigned char sumMin,
      const unsigned char sumMax) const
    {
      if (lowerProfile.length > sumMin)
      {
        if (upperProfile.length <= sumMax ||
            upperProfile.length == maxCards)
        {
          // Either strictly inside, or of the form ">=" where the
          // lower limit is strictly inside.
          return true;
        }
        else
          return false;
      }
      else if (lowerProfile.length == sumMin ||
          lowerProfile.length == 0)
      {
        if (upperProfile.length < sumMax)
          return true;
        else
          return false;
      }
      else
        return false;
    }


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
      [[maybe_unused]] const unsigned char topNumber) const
    {
      unsigned char sumMin = 0;
      unsigned char sumMax = 0;

      for (unsigned char n: lowerProfile.tops)
        sumMin += n;

      for (unsigned char n: upperProfile.tops)
        sumMax += n;

      if (lengthActive(sumProfile.length, sumMin, sumMax))
        return true;
      else
        return false;
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
