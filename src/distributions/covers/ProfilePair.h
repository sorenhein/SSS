#ifndef SSS_PROFILEPAIR_H
#define SSS_PROFILEPAIR_H

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
