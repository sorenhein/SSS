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

  public:

    // Turn these private too?
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
