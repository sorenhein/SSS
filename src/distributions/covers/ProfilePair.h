#ifndef SSS_PROFILEPAIR_H
#define SSS_PROFILEPAIR_H

#include <algorithm>

#include "Profile.h"

using namespace std;


struct RunningBounds
{
  unsigned char minWest; // Sum of West's top minima
  unsigned char minEast; // Sum of East's top minima
  unsigned char maxDiff; // Largest difference max-min for a top
  unsigned char maxWest; // Largest West maximum
  unsigned char maxEast; // Largest East maximum

  unsigned char length;

  void reset(const unsigned char lengthIn)
  {
    minWest = 0;
    minEast = 0;
    maxDiff = 0;
    maxWest = 0;
    maxEast = 0;

    length = lengthIn;
  };

  void step(
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
  };

  bool busted() const
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
  };

  bool bustedLength(
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
  };

  unsigned char lengthWestLow() const
  {
    return minWest;
  };

  unsigned char lengthWestHigh() const
  {
    return length - minEast;
  };

  string str() const
  {
    stringstream ss;

    ss << "West: " << +minWest << " to " << +maxWest << "\n";
    ss << "East: " << +minEast << " to " << +maxEast << "\n";
    ss << "Diff: " << +maxDiff << "\n";
    
    return ss.str();
  };
};


class ProfilePair
{
  friend class Product;

  private:

    Profile lowerProfile;
    Profile upperProfile;

  public:

    RunningBounds bounds;

    unsigned char topNext; // Running top number

    ProfilePair(const Profile& sumProfile)
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
};

#endif
