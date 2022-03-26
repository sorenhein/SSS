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

  void reset()
  {
    minWest = 0;
    minEast = 0;
    maxDiff = 0;
    maxWest = 0;
    maxEast = 0;
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

      bounds.reset();
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
